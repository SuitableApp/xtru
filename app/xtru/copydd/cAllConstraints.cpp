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
#include "cAllConstraints.h"
#include "tAllConstraints.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

const char cAllConstraints::szCreStmt[] = {
"CREATE TABLE ALL_CONSTRAINTS\n"
"( OWNER                          TEXT NOT NULL\n"
", CONSTRAINT_NAME                TEXT NOT NULL\n"
", CONSTRAINT_TYPE                TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", SEARCH_CONDITION               TEXT\n"
", R_OWNER                        TEXT\n"
", R_CONSTRAINT_NAME              TEXT\n"
", DELETE_RULE                    TEXT\n"
", STATUS                         TEXT NOT NULL\n"
/* DEFERRABLE is reserved keyword in Sqlite3. */
", _DEFERRABLE                    TEXT NOT NULL\n"
", DEFERRED                       TEXT NOT NULL\n"
", VALIDATED                      TEXT NOT NULL\n"
", GENERATED                      TEXT NOT NULL\n"
", INDEX_OWNER                    TEXT\n"
", INDEX_NAME                     TEXT\n"
", CONSTRAINT PK_ALL_CONSTRAINTS PRIMARY KEY\n"
    "( OWNER\n"
    ", CONSTRAINT_NAME\n"
    ")\n"
")"
};

const char cAllConstraints::szIdxStmt[] = {
"CREATE INDEX ALL_CONSTRAINTS_IE01 ON ALL_CONSTRAINTS "
"(R_OWNER, R_CONSTRAINT_NAME)"
};

const char cAllConstraints::szDelStmt[] = {
"DELETE FROM ALL_CONSTRAINTS"
};

const char cAllConstraints::szInStmt_[] = {
"select owner, constraint_name, constraint_type "
", table_name, search_condition, r_owner "
", r_constraint_name, delete_rule, status "
", deferrable, deferred, validated, generated "
"%s "
"from ALL_CONSTRAINTS "
"where owner in %s "
"and table_name not like 'BIN$%' "
"order by constraint_name "
};

const char cAllConstraints::szAplddT03_r9_[] = {
", nvl(index_owner, owner), index_name "
};

const char cAllConstraints::szAplddT03_r8_[] = {
", owner as index_owner, constraint_name as index_name "
};

const char cAllConstraints::szOutStmt_[] = {
"INSERT INTO ALL_CONSTRAINTS "
"( OWNER, CONSTRAINT_NAME, CONSTRAINT_TYPE "
", TABLE_NAME, SEARCH_CONDITION, R_OWNER "
", R_CONSTRAINT_NAME, DELETE_RULE, STATUS "
", _DEFERRABLE, DEFERRED, VALIDATED, GENERATED "
", INDEX_OWNER, INDEX_NAME"
") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
};

const uint32_t cAllConstraints::iBulkSize_ = 10;

cAllConstraints::cAllConstraints(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_CONSTRAINTS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({std::string(szAplddT03_r9_), sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSearchCondition, SQLT_STR, &rTable_->nSearchConditionInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szROwner, SQLT_STR, &rTable_->nROwnerInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRConstraintName, SQLT_STR, &rTable_->nRConstraintNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeleteRule, SQLT_STR, &rTable_->nDeleteRuleInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szStatus, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexOwner, SQLT_STR, &rTable_->nIndexOwnerInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, &rTable_->nIndexNameInd, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szConstraintName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szConstraintType, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szSearchCondition, tLite3Type::STR, &rTable_->nSearchConditionInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szROwner, tLite3Type::STR, &rTable_->nROwnerInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szRConstraintName, tLite3Type::STR, &rTable_->nRConstraintNameInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDeleteRule, tLite3Type::STR, &rTable_->nDeleteRuleInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szStatus, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDeferrable, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDeferred, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szValidated, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szGenerated, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szIndexOwner, tLite3Type::STR, &rTable_->nIndexOwnerInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szIndexName, tLite3Type::STR, &rTable_->nIndexNameInd, iSkip_, iSkip_);
}

cAllConstraints::~cAllConstraints()
{
    delete [] rTable_;
}

void cAllConstraints::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllConstraints::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


