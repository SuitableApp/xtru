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
#include "cAllTables.h"

namespace ps
{

namespace demo
{

struct cAllTables::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];        // NOT NULL VARCHAR2(30)
    char szTablespaceName[OBJECT_NAME_LEN];   // VARCHAR2(30)
    ps::lib::sql::ind_t iTablespaceNameInd;
    float fPctFree;                           // NUMBER
    ps::lib::sql::ind_t iPctFreeInd;
    float fPctUsed;                           // NUMBER
    ps::lib::sql::ind_t iPctUsedInd;
    double fInitialExtent;                    // NUMBER
    ps::lib::sql::ind_t iInitialExtentInd;
    double fNextExtent;                       // NUMBER
    ps::lib::sql::ind_t iNextExtentInd;
    double fPctIncrease;                      // NUMBER
    ps::lib::sql::ind_t iPctIncreaseInd;
    char szLastAnalyzed[DATETIME_STR_LEN];    // DATE
    ps::lib::sql::ind_t iLastAnalyzedInd;
    int32_t iNumRows;
    ps::lib::sql::ind_t iNumRowsInd;
    int32_t iBlocks;
    ps::lib::sql::ind_t iBlocksInd;
    uint32_t iAvgRowLen;
    ps::lib::sql::ind_t iAvgRowLenInd;
};

const char cAllTables::szStmt_[] = {
"select OWNER "
", TABLE_NAME "
", TABLESPACE_NAME "
", PCT_FREE "
", PCT_USED "
", INITIAL_EXTENT "
", NEXT_EXTENT "
", PCT_INCREASE "
", LAST_ANALYZED "
", NUM_ROWS "
", BLOCKS "
", AVG_ROW_LEN "
"from all_tables "
"where DROPPED = :DROPPED "
};

const uint32_t cAllTables::iBulkSize_ = 10;

cAllTables::cAllTables(const std::string& sOwner, ps::lib::sql::occi::cSvc& oSvc)
    : cUnloader(oSvc
        , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "ALL_TABLES", "")
        , iBulkSize_, szStmt_, "ALL_TABLES", ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(nullptr)
    , search_value_("NO")
{
    rTable_ = new tAttributes[iBulkSize_];
    // input data
    oBind_.vAddItem(":DROPPED", search_value_);
    // output data
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->iTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->fPctFree, SQLT_BFLOAT, &rTable_->iPctFreeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->fPctUsed, SQLT_BFLOAT, &rTable_->iPctUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->fInitialExtent, SQLT_BDOUBLE, &rTable_->iInitialExtentInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->fNextExtent, SQLT_BDOUBLE, &rTable_->iNextExtentInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->fPctIncrease, SQLT_BDOUBLE, &rTable_->iPctIncreaseInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLastAnalyzed, SQLT_STR, &rTable_->iLastAnalyzedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iNumRows, SQLT_INT, &rTable_->iNumRowsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iBlocks, SQLT_INT, &rTable_->iBlocksInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iAvgRowLen, SQLT_UIN, &rTable_->iAvgRowLenInd, NULL, NULL, iSkip_);
    st_data_ = oStreamSup_->oOpen(ps::lib::nsStreamLocator::iExtData, sDataFileDir_);
}

cAllTables::~cAllTables()
{
    delete [] rTable_;
}

void cAllTables::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cAllTables::vPreRepeatAction() 
{
    if (oDelim_.iDoesEmbedColumnNames())
    {
        vPutColumnNamesToDataFile();
    }
}

void cAllTables::vPostBulkAction(const uint32_t& iNumIter)
{
    for (auto iRow = 0lu; iRow < iNumIter; ++iRow)
    {
        const auto * rCur = &rTable_[iRow];
        std::string sRow;
        vAddDelims(sRow, oDelim_, rCur->szOwner, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
        vAddDelims(sRow, oDelim_, rCur->szTableName, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
        vAddDelims(sRow, oDelim_, rCur->szTablespaceName, rCur->iTablespaceNameInd, true);
        vAddDelims(sRow, oDelim_, rCur->fPctFree, rCur->iPctFreeInd, true);
        vAddDelims(sRow, oDelim_, rCur->fPctUsed, rCur->iPctUsedInd, true);
        vAddDelims(sRow, oDelim_, rCur->fInitialExtent, rCur->iInitialExtentInd, true);
        vAddDelims(sRow, oDelim_, rCur->fNextExtent, rCur->iNextExtentInd, true);
        vAddDelims(sRow, oDelim_, rCur->fPctIncrease, rCur->iPctIncreaseInd, true);
        vAddDelims(sRow, oDelim_, rCur->szLastAnalyzed, rCur->iLastAnalyzedInd, true);
        vAddDelims(sRow, oDelim_, rCur->iNumRows, rCur->iNumRowsInd, true);
        vAddDelims(sRow, oDelim_, rCur->iBlocks, rCur->iBlocksInd, true);
        vAddDelims(sRow, oDelim_, rCur->iAvgRowLen, rCur->iAvgRowLenInd, false);
        sRow += oDelim_.sGetLastSeparator(ps::lib::cDelimiter::iData);
        sRow += oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iData);
        const auto& sLength = oDelim_.sGetLengthString(sRow);
        vAddOutputBytes(sLength.size() + sRow.size());
        *st_data_ << sLength << sRow;
    }
    vAddOutputRows(iNumIter);
}

} // ps::demo

} // ps

