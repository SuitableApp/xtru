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
#include "cEffectiveCons.h"
#include "tAllConstraints.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

const char cEffectiveCons::szCreStmt[] = {
/* Old name is APLDDT03 */
"CREATE TABLE EFFECTIVE_CONS\n"
"( OWNER                          TEXT NOT NULL\n"
", CONSTRAINT_NAME                TEXT NOT NULL\n"
", CONSTRAINT_TYPE                TEXT\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", SEARCH_CONDITION               TEXT\n"
", R_OWNER                        TEXT\n"
", R_CONSTRAINT_NAME              TEXT\n"
", DELETE_RULE                    TEXT\n"
", STATUS                         TEXT\n"
/* DEFERRABLE is reserved keyword in Sqlite3. */
", _DEFERRABLE                    TEXT\n"
", DEFERRED                       TEXT\n"
", VALIDATED                      TEXT\n"
", GENERATED                      TEXT\n"
", INDEX_OWNER                    TEXT\n"
", INDEX_NAME                     TEXT\n"
", CONSTRAINT PK_EFFECTIVE_CONS PRIMARY KEY\n"
    "( OWNER, CONSTRAINT_NAME\n"
    ")\n"
")"
};

const char cEffectiveCons::szIdx1Stmt[] = {
"CREATE INDEX EFFECTIVE_CONS_IE01 ON EFFECTIVE_CONS (TABLE_NAME)"
};

const char cEffectiveCons::szIdx2Stmt[] = {
"CREATE INDEX EFFECTIVE_CONS_IE02 ON EFFECTIVE_CONS (CONSTRAINT_NAME)"
};

const std::string cEffectiveCons::Z = {
"select OWNER, CONSTRAINT_NAME, CONSTRAINT_TYPE "
", TABLE_NAME, SEARCH_CONDITION, R_OWNER "
", R_CONSTRAINT_NAME, DELETE_RULE, STATUS "
", _DEFERRABLE, DEFERRED, VALIDATED, GENERATED "
", INDEX_OWNER, INDEX_NAME "
"from EFFECTIVE_CONS  "
};
const std::string cEffectiveCons::A = {
"select OWNER, CONSTRAINT_NAME, CONSTRAINT_TYPE "
", TABLE_NAME, SEARCH_CONDITION, R_OWNER "
", R_CONSTRAINT_NAME, DELETE_RULE, STATUS "
", _DEFERRABLE, DEFERRED, VALIDATED, GENERATED "
", INDEX_OWNER, INDEX_NAME "
"from ALL_CONSTRAINTS  "
};
const std::string cEffectiveCons::B = {
"select t0.OWNER, t0.CONSTRAINT_NAME, t0.CONSTRAINT_TYPE "
", t0.TABLE_NAME, t0.SEARCH_CONDITION, t0.R_OWNER "
", t0.R_CONSTRAINT_NAME, t0.DELETE_RULE, t0.STATUS "
", t0._DEFERRABLE, t0.DEFERRED, t0.VALIDATED, t0.GENERATED "
", t0.INDEX_OWNER, t0.INDEX_NAME "
"from ALL_CONSTRAINTS t0 "
"where ( "
"(exists (select null "
"from TARGET_TABLES "
"where OWNER = t0.OWNER "
"and TABLE_NAME = t0.TABLE_NAME "
")) or "
"(exists (select null "
"from TARGET_TABLES t1, ALL_CONSTRAINTS t2 "
"where t2.OWNER = t1.OWNER "
"and t2.TABLE_NAME = t1.TABLE_NAME "
"and t2.CONSTRAINT_TYPE in ('P', 'U') "
"and t2.OWNER = t0.R_OWNER "
"and t2.CONSTRAINT_NAME = t0.R_CONSTRAINT_NAME) "
"and t0.CONSTRAINT_TYPE = 'R' "
"))"
};

const uint32_t cEffectiveCons::iBulkSize_ = 1;
const std::string cEffectiveCons::tag_ = "EFFECTIVE_CONS";
const size_t cEffectiveCons::iSkip_ = sizeof(tAttributes);

cEffectiveCons::cEffectiveCons(ps::lib::sql::lite3::cSqliteDb& oDb, const std::string& szStmt)
    : oDb_(oDb)
{
    tAttributes rRowBuf;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_, szStmt);
    ASSERT_OR_RAISE_FNC(
        oStmt.iParse() == SQLITE_OK
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    // tells user memory addresses to ps::lib::sql::lite3::cSqliteStmt
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwner, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szConstraintName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szConstraintType, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szTableName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szSearchCondition, cAttr::STR, &rRowBuf.nSearchConditionInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szROwner, cAttr::STR, &rRowBuf.nROwnerInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szRConstraintName, cAttr::STR, &rRowBuf.nRConstraintNameInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szDeleteRule, cAttr::STR, &rRowBuf.nDeleteRuleInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szStatus, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szDeferrable, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szDeferred, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szValidated, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szGenerated, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szIndexOwner, cAttr::STR, &rRowBuf.nIndexOwnerInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szIndexName, cAttr::STR, &rRowBuf.nIndexNameInd, iSkip_, iSkip_);
    // fetches values.
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&] { oContainer_.push_back(rRowBuf); }
        , [&] {} // pre-fetch-rows action.
        , [&] {} // post-fetch-rows action.
        , [&] {} // not found action.
        , [&]
        {
            // The following sort function will call a comparison function what
            // is defined at "tAllConstraints.h" as
            // bool tAllConstraints::operator<(const tAllConstraints& rhs) const;
            oContainer_.sort();
        }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

cEffectiveCons::cEffectiveCons(const cEffectiveCons& rhs)
    : oDb_(rhs.oDb_), oContainer_(rhs.oContainer_)
{}

cEffectiveCons::~cEffectiveCons()
{}

cEffectiveCons& cEffectiveCons::operator-=(const cEffectiveCons& rhs)
{
    std::list<tAttributes> oTemp;
    oContainer_.swap(oTemp);
    std::set_difference(
        oTemp.cbegin(), oTemp.cend()
        , rhs.oContainer_.begin(), rhs.oContainer_.end()
        , std::back_inserter(oContainer_)
    );
    return *this;
}

void cEffectiveCons::vSqlDel() const
{
    const int32_t iNumIter = oContainer_.size();
    if (0 == iNumIter) return;
    std::unique_ptr<tAttributes[]> rRowBuf(new tAttributes[iNumIter]);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "DELETE FROM EFFECTIVE_CONS "
        "WHERE OWNER = ? "
        "AND CONSTRAINT_NAME = ? "
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    ps::lib::sql::lite3::cBind& oOBind(oStmt.oGetBind());
    using ps::lib::sql::lite3::cAttr;
    oOBind.vAddItem(rRowBuf[0].szOwner, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szConstraintName, cAttr::STR, NULL, iSkip_, iSkip_);
    int32_t i = 0;
    for (const auto& oItem : oContainer_)
    {
        ::memcpy(rRowBuf[i].szOwner, oItem.szOwner, sizeof(rRowBuf[0].szOwner));
        ::memcpy(rRowBuf[i].szConstraintName, oItem.szConstraintName, sizeof(rRowBuf[0].szConstraintName));
        ++i;
    }
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

void cEffectiveCons::vSqlIns() const
{
    const int32_t iNumIter = oContainer_.size();
    if (0 == iNumIter) return;
    std::unique_ptr<tAttributes[]> rRowBuf(new tAttributes[iNumIter]);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "insert into EFFECTIVE_CONS "
        "( OWNER, CONSTRAINT_NAME, CONSTRAINT_TYPE "
        ", TABLE_NAME, SEARCH_CONDITION, R_OWNER "
        ", R_CONSTRAINT_NAME, DELETE_RULE, STATUS "
        ", _DEFERRABLE, DEFERRED, VALIDATED, GENERATED "
        ", INDEX_OWNER, INDEX_NAME "
        ") values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    ps::lib::sql::lite3::cBind& oOBind(oStmt.oGetBind());
    using ps::lib::sql::lite3::cAttr;
    oOBind.vAddItem(rRowBuf[0].szOwner, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szConstraintName, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szConstraintType, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szTableName, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szSearchCondition, cAttr::STR, &rRowBuf[0].nSearchConditionInd, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szROwner, cAttr::STR, &rRowBuf[0].nROwnerInd, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szRConstraintName, cAttr::STR, &rRowBuf[0].nRConstraintNameInd, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szDeleteRule, cAttr::STR, &rRowBuf[0].nDeleteRuleInd, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szStatus, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szDeferrable, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szDeferred, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szValidated, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szGenerated, cAttr::STR, NULL, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szIndexOwner, cAttr::STR, &rRowBuf[0].nIndexOwnerInd, iSkip_, iSkip_);
    oOBind.vAddItem(rRowBuf[0].szIndexName, cAttr::STR, &rRowBuf[0].nIndexNameInd, iSkip_, iSkip_);
    int32_t i = 0;
    for (const auto& oItem : oContainer_)
    {
        ::memcpy(&rRowBuf[i], &oItem, sizeof(tAttributes));
        ++i;
    }
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

