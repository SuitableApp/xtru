/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
 *
 * Extreme Unloader(XTRU) is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Extreme Unloader(XTRU) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Extreme Unloader(XTRU).  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <pslib.h>

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cFailover::cFailover()
    : conf_(ps::lib::cConfigures::get_const_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , iNumSecsInterval(10)
    , iTafDelayTime_(conf_.as<int32_t>("taf_delay_time"))
{
    if (iTafDelayTime_ > 300) iTafDelayTime_ = 60;
}

cFailover& cFailover::oAddParam(const tStmtType& iStmtType, const std::string& sKey, const std::string& sValue)
{
    auto it = oMap.find(sKey);
    if (it == oMap.end())
    {
        oMap.insert({sKey, tValue(iStmtType, sValue)});
    }
    else
    {
        mos_ << boost::format("*** WARNING *** Specified %s exists already. Replaced from %s to %s.")
            % sKey % it->second.sGetValue() % sValue
            << std::endl;
        vModParam(sKey, sValue);
    }
    return *this;
}

void cFailover::vModParam(const std::string& sKey, const std::string& sValue)
{
    auto it = oMap.find(sKey);
    BOOST_ASSERT(it != oMap.end());
    it->second.vSetValue(sValue);
}

void cFailover::vDelParam(const std::string& sKey)
{
    auto it = oMap.find(sKey);
    BOOST_ASSERT(it != oMap.end());
    oMap.erase(it);
}

void cFailover::vAlterSession(oracle::occi::Connection *conn, const bool iForce)
{
    std::lock_guard<std::mutex> lk(mtx_);
    std::string sConn = boost::lexical_cast<std::string>(conn);
    std::pair<tHistory::iterator, bool> history
        = oHistMap.insert({sConn, false});
    if (!history.second)
    {
        // if target history exists
        if (iForce) history.first->second = false;
        if (history.first->second) return;
    }
    std::ostringstream oParam;
    std::ostringstream oEvent;
    for (const auto oPair : oMap)
    {
        if (oPair.second.iStmtType_ == STR_PARAM)
        {
            oParam << boost::format(" %s='%s'") % oPair.first % oPair.second.sValue_;
        }
        else if (oPair.second.iStmtType_ == INT_PARAM)
        {
            oParam << boost::format(" %s=%s") % oPair.first % oPair.second.sValue_;
        }
        else
        {
            oEvent << boost::format(" events '%s %s'") % oPair.first % oPair.second.sValue_;
        }
    }
    if (oParam.tellp() > 0)
    {
        trc_ << boost::format("Connection(%s) is altered by:%s") % conn % oParam.str() << std::endl;
        try
        {
            std::string sql_ = "alter session set";
            sql_ += oParam.str();
            ps::lib::sql::occi::tSmartStmt stmt(conn);
            stmt->executeUpdate(sql_);
        }
        catch (const oracle::occi::SQLException& ex)
        {
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
    }
    if (oEvent.tellp() > 0)
    {
        mos_ << boost::format("Connection(%s) is altered by:%s") % conn % oEvent.str() << std::endl;
        try
        {
            std::string sql_ = "alter session set";
            sql_ += oEvent.str();
            ps::lib::sql::occi::tSmartStmt stmt(conn);
            stmt->executeUpdate(sql_);
        }
        catch (const oracle::occi::SQLException& ex)
        {
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
    }
    history.first->second = true;
}

int32_t cFailover::iTafEventHandler(
    oracle::occi::Environment * /*env*/
    , oracle::occi::Connection *conn
    , void *ctx
    , oracle::occi::Connection::FailOverType foType
    , oracle::occi::Connection::FailOverEventType foEvent
){
    static int32_t iRetries = 0;
    int32_t iSecondsForWait = 0;
    int32_t iRet = oracle::occi::OCCI_SUCCESS;
    cFailover *me = static_cast<cFailover *>(ctx);
    std::ostringstream oss;

    switch (foEvent)
    {
    case oracle::occi::Connection::FO_BEGIN:
        oss << "Failing over (fo_type=" <<
            ( (foType==oracle::occi::Connection::FO_NONE) ?    "NONE"
            : (foType==oracle::occi::Connection::FO_SESSION) ? "SESSION"
            : (foType==oracle::occi::Connection::FO_SELECT) ?  "SELECT"
            : "UNKNOWN") << ") ... Please stand by.";
        iSecondsForWait = me->iTafDelayTime_;
        break;
    case oracle::occi::Connection::FO_ABORT:
        oss << "Failover aborted. Failover will not take place.";
        break;
    case oracle::occi::Connection::FO_END:
        oss << "Failover ended ... resuming services. ";
        me->vAlterSession(conn, true);
        break;
    case oracle::occi::Connection::FO_REAUTH:
        oss << "Failed over user. Resuming services.";
        me->vAlterSession(conn, true);
        break;
    case oracle::occi::Connection::FO_ERROR:
        oss << "Failover error gotten. Retried " << ++iRetries << " time(s). Sleeping.";
        iRet = oracle::occi::FO_RETRY;
        iSecondsForWait = me->iNumSecsInterval;
        break;
    default:
        oss << "Bad failover (fo_event=" << foEvent << ").";
        break;
    }
    me->trc_ << boost::format("Connection(%s) ") % conn;
    me->mos_ << oss.str() << std::endl;
    ps::lib::vSleepSecs(iSecondsForWait);
    return iRet;
}

void cFailover::vCheckout(oracle::occi::Connection *conn) 
{
    try
    {
        // Connection was inactive at the time of the failure is activated,
        // the operation to prevent the error returned by the first call after resuming.
        conn->getServerVersion();
    }
    catch (const oracle::occi::SQLException& ex)
    {
        int32_t code = ex.getErrorCode();
        switch (code)
        {
        case 25401:  // can not continue fetches
        case 25402:  // transaction must roll back
        case 25408:  // can not safely replay call
            mos_ << boost::format("Caught oracle::occi::SQLException (ORA-%d), but ignored.")
                % code << std::endl;
            break;
        default:
            RAISE_EX_CONVERT(std::runtime_error, ex.getMessage());
        }
    }
    vAlterSession(conn, true);
    conn->setTAFNotify(&iTafEventHandler, (void *) this);
}

void cFailover::vCheckin(oracle::occi::Connection *conn) 
{
    /* Nothing to do.*/
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps


