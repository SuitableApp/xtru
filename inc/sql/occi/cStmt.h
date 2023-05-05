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

#pragma once

namespace ps
{

namespace lib
{

namespace sql
{

class cFetchable;

namespace occi
{

/// This implements details for communicating with Oracle Database via OCCI.
/**
 * detail
 */
class cStmt
{
protected:
    const ps::lib::cRtn& rtn_;
    const ps::lib::cConfigures& conf_;

private:
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    ps::lib::sql::occi::cSvc& oSvc_;
    ps::lib::sql::occi::cOciStmt oOciStmt_;
    uint32_t iBulkSize_;
    std::string sql_;
    std::string tag_;
    cBind* oBind_; // Must be a pointer, not a reference.
    cDefine oDefine_;
    std::unique_ptr<oracle::occi::Connection, ps::lib::sql::occi::cConnDeleter> conn_;
    std::unique_ptr<oracle::occi::Statement, ps::lib::sql::occi::cStmtDeleter> stmt_;
    std::unique_ptr<oracle::occi::ResultSet, ps::lib::sql::occi::cRsDeleter> rs_;
    int32_t iFeedBack_;
    bool iFetchHasDone_;
    ps::lib::sql::occi::cAttr::tContainer oAttrs_; ///< stores retrieved data from SQL select

private:
    cStmt(const cStmt&) =delete;
    cStmt& operator=(const cStmt&) =delete;
    void vPrepareAndBind();
    void vExecuteQuery();
    void vAnalyzeDescribe();

public:
    cStmt(
        ps::lib::sql::occi::cSvc& oSvc
        , const uint32_t& iBulkSize
        , const std::string& sql
        , const std::string& tag
        , ps::lib::sql::occi::cBind* oBind
    );
    ~cStmt();
    /**
     * Place-holders (=%s) in SQL-statement will be replaced
     * by given "opts" which is arraied string.
     *
     * @param[in] opts
     *   stores values for %s place-holder in sql_.
     */
    void vConvPlaceHolder(
        const ps::lib::str_vct& opts
    );
    /**
     * @brief
     * - for DML and PL/SQL
     * - executable A reference to instance which can be executed
     */
    void vExecute();
    /**
     * @brief
     *
     * @param [in,out] fetchable
     * - A reference to instance which can be fetched.
     *   cFetchable has following three virtual functions:
     *   -# cFetchable::vPreBulkAction  : before fetching
     *      with the number of rows to be fetched.
     *   -# cFetchable::vPostBulkAction : after fetching
     *      with the number of rows actually fetched.
     *   -# cFetchable::vFeedbackAction : after fetching and cStmt::iFeedBack_ > 0
     *     : To notify the timing when the number of rows of
     *      cStmt::iFeedBack_ * cStmt::iBulkSize_ has been newly fetched.
     * - If virtual functions are defined in a user-defined class that
     *   inherits cFetchable, you can call back these at the timing
     *   before and after each fetch operation.
     * @param [in,out] ep
     *   Share exceptions that occurred in the current thread
     *   with the calling thread
     */
    uint32_t iFetch(
        ps::lib::sql::cFetchable& fetchable
        , std::exception_ptr& ep =boost::value_initialized<std::exception_ptr>()
    );
    cDefine& oGetDefine()
    {
        return oDefine_;
    }
    bool iGetFetchHasDone() const
    {
        return iFetchHasDone_;
    }
    const ps::lib::sql::occi::cAttr::tContainer& oGetAttrs() const
    {
        return oAttrs_;
    }
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

