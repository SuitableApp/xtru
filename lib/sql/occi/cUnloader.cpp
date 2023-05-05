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

namespace ps 
{

namespace lib
{

namespace sql
{

namespace occi
{
const int32_t cUnloader::NO_LONG_COLUMN = 0;

/**
 * @details
 */
std::string cUnloader::oGetColNamesList() const
{
    BOOST_ASSERT(oCont_.size());
    std::string buf;
    auto i = 0;
    const auto iNumCols = iGetNumCols();
    for (const auto& oAttr: oCont_[0].oStmt_->oGetAttrs())
    {
        vAddDelims(buf, oDelim_, oAttr.sGetFieldName(), ps::lib::sql::ind_t::VAL_IS_NOTNULL, i < iNumCols - 1);
        ++i;
    }
    return buf;
}
/**
 * @details
 */
std::string cUnloader::sGetFieldsListForCtrl() const
{
    BOOST_ASSERT(oCont_.size());
    std::ostringstream oss;
    auto i = 0;
    for (const auto& oAttr: oCont_[0].oStmt_->oGetAttrs())
    {
        oss << std::string(i ? ",": "(") + std::string(2, ' ')
            << oAttr.sGetFieldForCtrl(oDelim_) << std::endl;
        ++i;
    }
    if (i) oss << ")" << std::endl;
    return oss.str();
}
/**
 * @details
 */
void cUnloader::vAddOutputBytes(const int64_t& iOutputBytes)
{
    stat_.vAddOutputBytes(iOutputBytes); // for cumulating the process total.
    iTotalBytes_ += iOutputBytes; // for cumulating local instance total.
}
/**
 * @details
 */
void cUnloader::vAddOutputRows(const int32_t& iOutputRows)
{
    iTotalRows_ += iOutputRows;
}
/**
 * @details
 */
void cUnloader::vShowStatValue() const
{
    // counts the number of the element SQLs fetch has done.
    const size_t iNumFetchHasDone = std::count_if(
        oCont_.cbegin(), oCont_.cend()
        , [](const tValue& oItem){ return oItem.oStmt_->iGetFetchHasDone(); }
    );
    const auto iMiSec = stat_.iDurationMilliSeconds();
    const auto iOutputBytes = stat_.iGetOutputBytes();
    const auto iDataRate = iMiSec ? iOutputBytes * 1000 / iMiSec : 0;
    const auto sContinuety
        = rtn_.iCotinue()
        ? (iNumFetchHasDone == oCont_.size() ? "Done." : "...")
        : "CAN."
    ;
    // Outline of statistics for console.
    cout_ << boost::format(ps::lib::sql::sFmtStatCout)
        % tag_
        % ps::lib::sIntToa(iTotalRows_.load())
        % ps::lib::sBinIntToIntStr(iOutputBytes)
        % (iMiSec/1000.0)
        % ps::lib::sBinIntToIntStr(iDataRate)
        % sContinuety
    << std::endl;
    // Detail of statistics for tracefile.
    trc_ << boost::format(ps::lib::sql::sFmtStatTrace)
        % tag_
        % ps::lib::sIntToa(iTotalRows_.load())
        % ps::lib::sIntToa(iTotalBytes_.load())
        % ps::lib::sBinIntToIntStr(iOutputBytes)
        % ps::lib::sIntToa(iOutputBytes)
        % (iMiSec/1000.0)
        % ps::lib::sBinIntToIntStr(iDataRate)
        % sContinuety
    << std::endl;
}
/**
 * @details
 */
void cUnloader::vClearBuffer()
{
    BOOST_ASSERT(oCont_.size());
    for (auto& oRowBuf : oCont_[*oTls_].oRowBuf_)
    {
        /*
         * In order to avoid shrinking allocated storage,
         * clearing of the buffer by the following method 
         * is effective other than substitution empty a string.
         */
        oRowBuf.clear();
    }
}
/**
 * @details
 */
void cUnloader::vPutColumnNamesToDataFile()
{
    // column name outputting.
    auto buf = oGetColNamesList();
    buf += oDelim_.sGetLastSeparator(ps::lib::cDelimiter::iData);
    buf += oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iData);
    vAddOutputBytes(buf.size());
    // column name list is not a part of row data.
    vAddOutputRows(ps::lib::iOneItem);
    *st_data_ << buf;
    ASSERT_OR_RAISE(*st_data_, std::runtime_error, ::strerror(errno));
}
/**
 * @details
 */
void cUnloader::vSetRowBuf(
    const int32_t& iCol
    , const uint32_t& iNumIter
    , const bool& iSep
){
    BOOST_ASSERT(iNumIter);
    auto& oRowBuf = oCont_[*oTls_].oRowBuf_; // Converted data is filled up here.
    oCont_[*oTls_].oStmt_->oGetAttrs()[iCol].vConvertStringVct(oRowBuf, iNumIter, iSep, oDelim_);
}
/**
 * @details
 */
void cUnloader::vPutRowsToDataFile(const uint32_t& iNumIter)
{
    std::lock_guard<spinlock_t> lk(spin_);
    auto& oRowBuf = oCont_[*oTls_].oRowBuf_;
    for (auto iRow = 0u; iRow < iNumIter; ++iRow)
    {
        oRowBuf[iRow] += oDelim_.sGetLastSeparator(ps::lib::cDelimiter::iData);
        oRowBuf[iRow] += oDelim_.sGetRowSeparator(ps::lib::cDelimiter::iData);
        const auto& sLength = oDelim_.sGetLengthString(oRowBuf[iRow]);
        vAddOutputBytes(sLength.size() + oRowBuf[iRow].size());
        *st_data_ << sLength << oRowBuf[iRow];
    }
    ASSERT_OR_RAISE(*st_data_, std::runtime_error, ::strerror(errno));
}
/**
 * @details
 */
void cUnloader::vPutGrammerToCtrlFile()
{
    BOOST_ASSERT(sLastOpendFilenme_.has_filename());
    ps::lib::sql::cCtrlFile oCtrlFile(
        sLastOpendFilenme_.filename(), tag_, sPartitionName_, iNumLongs_
    );
    st_ctrl_ = oStreamSup_->oOpen(ps::lib::nsStreamLocator::iExtCtrl, sDataFileDir_);
    BOOST_SCOPE_EXIT(&st_ctrl_,&oStreamSup_)
    {
        st_ctrl_->flush();
        delete st_ctrl_.release();
    }
    BOOST_SCOPE_EXIT_END;
    *st_ctrl_ << oCtrlFile.sGetGrammar() << sGetFieldsListForCtrl()
      << std::flush;
    ASSERT_OR_RAISE(*st_ctrl_, std::runtime_error, ::strerror(errno));
}

/**
 * @details
 */
cUnloader::cUnloader(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::nsStreamLocator::cStreamSupplier* oStreamSup
    , const uint32_t& iBulkSize
    , const std::string& sql
    , const std::string& tag
    , const int32_t& iNumLongs
    , ps::lib::sql::occi::cBind* oBind
)
    : stat_(ps::lib::cStat::get_mutable_instance())
    , rtn_(ps::lib::cRtn::get_const_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , spin_(std::chrono::microseconds(300)) // microseconds to wait for each spin.
    , iTls_(0)
    , oTls_(vRegularDeleter<int32_t>)
    , iTotalBytes_(0)
    , iTotalRows_(0U)
    , tag_(tag)
    , iNumLongs_(iNumLongs)
    , oDelim_(ps::lib::oMakeVarDelimiter())
    , iBulkSize_(iBulkSize)
    , oStreamSup_(oStreamSup)
{
    // Multiple statement is sparated by a semi-colon.
    ps::lib::tSep sep("\\", ";", "");
    boost::tokenizer< ps::lib::tSep > tokens(sql, sep);
    ASSERT_OR_RAISE(tokens.begin() != tokens.end(), std::runtime_error
        , boost::format(
          "At least one statement must be included in 'sql'. Actually zero."
        )
    );
    for (const auto& sql: tokens)
    {
        const auto trimed = boost::trim_copy(sql);
        if (ps::lib::trim(trimed).size() == 0U) return;
        oCont_.emplace_back(
            new ps::lib::sql::occi::cStmt(oSvc, iBulkSize_, trimed, tag_, oBind)
            , iBulkSize_
        );
    }
}
/**
 * @details
 */
cUnloader::cUnloader(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::nsStreamLocator::cStreamSupplier* oStreamSup
    , const uint32_t& iBulkSize
    , const std::string& sql
    , const std::string& tag
    , const int32_t& iNumLongs
)
    : cUnloader(oSvc, oStreamSup, iBulkSize, sql, tag, iNumLongs, nullptr)
{}
/**
 * @details
 */
cUnloader::~cUnloader()
{}
/**
 * @details
 */
void cUnloader::vExecuteAndFetch()
{
    auto iTotal = 0lu;
    std::exception_ptr ep = nullptr;
    st_data_ = oStreamSup_->oOpen(ps::lib::nsStreamLocator::iExtData, sDataFileDir_);
    sLastOpendFilenme_ = oStreamSup_->oGetsLastOpendFilename();
    sPartitionName_ = oStreamSup_->sGetPartitionName();
    {
        BOOST_SCOPE_EXIT(&st_data_)
        {
            // flush() operation can not be omitted.
            // Because the end of the data is lost.
            st_data_->flush();
            delete st_data_.release();
        }
        BOOST_SCOPE_EXIT_END;
        for (auto& oItem: oCont_)
        {
            oItem.oStmt_->vExecute();
            if (&oCont_[0] == &oItem)
            {
                /*
                 * When oItem points first element of oCont_,
                 * this will be done.
                 */
                vPreRepeatAction();
            }
            else
            {
                /*
                 * If this condition is true, it is necessary
                 * to check for whether data type of the fields of
                 * the first statement are compatible with type of
                 * the fields of the other statement.
                 */
                ps::lib::sql::occi::vCheckCompatibility(
                    oCont_[0].oStmt_->oGetAttrs()
                    , oItem.oStmt_->oGetAttrs()
                );
            }
            if (!rtn_.iCotinue()) break;
            std::packaged_task<uint32_t()> task(std::bind(
                &ps::lib::sql::occi::cStmt::iFetch
                , oItem.oStmt_.get()
                , std::ref(*this), std::ref(ep)
            ));
            oItem.oFuture_ = task.get_future();
            oItem.oThr_.reset(new std::thread(std::move(task)));
            oItem.iTid_ = oItem.oThr_->get_id();
        }
        for (auto& oItem: oCont_)
        {
            if (oItem.oThr_ && oItem.oThr_->joinable())
            {
                oItem.oThr_->join();
                iTotal += (oItem.iNumRows_ = oItem.oFuture_.get());
            }
        }
        if (iTotal)
        {
            vPostRepeatAction();
        }
        else
        {
            vNotFoundAction();
        }
    } /// The data file is closed here.
    vFinalizeAction();
    if (! ps::lib::nsStreamLocator::iSuppressCtrlf_)
    {
        vPutGrammerToCtrlFile();
    }
    {
        for (const auto& oItem: oCont_)
        {
            std::ostringstream oss;
            oss << std::hex << oItem.iTid_;
            trc_ << boost::format("Number of rows=%16s [%s]")
                % ps::lib::sIntToa(oItem.iNumRows_) % oss.str() << std::endl;
        }
        if (oDelim_.iDoesEmbedColumnNames())
        {
            trc_ << boost::format("     Header(s)=%16s [%s]")
                % ps::lib::sIntToa(iTotalRows_ - iTotal) % tag_ << std::endl;
        }
        trc_ << boost::format("    Data total=%16s [%s]")
            % ps::lib::sIntToa(iTotal) % tag_ << std::endl;
    }
    if (ep)
    {
        std::rethrow_exception(ep);
    }
}
/**
 * @details
 */
void cUnloader::vPreRepeatAction() 
{
    if (oDelim_.iDoesEmbedColumnNames())
    {
        vPutColumnNamesToDataFile();
    }
}
/**
 * @details
 */
void cUnloader::vPreBulkAction(const uint32_t& iBulkSize)
{
    std::lock_guard<spinlock_t> lk(spin_);
    {
        if (!oTls_.get())
        {
            // TLS has not been allocated.
            oTls_.reset(new int32_t(iTls_++));
        }
    }
    vClearBuffer();
}
/**
 * @details
 */
void cUnloader::vPostBulkAction(const uint32_t& iNumIter) 
{
    BOOST_ASSERT(iBulkSize_ >= iNumIter);
    const auto iNumCols = iGetNumCols();
    for (auto i = 0; i < iNumCols; ++i)
    {
        vSetRowBuf(i, iNumIter, i < iNumCols - 1);
    }
    vPutRowsToDataFile(iNumIter);
    vAddOutputRows(iNumIter);
}
/**
 * @details
 */
void cUnloader::vConvPlaceHolder(const ps::lib::str_vct& opts)
{
    if (! opts.empty())
    {
        for (auto& oItem: oCont_)
        {
            oItem.oStmt_->vConvPlaceHolder(opts);
        }
    }
}
/**
 * @details
 */
void cUnloader::vPostRepeatAction()
{}
/**
 * @details
 */
void cUnloader::vNotFoundAction()
{}
/**
 * @details
 */
void cUnloader::vFeedbackAction()
{
    vShowStatValue(); // Progress report.
}
/**
 * @details
 */
void cUnloader::vFinalizeAction()
{
    vShowStatValue(); // Final report.
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

