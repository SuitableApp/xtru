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
#include "cAllMviews.h"

#define UPDATABLE_LEN                      (1+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllMviews::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szMviewName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szUpdatable[UPDATABLE_LEN];          // VARCHAR2(1)
    ps::lib::sql::ind_t iUpdatableInd;
};

const char cAllMviews::szCreStmt[] = {
"CREATE TABLE ALL_MVIEWS\n"
"( OWNER                          TEXT NOT NULL\n"
", MVIEW_NAME                     TEXT NOT NULL\n"
", UPDATABLE                      TEXT \n"
", CONSTRAINT PK_ALL_MVIEWS PRIMARY KEY\n"
    "( OWNER\n"
    ", MVIEW_NAME\n"
    ")\n"
")"
};

const char cAllMviews::szDelStmt[] = {
"DELETE FROM ALL_MVIEWS"
};

const char cAllMviews::szInStmt_[] = {
"select OWNER "
", MVIEW_NAME "
", UPDATABLE "
"from ALL_MVIEWS "
"where OWNER in %s "
};

const char cAllMviews::szOutStmt_[] = {
"INSERT INTO ALL_MVIEWS "
"( OWNER "
", MVIEW_NAME "
", UPDATABLE "
") VALUES (?,?,?)"
};

const uint32_t cAllMviews::iBulkSize_ = 1000;

cAllMviews::cAllMviews(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_MVIEWS", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szMviewName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUpdatable, SQLT_STR, &rTable_->iUpdatableInd, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szMviewName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szUpdatable, tLite3Type::STR, &rTable_->iUpdatableInd, iSkip_, iSkip_);
}

cAllMviews::~cAllMviews()
{
    delete [] rTable_;
}

void cAllMviews::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllMviews::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


