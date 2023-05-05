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
#include "cAllIndexes.h"

#define INDEX_TYPE_LEN                      (27+1)
#define UNIQUENESS_LEN                      (9+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllIndexes::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szIndexName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(3)
    char szIndexType[INDEX_TYPE_LEN];         // NOT NULL VARCHAR2(27)
    char szTableOwner[OBJECT_NAME_LEN];       // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szUniqueness[UNIQUENESS_LEN];        // NOT NULL VARCHAR2(9)
};

//  ps::lib::sql::ind_t iUniquenessInd;
const char cAllIndexes::szCreStmt[] = {
"CREATE TABLE ALL_INDEXES\n"
"( OWNER                          TEXT NOT NULL\n"
", INDEX_NAME                     TEXT NOT NULL\n"
", INDEX_TYPE                     TEXT NOT NULL\n"
", TABLE_OWNER                    TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", UNIQUENESS                     TEXT NOT NULL\n"
", CONSTRAINT PK_ALL_INDEXES PRIMARY KEY(OWNER, INDEX_NAME)\n"
")"
};

const char cAllIndexes::szDelStmt[] = {
"DELETE FROM ALL_INDEXES"
};

const char cAllIndexes::szInStmt_[] = {
"select OWNER "
", INDEX_NAME "
", INDEX_TYPE "
", TABLE_OWNER "
", TABLE_NAME "
", UNIQUENESS "
"from ALL_INDEXES "
"where OWNER in %s "
"and DROPPED = 'NO' "
};

const char cAllIndexes::szOutStmt_[] = {
"INSERT INTO ALL_INDEXES "
"( OWNER "
", INDEX_NAME "
", INDEX_TYPE "
", TABLE_OWNER "
", TABLE_NAME "
", UNIQUENESS "
") VALUES (?,?,?,?,?,?)"
};

const uint32_t cAllIndexes::iBulkSize_ = 1000;

cAllIndexes::cAllIndexes(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_INDEXES", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUniqueness, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szIndexName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szIndexType, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szUniqueness, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cAllIndexes::~cAllIndexes()
{
    delete [] rTable_;
}

void cAllIndexes::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllIndexes::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


