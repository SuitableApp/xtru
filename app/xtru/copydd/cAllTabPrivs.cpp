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

#include <pslib.h>
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllTabPrivs::tAttributes
{
    char szGrantor[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(120)
    char szGrantee[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(120)
    char szTableSchema[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(120)
    char szTableName[OBJECT_NAME_LEN];          // NOT NULL VARCHAR2(120)
    char szPrivilege[PRIVILEGE_LEN];            // NOT NULL VARCHAR2(40)
};

const char cAllTabPrivs::szCreStmt[] = {
"CREATE TABLE ALL_TAB_PRIVS\n"
"( GRANTOR                       TEXT NOT NULL\n"
", GRANTEE                       TEXT NOT NULL\n"
", TABLE_SCHEMA                  TEXT NOT NULL\n"
", TABLE_NAME                    TEXT NOT NULL\n"
", PRIVILEGE                     TEXT NOT NULL\n"
", CONSTRAINT PK_ALL_TAB_PRIVS PRIMARY KEY\n"
"   (TABLE_SCHEMA,TABLE_NAME,PRIVILEGE,GRANTEE,GRANTOR)\n"
")"
};

const char cAllTabPrivs::szDelStmt[] = {
"DELETE FROM ALL_TAB_PRIVS"
};

const char cAllTabPrivs::szInStmt_[] = {
"select GRANTOR "
", GRANTEE "
", TABLE_SCHEMA "
", TABLE_NAME "
", PRIVILEGE "
"from ALL_TAB_PRIVS "
"where TABLE_SCHEMA in %s "
};

const char cAllTabPrivs::szOutStmt_[] = {
"INSERT INTO ALL_TAB_PRIVS "
"( GRANTOR "
", GRANTEE"
", TABLE_SCHEMA"
", TABLE_NAME"
", PRIVILEGE"
") VALUES (?,?,?,?,?)"
};

const uint32_t cAllTabPrivs::iBulkSize_ = 1000;

cAllTabPrivs::cAllTabPrivs(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_TAB_PRIVS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szGrantor, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGrantee, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableSchema, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPrivilege, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szGrantor, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szGrantee, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableSchema, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szPrivilege, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cAllTabPrivs::~cAllTabPrivs()
{
    delete [] rTable_;
}

void cAllTabPrivs::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTabPrivs::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


