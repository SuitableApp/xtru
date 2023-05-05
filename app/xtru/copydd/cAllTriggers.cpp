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
#include "cAllTriggers.h"

#define TRIGGER_TYPE_LEN                    (16+1)
#define STATUS_LEN                          (8+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllTriggers::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szTriggerName[OBJECT_NAME_LEN];      // NOT NULL VARCHAR2(3)
    char szTriggerType[TRIGGER_TYPE_LEN];     // NOT NULL VARCHAR2(16)
    char szTableOwner[OBJECT_NAME_LEN];       // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szStatus[STATUS_LEN];                // NOT NULL VARCHAR2(8)
};

//  ps::lib::sql::ind_t iUniquenessInd;
const char cAllTriggers::szCreStmt[] = {
"CREATE TABLE ALL_TRIGGERS\n"
"( OWNER                          TEXT NOT NULL\n"
", TRIGGER_NAME                   TEXT NOT NULL\n"
", TRIGGER_TYPE                   TEXT NOT NULL\n"
", TABLE_OWNER                    TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", STATUS                         TEXT NOT NULL\n"
", CONSTRAINT PK_ALL_TRIGGERS PRIMARY KEY(OWNER, TRIGGER_NAME)\n"
")"
};

const char cAllTriggers::szDelStmt[] = {
"DELETE FROM ALL_TRIGGERS"
};

const char cAllTriggers::szInStmt_[] = {
"select OWNER "
", TRIGGER_NAME "
", TRIGGER_TYPE "
", TABLE_OWNER "
", TABLE_NAME "
", STATUS "
"from ALL_TRIGGERS "
"where OWNER in %s "
"and BASE_OBJECT_TYPE = 'TABLE' "
};

const char cAllTriggers::szOutStmt_[] = {
"INSERT INTO ALL_TRIGGERS "
"( OWNER "
", TRIGGER_NAME "
", TRIGGER_TYPE "
", TABLE_OWNER "
", TABLE_NAME "
", STATUS "
") VALUES (?,?,?,?,?,?)"
};

const uint32_t cAllTriggers::iBulkSize_ = 1000;

cAllTriggers::cAllTriggers(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_TRIGGERS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTriggerName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTriggerType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szStatus, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTriggerName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTriggerType, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szStatus, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cAllTriggers::~cAllTriggers()
{
    delete [] rTable_;
}

void cAllTriggers::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTriggers::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


