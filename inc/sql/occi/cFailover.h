/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

class cFailover
{
public:
    enum tStmtType { STR_PARAM, INT_PARAM, EVENTS };
private:
    struct tValue
    {
        tStmtType iStmtType_;
        std::string sValue_;
    public:
        tValue(const tStmtType& iStmtType, const std::string& sValue)
            : iStmtType_(iStmtType), sValue_(sValue)
        {}
        void vSetValue(const std::string& sValue)
        {
            sValue_ = sValue;
        }
        std::string sGetValue() const
        {
            return sValue_;
        }
    };
    const ps::lib::cConfigures& conf_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    const int32_t iNumSecsInterval;
    mutable std::mutex mtx_;
    int32_t iTafDelayTime_; // unit: second
    void vAlterSession(oracle::occi::Connection *conn, const bool iForce);
    static int32_t iTafEventHandler(
        oracle::occi::Environment *env
        , oracle::occi::Connection *conn
        , void *ctx
        , oracle::occi::Connection::FailOverType foType
        , oracle::occi::Connection::FailOverEventType foEvent
    );
    typedef ps::lib::cMap<std::string, tValue> tType;
    tType oMap;
    typedef ps::lib::cMap<std::string, bool> tHistory;
    tHistory oHistMap;
public:
    cFailover();
    cFailover& oAddParam(const tStmtType& iStmtType, const std::string& sKey, const std::string& sValue);
    void vModParam(const std::string& sKey, const std::string& sValue);
    void vDelParam(const std::string& sKey);
    void vCheckout(oracle::occi::Connection *conn) ;
    void vCheckin(oracle::occi::Connection *conn) ;
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

