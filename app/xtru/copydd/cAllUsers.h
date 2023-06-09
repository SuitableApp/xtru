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

namespace app
{

namespace xtru
{

namespace copydd
{

class cAllUsers
    : public ps::lib::sql::cTransporter
{
private:
    struct tAttributes;
    static const char szInStmt_[];  ///< Inbounding from Oracle.
    static const char szOutStmt_[]; ///< Outbounding to SQLite3.
    static const uint32_t iBulkSize_;
    ps::lib::sql::occi::cBind oBind_;
    ps::lib::sql::occi::cDefine& oDefine_;
    const size_t iSkip_;
    tAttributes * rTable_;
public:
    static const char szCreStmt[]; ///< Creating newly.
    static const char szDelStmt[]; ///< Deleting all rows.
    cAllUsers(
        ps::lib::sql::occi::cSvc& oSvc
        , ps::lib::sql::lite3::cSqliteDb& oDb
    );
private:
    virtual ~cAllUsers();
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    virtual void vPostBulkAction(const uint32_t& iNumIter);
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

