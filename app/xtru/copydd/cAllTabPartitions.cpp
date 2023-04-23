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
#include "cAllTabPartitions.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllTabPartitions::tAttributes
{
    char szTableOwner[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];         // NOT NULL VARCHAR2(30)
    char szPartitionName[OBJECT_NAME_LEN];     // NOT NULL VARCHAR2(30)
    int32_t iNumRows;                              // NUMBER
    int32_t iAvgRowLen;                            // NUMBER
};

const char cAllTabPartitions::szCreStmt[] = {
"CREATE TABLE ALL_TAB_PARTITIONS\n"
"( TABLE_OWNER                    TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", PARTITION_NAME                 TEXT NOT NULL\n"
", NUM_ROWS                       INT NOT NULL\n"
", AVG_ROW_LEN                    INT NOT NULL\n"
", CONSTRAINT PK_ALL_TAB_PARTITIONS PRIMARY KEY\n"
    "( TABLE_OWNER\n"
    ", TABLE_NAME\n"
    ", PARTITION_NAME\n"
    ")\n"
")"
};

const char cAllTabPartitions::szDelStmt[] = {
"DELETE FROM ALL_TAB_PARTITIONS"
};

const char cAllTabPartitions::szInStmt_[] = {
"select TABLE_OWNER "
", TABLE_NAME "
", PARTITION_NAME "
", nvl(NUM_ROWS, 0) as NUM_ROWS "
", nvl(AVG_ROW_LEN, 0) as AVG_ROW_LEN "
"from ALL_TAB_PARTITIONS "
"where TABLE_OWNER in %s "
};

const char cAllTabPartitions::szOutStmt_[] = {
"INSERT INTO ALL_TAB_PARTITIONS "
"( TABLE_OWNER "
", TABLE_NAME "
", PARTITION_NAME "
", NUM_ROWS "
", AVG_ROW_LEN "
") VALUES (?,?,?,?,?)"
};

const uint32_t cAllTabPartitions::iBulkSize_ = 1000;

cAllTabPartitions::cAllTabPartitions(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_TAB_PARTITIONS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szTableOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iNumRows, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iAvgRowLen, SQLT_INT, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szTableOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szPartitionName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iNumRows, tLite3Type::INT32, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iAvgRowLen, tLite3Type::INT32, NULL, iSkip_, iSkip_);
}

cAllTabPartitions::~cAllTabPartitions()
{
    delete [] rTable_;
}

void cAllTabPartitions::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTabPartitions::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


