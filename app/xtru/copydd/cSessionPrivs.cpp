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
#include "cSessionPrivs.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cSessionPrivs::tAttributes
{
    char szPrivilege[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(40)
};

const char cSessionPrivs::szCreStmt[] = {
"CREATE TABLE SESSION_PRIVS\n"
"( PRIVILEGE                      TEXT NOT NULL\n"
", CONSTRAINT PK_SESSION_PRIVS PRIMARY KEY\n"
    "( PRIVILEGE\n"
    ")\n"
")"
};

const char cSessionPrivs::szDelStmt[] = {
"DELETE FROM SESSION_PRIVS"
};

const char cSessionPrivs::szInStmt_[] = {
"select PRIVILEGE "
"from SESSION_PRIVS "
"/* %s */ "
};

const char cSessionPrivs::szOutStmt_[] = {
"INSERT INTO SESSION_PRIVS "
"( PRIVILEGE "
") VALUES (?)"
};

const uint32_t cSessionPrivs::iBulkSize_ = 1000;

cSessionPrivs::cSessionPrivs(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oPrivileges
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "SESSION_PRIVS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oPrivileges) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szPrivilege, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szPrivilege, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cSessionPrivs::~cSessionPrivs()
{
    delete [] rTable_;
}

void cSessionPrivs::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cSessionPrivs::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


