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

#define NULLABLE_LEN           1+1
#define CHAR_USED_LEN          1+1
#define VIRTUAL_COLUMN_LEN     3+1
#define DATA_DEFAULT_LEN    1024+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cAllTabColumns::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(128)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(128)
    char szColumnName[COLUMN_NAME_LEN];       // NOT NULL VARCHAR2(128)
    char szDataType[OBJECT_NAME_LEN];         //          VARCHAR2(128)
    ps::lib::sql::ind_t nDataTypeInd;         // 
    int32_t iDataLength;                      // NOT NULL NUMBER
    ps::lib::sql::ind_t nDataLengthInd;       // 
    int32_t iDataPrecision;                   //          NUMBER
    ps::lib::sql::ind_t nDataPrecisionInd;    // 
    int32_t iDataScale;                       //          NUMBER
    ps::lib::sql::ind_t nDataScaleInd;        // 
    char szNullable[NULLABLE_LEN];            //          VARCHAR2(1)
    ps::lib::sql::ind_t nNullableInd;         // 
    int32_t iColumnId;                        //          NUMBER
    ps::lib::sql::ind_t nColumnIdInd;         // 
    int32_t iDefaultLength;                   //          NUMBER
    ps::lib::sql::ind_t nDefaultLengthInd;    // 
    char szDataDefault[DATA_DEFAULT_LEN];     //          LONG
    ps::lib::sql::ind_t nDataDefaultInd;      // 
    int32_t iCharColDeclLength;               //          NUMBER
    ps::lib::sql::ind_t nCharColDeclLengthInd;// 
    int32_t iCharLgength;                     //          NUMBER
    ps::lib::sql::ind_t nCharLgengthInd;      // 
    char szCharUsed[DATA_DEFAULT_LEN];        //          VARCHAR2(1)  
    ps::lib::sql::ind_t nCharUsedInd;         // 
    char szVirtualColumn[VIRTUAL_COLUMN_LEN]; //          VARCHAR2(3)
    ps::lib::sql::ind_t nVirtualColumnInd;    // 
};

const char cAllTabColumns::szCreStmt[] = {
"CREATE TABLE ALL_TAB_COLUMNS\n"
"( OWNER                          TEXT NOT NULL\n"
", TABLE_NAME                     TEXT NOT NULL\n"
", COLUMN_NAME                    TEXT NOT NULL\n"
", DATA_TYPE                      TEXT \n"
", DATA_LENGTH                    INT NOT NULL\n"
", DATA_PRECISION                 INT \n"
", DATA_SCALE                     INT \n"
", NULLABLE                       TEXT \n"
", COLUMN_ID                      INT \n"
", DEFAULT_LENGTH                 INT \n"
", DATA_DEFAULT                   TEXT \n"
", CHAR_COL_DECL_LENGTH           INT \n"
", CHAR_LENGTH                    INT \n"
", CHAR_USED                      TEXT \n"
", VIRTUAL_COLUMN                 TEXT \n"
", CONSTRAINT PK_ALL_TAB_COLUMNS PRIMARY KEY\n"
    "( OWNER\n"
    ", TABLE_NAME\n"
    ", COLUMN_NAME\n"
    ")\n"
")"
};

// ALL_TAB_COLS has appeared since Oracle 10g Release 1
// VIRTUAL_COLUMN VARCHAR2(3) has appeared since Oracle 11g Release 1

const char cAllTabColumns::szDelStmt[] = {
"DELETE FROM ALL_TAB_COLUMNS"
};

const char cAllTabColumns::szInStmt_[] = {
"select OWNER "
", TABLE_NAME "
", COLUMN_NAME "
", DATA_TYPE "
", DATA_LENGTH "
", DATA_PRECISION "
", DATA_SCALE "
", NULLABLE "
", COLUMN_ID "
", DEFAULT_LENGTH "
", DATA_DEFAULT "
", CHAR_COL_DECL_LENGTH "
", CHAR_LENGTH "
", CHAR_USED "
", '' as VIRTUAL_COLUMN "
"from ALL_TAB_COLUMNS "
"where owner in %s "
};

const char cAllTabColumns::szOutStmt_[] = {
"INSERT INTO ALL_TAB_COLUMNS "
"( OWNER "
", TABLE_NAME "
", COLUMN_NAME "
", DATA_TYPE "
", DATA_LENGTH "
", DATA_PRECISION "
", DATA_SCALE "
", NULLABLE "
", COLUMN_ID "
", DEFAULT_LENGTH "
", DATA_DEFAULT "
", CHAR_COL_DECL_LENGTH "
", CHAR_LENGTH "
", CHAR_USED "
", VIRTUAL_COLUMN "
") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
};

const uint32_t cAllTabColumns::iBulkSize_ = 100;


cAllTabColumns::cAllTabColumns(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ALL_TAB_COLUMNS", &oBind_, szOutStmt_)
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
    oDefine_.vAddItem(rTable_->szDataType, SQLT_STR, &rTable_->nDataTypeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDataLength, SQLT_INT, &rTable_->nDataLengthInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDataPrecision, SQLT_INT, &rTable_->nDataPrecisionInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDataScale, SQLT_INT, &rTable_->nDataScaleInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szNullable, SQLT_STR, &rTable_->nNullableInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_INT, &rTable_->nColumnIdInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefaultLength, SQLT_INT, &rTable_->nDefaultLengthInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDataDefault, SQLT_STR, &rTable_->nDataDefaultInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iCharColDeclLength, SQLT_INT, &rTable_->nCharColDeclLengthInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iCharLgength, SQLT_INT, &rTable_->nCharLgengthInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCharUsed, SQLT_STR, &rTable_->nCharUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szVirtualColumn, SQLT_STR, &rTable_->nVirtualColumnInd, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szColumnName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDataType, tLite3Type::STR, &rTable_->nDataTypeInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iDataLength, tLite3Type::INT32, &rTable_->nDataLengthInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iDataPrecision, tLite3Type::INT32, &rTable_->nDataPrecisionInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iDataScale, tLite3Type::INT32, &rTable_->nDataScaleInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szNullable, tLite3Type::STR, &rTable_->nNullableInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iColumnId, tLite3Type::INT32, &rTable_->nColumnIdInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iDefaultLength, tLite3Type::INT32, &rTable_->nDefaultLengthInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szDataDefault, tLite3Type::STR, &rTable_->nDataDefaultInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iCharColDeclLength, tLite3Type::INT32, &rTable_->nCharColDeclLengthInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iCharLgength, tLite3Type::INT32, &rTable_->nCharLgengthInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szCharUsed, tLite3Type::STR, &rTable_->nCharUsedInd, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szVirtualColumn, tLite3Type::STR, &rTable_->nVirtualColumnInd, iSkip_, iSkip_);
}

cAllTabColumns::~cAllTabColumns()
{
    delete [] rTable_;
}

void cAllTabColumns::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTabColumns::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


