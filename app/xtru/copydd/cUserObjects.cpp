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

#define STATUS_LEN                   (7+1) /* Length of status. */

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cUserObjects::tAttributes
{
    char szObjectName[OBJECT_NAME_LEN];       // NOT NULL VARCHAR2(30)
    char szSubObjectName[OBJECT_NAME_LEN];    //          VARCHAR2(30)
    ps::lib::sql::ind_t iSubObjectNameInd;
    int32_t iObjectId;                        // NOT NULL NUMBER
    int32_t iDataObjectId;                    // NOT NULL NUMBER
    char szObjectType[OBJECT_TYPE_LEN];       // NOT NULL VARCHAR2(30)
    char szCreated[DATETIME_STR_LEN];         // NOT NULL DATE
    char szLastDdlTime[DATETIME_STR_LEN];     // NOT NULL DATE
    char szStatus[STATUS_LEN];                // NOT NULL VARCHAR2(7)
    char szTemporary[FLAG_LEN];               // NOT NULL VARCHAR2(1)
    char szGenerated[FLAG_LEN];               // NOT NULL VARCHAR2(1)
};

const char cUserObjects::szCreStmt[] = {
"CREATE TABLE USER_OBJECTS\n"
"( OBJECT_NAME                    TEXT NOT NULL\n"
", SUBOBJECT_NAME                 TEXT\n"
", OBJECT_ID                      INT NOT NULL\n"
", DATA_OBJECT_ID                 INT NOT NULL\n"
", OBJECT_TYPE                    TEXT NOT NULL\n"
", CREATED                        TEXT NOT NULL\n"
", LAST_DDL_TIME                  TEXT NOT NULL\n"
", STATUS                         TEXT NOT NULL\n"
", TEMPORARY                      TEXT NOT NULL\n"
", GENERATED                      TEXT NOT NULL\n"
", CONSTRAINT UQ_USER_OBJECTS UNIQUE\n"
    "( OBJECT_NAME, SUBOBJECT_NAME, OBJECT_TYPE\n"
    ")\n"
")"
};

const char cUserObjects::szDelStmt[] = {
"DELETE FROM USER_OBJECTS"
};

const char cUserObjects::szInStmt_[] = {
"select OBJECT_NAME "
", SUBOBJECT_NAME "
", OBJECT_ID "
", DATA_OBJECT_ID "
", OBJECT_TYPE "
", CREATED "
", LAST_DDL_TIME "
", STATUS "
", TEMPORARY "
", GENERATED "
"from USER_OBJECTS "
"order by OBJECT_NAME, SUBOBJECT_NAME "
" /* %s */ "
};

const char cUserObjects::szOutStmt_[] = {
"INSERT INTO USER_OBJECTS "
"( OBJECT_NAME "
", SUBOBJECT_NAME "
", OBJECT_ID "
", DATA_OBJECT_ID "
", OBJECT_TYPE "
", CREATED "
", LAST_DDL_TIME "
", STATUS "
", TEMPORARY "
", GENERATED "
") VALUES (?,?,?,?,?,?,?,?,?,?)"
};

const uint32_t cUserObjects::iBulkSize_ = 1000;


cUserObjects::cUserObjects(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "USER_OBJECTS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // input data
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubObjectName, SQLT_STR, &rTable_->iSubObjectNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iObjectId, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDataObjectId, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCreated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLastDdlTime, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szStatus, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTemporary, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szObjectName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szSubObjectName, tLite3Type::STR, &rTable_->iSubObjectNameInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iObjectId, tLite3Type::INT32, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iDataObjectId, tLite3Type::INT32, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szObjectType, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szCreated, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szLastDdlTime, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szStatus, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTemporary, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szGenerated, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cUserObjects::~cUserObjects()
{
    delete [] rTable_;
}

void cUserObjects::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cUserObjects::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


