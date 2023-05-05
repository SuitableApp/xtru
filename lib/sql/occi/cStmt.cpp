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

namespace /* anonymous */
{

ps::lib::cMap<oracle::occi::Statement::Status, std::string> oStatusMap =
{    {oracle::occi::Statement::UNPREPARED            , "UNPREPARED"}
,    {oracle::occi::Statement::PREPARED              , "PREPARED"}
,    {oracle::occi::Statement::RESULT_SET_AVAILABLE  , "RESULT_SET_AVAILABLE"}
,    {oracle::occi::Statement::UPDATE_COUNT_AVAILABLE, "UPDATE_COUNT_AVAILABLE"}
,    {oracle::occi::Statement::NEEDS_STREAM_DATA     , "NEEDS_STREAM_DATA"}
,    {oracle::occi::Statement::STREAM_DATA_AVAILABLE , "STREAM_DATA_AVAILABLE"}
};

} /* anonymous */

void cStmt::vPrepareAndBind()
{
    BOOST_ASSERT(conn_);
    try
    {
        trc_ << boost::format("SQL:[%s]") % sql_ << std::endl;
        mos_ << boost::format("%s; Executing ...") % tag_ << std::endl;
        stmt_.reset(conn_->createStatement(sql_));
        trc_ << boost::format("Status:%s") % oStatusMap[stmt_->status()] << std::endl;
        oOciStmt_.vAddHndl(stmt_.get());
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage() + sql_ + "\n");
    }
    if (oBind_ == nullptr)
    {
        trc_ << boost::format("No host variables are tied up.") << std::endl;
        return;
    }
    // Are there place-holders in the SQL Statement?
    boost::regex pattern(R"(:\w+)"); // a pattern of the place-holder name (phn).
    if ( ! boost::regex_search(sql_, pattern))
    {
        return; // SQL statement has no place-holder.
    }
    ASSERT_OR_RAISE(oBind_->iSize() > 0, std::runtime_error
        , boost::format("SQL-Statement has place-holder(s), but any values are not binded. [%s]") % sql_
    );
    auto it = sql_.cbegin(), ite = sql_.cend();
    ps::lib::cSet<std::string> once_appeared;
    int32_t first_appeared(1);
    // Binding each parameter values to the phn.
    while (it != ite)
    {
        boost::smatch result;
        if ( ! boost::regex_search(it, ite, result, pattern) )
        {
            break;
        }
        std::string phn = result.str();  // "phn" means a place-holder name.
        if (oBind_->find(phn))
        {
            if (once_appeared.find(phn) == once_appeared.end())
            {
                once_appeared.insert(phn);
                std::string sVal = oBind_->sAttachTo(stmt_.get(), phn, once_appeared.size());
                trc_ << boost::format("Binded a value (first_appeared=%d, typ=%s) \"%s\" to the \"%s\".")
                    % first_appeared % oBind_->sGetTypeIdName(phn) % sVal % phn
                    << std::endl;
            }
            else
            {
                /// Nothing to do.
            }
        }
        else
        {
            mos_ << boost::format("%s \"%s\" is not defined. first_appeared=%d")
                % sClass(ps::lib::W) % phn % first_appeared
                << std::endl;
        }
        it = result[0].second;
        first_appeared++;
    }
}

void cStmt::vExecuteQuery()
{
    BOOST_ASSERT(stmt_);
    try
    {
        rs_.reset(stmt_->executeQuery());
        trc_ << boost::format("Status:%s") % oStatusMap[stmt_->status()] << std::endl;
    }
    catch (const oracle::occi::SQLException& ex)
    {
        RAISE_EX_CONVERT(std::runtime_error, ex.getMessage() + sql_ + "\n");
    }
}

void cStmt::vAnalyzeDescribe()
{
    BOOST_ASSERT(rs_);
    auto colList = rs_->getColumnListMetaData();
    auto iNumCols_ = colList.size();

    auto iAclualAllocateSize = 0lu;
    for (auto i = 0lu; i < iNumCols_; ++i)
    {
        ps::lib::sql::occi::cAttr *oAttr = cAttr::oMakeInstance(tag_, oOciStmt_, i + 1, colList[i], iBulkSize_);
        oAttrs_.push_back(oAttr);
        // Analyzing implicit describes and initializing OCCI interface buffer.
        iAclualAllocateSize += oAttr->iGetBufMemSize();
        // Getting the described information.
    }
    if (oDefine_.size() == 0)
    {
        for (auto& oAttr: oAttrs_)
        {
            oAttr.vSetDataBuffer(oDefine_);
        }
    }

    const decltype(iAclualAllocateSize) iMaxAllocateSize_(1 << 20); // 1MBytes
    const auto iTmpBufMemSize = (iMaxAllocateSize_ < iAclualAllocateSize)
        ? iMaxAllocateSize_: iAclualAllocateSize;
        // Excess allocation affects the performance of the normal processing.
        // Too few allocation affects the performance of the rewinding on TAF.
    rs_->setPrefetchMemorySize(iTmpBufMemSize);
    rs_->setPrefetchRowCount(iBulkSize_);
    {
        trc_ << boost::format("%s; ") % tag_ << std::setprecision(3)
            << boost::format(
                "Buffer size=%s Bytes, "
                "Setting PrefetchMemorySize=%s Bytes, "
                "PrefetchRowCount=%d rows")
            % ps::lib::sBinIntToIntStr(iAclualAllocateSize)
            % ps::lib::sBinIntToIntStr(iTmpBufMemSize)
            % ps::lib::sIntToa(iBulkSize_)
            << std::endl;
    }
}

cStmt::cStmt(
    ps::lib::sql::occi::cSvc& oSvc
    , const uint32_t& iBulkSize
    , const std::string& sql
    , const std::string& tag
    , ps::lib::sql::occi::cBind* oBind
)
    : rtn_(ps::lib::cRtn::get_const_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , oSvc_(oSvc)
    , iBulkSize_(iBulkSize)
    , sql_(sql)
    , tag_(tag)
    , oBind_(oBind)
    , oDefine_(ps::lib::sql::occi::cDefine::tTiming::iRepeat)
    , conn_(nullptr, ps::lib::sql::occi::cConnDeleter(oSvc_))
    , stmt_(nullptr, ps::lib::sql::occi::cStmtDeleter(conn_))
    , rs_(nullptr, ps::lib::sql::occi::cRsDeleter(stmt_))
    , iFeedBack_(conf_.as<int32_t>("feedback"))
    , iFetchHasDone_(false)
{
    BOOST_ASSERT(iBulkSize_);
    BOOST_ASSERT(sql_.size());
    BOOST_ASSERT(tag_.size());
}

cStmt::~cStmt()
{
#ifndef NDEBUG
    trc_ << boost::format("%s; %s") % __PRETTY_FUNCTION__ % tag_ << std::endl;
#endif
}

void cStmt::vConvPlaceHolder(
    const ps::lib::str_vct& opts
){
    BOOST_ASSERT_MSG(sql_.size() != 0, "Call CTOR first and initialize the member sql_.");
    ps::lib::vConvPlaceHolder(opts, sql_, trc_);
}

void cStmt::vExecute()
{
    conn_.reset(oSvc_.oGetCon());
    vPrepareAndBind();
    vExecuteQuery();
    vAnalyzeDescribe();
}

/**
 * @details
 *
 * @note
 * - On the reference guide of OCCI, oracle::occi::ResultSet class is used
 *   to retrieve the data set, but not do so in this implementation.
 * - This is because the oracle::occi::Statement class does not have
 *   a member function which attaches structure array of user-defined
 *   to the selection statement like the OCI.
 * - Instead, in order to handle the structure array of user-defined,
 *   the use of [OCIStmtFetch2](https://docs.oracle.com/database/121/LNOCI/oci17msc001.htm#LNOCI-GUID-DF585B90-58BA-45FC-B7CE-6F7F987C03B9)
 *   and [OCIDefineArrayOfStruct](https://docs.oracle.com/database/121/LNOCI/oci16rel003.htm#LNOCI-GUID-D3F74828-5DA7-48AD-A170-120AD547DEB3)
 *   were selected.
 * - ps::lib::sql::occi::iStmtFetch2 makes it possible to store data
 *   in the user defined definition array.
 */
uint32_t cStmt::iFetch(
    ps::lib::sql::cFetchable& fetchable
    , std::exception_ptr& ep
)
try 
{
    BOOST_SCOPE_EXIT(&rs_, &stmt_, &conn_)
    {
        rs_.reset();
        stmt_.reset();
        conn_.reset();
    } BOOST_SCOPE_EXIT_END
    auto iTotalRows = 0LU;
    BOOST_ASSERT(oOciStmt_.oGetOciSvcCtx());
    BOOST_ASSERT(oDefine_.size());
    if (oDefine_.iGetTiming() == ps::lib::sql::occi::cDefine::tTiming::iRepeat)
    {
        oDefine_.vAttachTo(oOciStmt_); // attaches host memory to SQL statement.
    }

    ub4 iNextFeedback = (iFeedBack_ * iBulkSize_);
    sword iOciRtn = OCI_SUCCESS;
    while (rtn_.iCotinue() && iOciRtn != OCI_NO_DATA)
    {
        fetchable.vPreBulkAction(iBulkSize_);
        if (oDefine_.iGetTiming() == ps::lib::sql::occi::cDefine::tTiming::iOnce)
        {
            oDefine_.vAttachTo(oOciStmt_); // attaches host memory to SQL statement.
        }
        iOciRtn = iStmtFetch2(oOciStmt_, iBulkSize_, sql_);
        const auto iNumIter = getNumArrayRows(oOciStmt_);
        if (0 == iNumIter) continue;
        fetchable.vPostBulkAction(iNumIter);
        iTotalRows += iNumIter;
        if (iFeedBack_ > 0 && iTotalRows >= iNextFeedback)
        {
            fetchable.vFeedbackAction();
            iNextFeedback = iFeedBack_ * iBulkSize_ + iTotalRows;
        }
    }
    iFetchHasDone_ = true;
    return iTotalRows;
}
catch (...)
{
    // It would be sufficient if we could only distinguish one exception
    // which is caught earliest.
    if (ep == nullptr)
    {
        ep = std::current_exception();
    }
    trc_ << boost::format("SQL:[%s]") % sql_ << std::endl;
    return 0LU;
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

