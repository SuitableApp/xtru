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

class cTransporter
    : public ps::lib::sql::cFetchable
    , private ps::lib::sql::occi::cStmt
    , private ps::lib::sql::lite3::cSqliteStmt
{
protected:
    using tLite3Type = ps::lib::sql::lite3::cAttr::tLite3Type;
    ps::lib::cStat& stat_;
private:
    const std::string tag_;
    int64_t iTotalBytes_;
    uint32_t iTotalRows_;
    void vShowStatValue() const;
public:
    /**
     * @brief
     *
     * @param[in] szInStmt
     *   for inbounding from Oracle.
     * @param[in] szOutStmt
     *   for outbounding to SQLite3.
     */
    cTransporter(
        ps::lib::sql::occi::cSvc& oSvc
        , ps::lib::sql::lite3::cSqliteDb& oDb
        , const uint32_t& iBulkSize
        , const std::string& szInStmt
        , const std::string& tag
        , ps::lib::sql::occi::cBind* oIBind
        , const std::string& szOutStmt
    );
    virtual ~cTransporter();
    virtual void vExecuteAndFetch();
    virtual void vPreRepeatAction()
    {}
    virtual void vPostRepeatAction()
    {}
    virtual void vNotFoundAction()
    {}
    virtual void vFeedbackAction();
    virtual void vFinalizeAction();
protected:
    void vAddOutputBytes(const int64_t& iOutputBytes);
    void vAddOutputRows(const int32_t& iOutputRows);
    ps::lib::sql::lite3::cBind& oOBind_;
    using ps::lib::sql::occi::cStmt::oGetDefine;
    using ps::lib::sql::occi::cStmt::vConvPlaceHolder;
    using ps::lib::sql::lite3::cSqliteStmt::iExecute;
    using ps::lib::sql::lite3::cSqliteStmt::oGetDb;
private:
    cTransporter(const cTransporter&) =delete;
    cTransporter& operator=(const cTransporter&) =delete;
};

} // ps::lib::sql

} // ps::lib

} // ps

