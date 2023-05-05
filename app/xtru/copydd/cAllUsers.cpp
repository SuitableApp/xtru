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
#include "cAllUsers.h"

#define UPDATABLE_LEN                      (1+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllUsers::tAttributes
{
    char szUserName[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
};

const char cAllUsers::szCreStmt[] = {
"CREATE TABLE ALL_USERS\n"
"( USERNAME                       TEXT NOT NULL\n"
", CONSTRAINT PK_ALL_USERS PRIMARY KEY(USERNAME)\n"
")"
};

const char cAllUsers::szDelStmt[] = {
"DELETE FROM ALL_USERS"
};

const char cAllUsers::szInStmt_[] = {
"select USERNAME "
"from ALL_USERS "
};

const char cAllUsers::szOutStmt_[] = {
"INSERT INTO ALL_USERS "
"( USERNAME "
") VALUES (?)"
};

const uint32_t cAllUsers::iBulkSize_ = 1000;

cAllUsers::cAllUsers(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_USERS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // input data
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szUserName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szUserName, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cAllUsers::~cAllUsers()
{
    delete [] rTable_;
}

void cAllUsers::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllUsers::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


