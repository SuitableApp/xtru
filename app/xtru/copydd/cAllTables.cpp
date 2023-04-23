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
#include "cAllTables.h"

#define IOT_TYPE_LEN                      (12+1)
#define DROPPED_LEN                        (3+1)
#define TEMPORARY_LEN                      (1+1)
#define PARTITIONED_LEN                   (3+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllTables::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szIotType[IOT_TYPE_LEN];             // VARCHAR2(12)
    ps::lib::sql::ind_t iIotTypeInd;
    char szDropped[DROPPED_LEN];              // VARCHAR2(3)
    ps::lib::sql::ind_t iDroppedInd;
    char szTemporary[TEMPORARY_LEN];          // VARCHAR2(1)
    ps::lib::sql::ind_t iTemporaryInd;
    char szPartitioned[PARTITIONED_LEN];     // VARCHAR2(3)
    ps::lib::sql::ind_t iPartitionedInd;
    int32_t iNumMBytes;                           // NUMBER
};

const char cAllTables::szCreStmt[] = {
"CREATE TABLE ALL_TABLES\n"
"( OWNER                          TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", IOT_TYPE                       TEXT \n"
", DROPPED                        TEXT \n"
", TEMPORARY                      TEXT \n"
", PARTITIONED                    TEXT \n"
", NUM_MBYTES                     INT NOT NULL \n"
", CONSTRAINT PK_ALL_TABLES PRIMARY KEY\n"
    "( OWNER\n"
    ", TABLE_NAME\n"
    ")\n"
")"
};

const char cAllTables::szDelStmt[] = {
"DELETE FROM ALL_TABLES"
};

const char cAllTables::szInStmt_[] = {
"select OWNER "
", TABLE_NAME "
", IOT_TYPE "
", DROPPED "
", TEMPORARY "
", PARTITIONED "
", nvl((NUM_ROWS * AVG_ROW_LEN)/power(1024,2), 0) as NUM_MBYTES "
"from ALL_TABLES "
"where OWNER in %s "
};

const char cAllTables::szOutStmt_[] = {
"INSERT INTO ALL_TABLES "
"( OWNER "
", TABLE_NAME "
", IOT_TYPE "
", DROPPED "
", TEMPORARY "
", PARTITIONED "
", NUM_MBYTES "
") VALUES (?,?,?,?,?,?,?)"
};

const uint32_t cAllTables::iBulkSize_ = 1000;

cAllTables::cAllTables(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_TABLES", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIotType, SQLT_STR, &rTable_->iIotTypeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDropped, SQLT_STR, &rTable_->iDroppedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTemporary, SQLT_STR, &rTable_->iTemporaryInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitioned, SQLT_STR, &rTable_->iPartitionedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iNumMBytes, SQLT_INT, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szIotType, tLite3Type::STR, &rTable_->iIotTypeInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDropped, tLite3Type::STR, &rTable_->iDroppedInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTemporary, tLite3Type::STR, &rTable_->iTemporaryInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szPartitioned, tLite3Type::STR, &rTable_->iPartitionedInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iNumMBytes, tLite3Type::INT32, NULL, iSkip_, iSkip_);
}

cAllTables::~cAllTables()
{
    delete [] rTable_;
}

void cAllTables::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTables::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


