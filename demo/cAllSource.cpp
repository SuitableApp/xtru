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
#include "cAllSource.h"

#define COLUMNS_PAR_LINE 4000 + 1

namespace ps
{

namespace demo
{

struct cAllSource::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];            // NOT NULL VARCHAR2(30)
    char szName[OBJECT_NAME_LEN];             // NOT NULL VARCHAR2(30)
    char szType[OBJECT_TYPE_LEN];             // VARCHAR2(12)
    int32_t fLine;                            // NUMBER
    ps::lib::sql::ind_t iLineInd;
    char szText[COLUMNS_PAR_LINE];            // VARCHAR2(4000)
    ps::lib::sql::ind_t iTextInd;
};

const char cAllSource::szStmt_[] = {
"select OWNER "
", NAME "
", TYPE "
", LINE "
", TEXT "
"from ALL_SOURCE "
"order by OWNER, NAME, LINE "
};

const uint32_t cAllSource::iBulkSize_ = 200;

cAllSource::cAllSource(const std::string& sOwner, ps::lib::sql::occi::cSvc& oSvc)
    : cUnloader(oSvc
        , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "ALL_SOURCE", "")
        , iBulkSize_, szStmt_, "ALL_SOURCE", ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    // output data
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    oDefine_.vAddItem(SQLT_INT, iSkip_);
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    st_data_ = oStreamSup_->oOpen(ps::lib::nsStreamLocator::iExtData, sDataFileDir_);
}

cAllSource::~cAllSource()
{
    for (auto rTable: bulk_)
    {
        delete [] rTable;
    }
}

void cAllSource::vPreBulkAction(const uint32_t& iBulkSize_)
{
    auto * rTable = new tAttributes[iBulkSize_];
    ::memset(rTable, 0, iSkip_ * iBulkSize_);
    oDefine_.vChangeAddr( 1, rTable->szOwner, NULL, NULL, NULL);
    oDefine_.vChangeAddr( 2, rTable->szName, NULL, NULL, NULL);
    oDefine_.vChangeAddr( 3, rTable->szType, NULL, NULL, NULL);
    oDefine_.vChangeAddr( 4, rTable->fLine, &rTable->iLineInd, NULL, NULL);
    oDefine_.vChangeAddr( 5, rTable->szText, &rTable->iTextInd, NULL, NULL);
    bulk_.push_back(rTable);
}

void cAllSource::vPreRepeatAction() 
{
    if (oDelim_.iDoesEmbedColumnNames())
    {
        vPutColumnNamesToDataFile();
    }
}

void cAllSource::vPostRepeatAction()
{
    ASSERT_OR_RAISE(st_data_ != nullptr, std::logic_error
        , boost::format("%s - It no longer points to any pointee.") % "ERROR");
    auto iNumBulks = iters_.size();
    for (auto i = 0lu; i < iNumBulks; ++i)
    {
        for (auto iRow = 0lu; iRow < iters_[i]; ++iRow)
        {
            const auto * rCur = bulk_[i] + iRow;
            std::string sRow;
            vAddDelims(sRow, oDelim_, rCur->szOwner, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
            vAddDelims(sRow, oDelim_, rCur->szName, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
            vAddDelims(sRow, oDelim_, rCur->szType, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
            vAddDelims(sRow, oDelim_, rCur->fLine, rCur->iLineInd, true);
            vAddDelims(sRow, oDelim_, rCur->szText, rCur->iTextInd, true);
            sRow += oDelim_.sGetLastSeparator(ps::lib::cDelimiter::iData);
            sRow += oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iData);
            const auto& sLength = oDelim_.sGetLengthString(sRow);
            vAddOutputBytes(sLength.size() + sRow.size());
            *st_data_ << sLength << sRow;
        }
    }
}

void cAllSource::vPostBulkAction(const uint32_t& iNumIter)
{
    iters_.push_back(iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::demo

} // ps

