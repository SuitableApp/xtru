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
#include "cAllObjects.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllObjects::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szObjectName[OBJECT_NAME_LEN];       // NOT NULL VARCHAR2(30)
    char szSubObjectName[OBJECT_NAME_LEN];    // NOT NULL VARCHAR2(30)
    ps::lib::sql::ind_t iSubObjectNameInd;
    char szObjectType[OBJECT_TYPE_LEN];       // NOT NULL VARCHAR2(30)
};

const char cAllObjects::szCreStmt[] = {
"CREATE TABLE ALL_OBJECTS\n"
"( OWNER                          TEXT NOT NULL\n"
", OBJECT_NAME                    TEXT NOT NULL\n"
", SUBOBJECT_NAME                 TEXT \n"
", OBJECT_TYPE                    TEXT NOT NULL\n"
", CONSTRAINT UQ_ALL_OBJECTS UNIQUE\n"
    "( OWNER\n"
    ", OBJECT_NAME\n"
    ", SUBOBJECT_NAME\n"
    ", OBJECT_TYPE\n"
    ")\n"
")"
};

const char cAllObjects::szDelStmt[] = {
"DELETE FROM ALL_OBJECTS"
};

const char cAllObjects::szInStmt_[] = {
"select OWNER "
", OBJECT_NAME "
", SUBOBJECT_NAME "
", OBJECT_TYPE "
"from ALL_OBJECTS "
"where OWNER in %s "
};

const char cAllObjects::szOutStmt_[] = {
"INSERT INTO ALL_OBJECTS "
"( OWNER "
", OBJECT_NAME "
", SUBOBJECT_NAME "
", OBJECT_TYPE "
") VALUES (?,?,?,?)"
};

const uint32_t cAllObjects::iBulkSize_ = 1000;


cAllObjects::cAllObjects(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_OBJECTS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // input data
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubObjectName, SQLT_STR, &rTable_->iSubObjectNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szObjectName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szSubObjectName, tLite3Type::STR, &rTable_->iSubObjectNameInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szObjectType, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cAllObjects::~cAllObjects()
{
    delete [] rTable_;
}

void cAllObjects::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllObjects::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


