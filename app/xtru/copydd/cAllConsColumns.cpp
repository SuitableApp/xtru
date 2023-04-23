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

struct cAllConsColumns::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // VARCHAR2(30)
    char szConstraintName[OBJECT_NAME_LEN];   // VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // VARCHAR2(30)
    char szColumnName[COLUMN_NAME_LEN];       // VARCHAR2(4000)
    int32_t iPosition;                            // NUMBER
    ps::lib::sql::ind_t nPositionInd; // When constraint_type is not 'P' or 'U'
                                 // , it retrieves ps::lib::sql::ind_t::VAL_IS_NULL.
};

const char cAllConsColumns::szCreStmt[] = {
"CREATE TABLE ALL_CONS_COLUMNS\n"
"( OWNER                          TEXT NOT NULL\n"
", CONSTRAINT_NAME                TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", COLUMN_NAME                    TEXT NOT NULL\n"
", POSITION                       INT\n"
", CONSTRAINT PK_ALL_CONS_COLUMNS PRIMARY KEY\n"
    "( OWNER\n"
    ", CONSTRAINT_NAME\n"
    ", COLUMN_NAME\n"
    ")\n"
")"
};

const char cAllConsColumns::szDelStmt[] = {
"DELETE FROM ALL_CONS_COLUMNS"
};

const char cAllConsColumns::szInStmt_[] = {
"select OWNER "
", CONSTRAINT_NAME "
", TABLE_NAME "
", COLUMN_NAME "
", POSITION "
"from ALL_CONS_COLUMNS "
"where owner in %s "
};

const char cAllConsColumns::szOutStmt_[] = {
"INSERT INTO ALL_CONS_COLUMNS "
"( OWNER, CONSTRAINT_NAME "
", TABLE_NAME, COLUMN_NAME, POSITION "
") VALUES (?,?,?,?,?)"
};

const uint32_t cAllConsColumns::iBulkSize_ = 100;


cAllConsColumns::cAllConsColumns(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_CONS_COLUMNS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPosition, SQLT_INT, &rTable_->nPositionInd, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szConstraintName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szColumnName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iPosition, tLite3Type::INT32, &rTable_->nPositionInd, iSkip_, iSkip_);
}

cAllConsColumns::~cAllConsColumns()
{
    delete [] rTable_;
}

void cAllConsColumns::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllConsColumns::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

