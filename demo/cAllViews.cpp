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
#include "cAllViews.h"

namespace ps
{

namespace demo
{

struct cAllViews::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];      ///< NOT NULL VARCHAR2(30)
    char szViewName[OBJECT_NAME_LEN];   ///< NOT NULL VARCHAR2(30)
    int32_t fTextLength;                ///< NUMBER
    ps::lib::sql::ind_t iTextLengthInd; ///< ps::lib::sql::ind_t::VAL_IS_NULL: TEXT_LENGTH IS NULL
    char *szText;                       ///< LONG
    ps::lib::sql::ind_t iTextInd;       ///< ps::lib::sql::ind_t::VAL_IS_NULL: TEXT IS NULL
    uint32_t iTextLen;                  ///< Length of szText.
};

const char cAllViews::szStmt_[] = {
"select OWNER "
", VIEW_NAME "
", TEXT_LENGTH "
", TEXT "
"from ALL_VIEWS "
"order by OWNER, VIEW_NAME "
};

const uint32_t cAllViews::iBulkSize_ = 10;

cAllViews::cAllViews(const std::string& sOwner, ps::lib::sql::occi::cSvc& oSvc)
    : cUnloader(oSvc
        , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "ALL_VIEWS", "")
        , iBulkSize_, szStmt_, "ALL_VIEWS", ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_)
{
    // output data
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    oDefine_.vAddItem(SQLT_STR, iSkip_);
    oDefine_.vAddItem(SQLT_INT, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
    st_data_ = oStreamSup_->oOpen(ps::lib::nsStreamLocator::iExtData, sDataFileDir_);
}

cAllViews::~cAllViews()
{
    auto i = 0;
    for (auto rTable: bulk_)
    {
        for (auto iRow = 0lu; iRow < iters_[i]; ++iRow)
        {
            const tAttributes * rCur = bulk_[i] + iRow;
            if (rCur->szText) delete [] rCur->szText;
        }
        delete [] rTable;
        ++i;
    }
}

void cAllViews::vPreBulkAction(const uint32_t& iBulkSize_)
{
    auto * rTable = new tAttributes[iBulkSize_];
    ::memset(rTable, 0, iSkip_ * iBulkSize_);
    oDefine_.vChangeAddr( 1, rTable->szOwner, NULL, NULL, NULL);
    oDefine_.vChangeAddr( 2, rTable->szViewName, NULL, NULL, NULL);
    oDefine_.vChangeAddr( 3, rTable->fTextLength, &rTable->iTextLengthInd, NULL, NULL);
    pv_.vSetAddr(&rTable->szText, &rTable->iTextInd, &rTable->iTextLen);
    bulk_.push_back(rTable);
}

void cAllViews::vPreRepeatAction() 
{
    if (oDelim_.iDoesEmbedColumnNames())
    {
        vPutColumnNamesToDataFile();
    }
}


void cAllViews::vPostRepeatAction()
{
    ASSERT_OR_RAISE(st_data_ != nullptr, std::logic_error
        , boost::format("%s - It no longer points to any pointee.") % "ERROR");
    std::ostringstream oss;
    oss.fill('0');
    auto iNumBulks = iters_.size();
    for (auto i = 0lu; i < iNumBulks; ++i)
    {
        for (auto iRow = 0lu; iRow < iters_[i]; ++iRow)
        {
            const auto * rCur = bulk_[i] + iRow;
            std::string sRow;
            vAddDelims(sRow, oDelim_, rCur->szOwner, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
            vAddDelims(sRow, oDelim_, rCur->szViewName, ps::lib::sql::ind_t::VAL_IS_NOTNULL, true);
            vAddDelims(sRow, oDelim_, rCur->fTextLength, rCur->iTextLengthInd, true);
            oss << std::setw(NUM_DIGITS_VARCHARC) << rCur->iTextLen;
            sRow += oss.str();
            oss.str("");
            sRow.append(rCur->szText, rCur->szText + rCur->iTextLen);
            sRow += oDelim_.sGetLastSeparator(ps::lib::cDelimiter::iData);
            sRow += oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iData);
            const auto& sLength = oDelim_.sGetLengthString(sRow);
            vAddOutputBytes(sLength.size() + sRow.size());
            *st_data_ << sLength << sRow;
        }
    }
}

void cAllViews::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    iters_.push_back(iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::demo

} // ps

