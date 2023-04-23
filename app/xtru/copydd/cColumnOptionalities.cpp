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
#include "cColumnOptionalities.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

#define VIR_COL_LEN        3+1
struct cColumnOptionalities::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szColumnName[OBJECT_NAME_LEN];       // NOT NULL VARCHAR2(30)
    char szVirtualColumn[VIR_COL_LEN];        // 'NO' or 'YES'
};

const char cColumnOptionalities::szCreStmt[] = {
"CREATE TABLE COLUMN_OPTIONALITIES\n"
"( OWNER                          TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", COLUMN_NAME                    TEXT NOT NULL\n"
", VIRTUAL_COLUMN                 TEXT\n"
", CONSTRAINT PK_COLUMN_OPTIONALITIES PRIMARY KEY\n"
    "( OWNER\n"
    ", TABLE_NAME\n"
    ", COLUMN_NAME\n"
    ")\n"
")"
};

const char cColumnOptionalities::szDelStmt[] = {
"DELETE FROM COLUMN_OPTIONALITIES"
};

const char cColumnOptionalities::szInStmt_[] = {
"select owner"
", table_name "
", column_name "
", virtual_column "
"from  all_tab_cols "
"where owner in %s "
};

const char cColumnOptionalities::szOutStmt_[] = {
"INSERT INTO COLUMN_OPTIONALITIES "
"( OWNER "
", TABLE_NAME "
", COLUMN_NAME "
", VIRTUAL_COLUMN "
") VALUES (?,?,?,?)"
};

const uint32_t cColumnOptionalities::iBulkSize_ = 1000;

cColumnOptionalities::cColumnOptionalities(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "COLUMN_OPTIONALITIES", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szVirtualColumn, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szColumnName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szVirtualColumn, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cColumnOptionalities::~cColumnOptionalities()
{
    delete [] rTable_;
}

void cColumnOptionalities::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cColumnOptionalities::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


