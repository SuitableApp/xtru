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

#pragma once

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

namespace nsLob
{

class tChr
{
protected:
    std::string sGetLdrField(const int32_t& iLength) const
    {
        return "VARCHARC(" + boost::lexical_cast<std::string>(NUM_DIGITS_VARCHARC)
                    + ", " + boost::lexical_cast<std::string>(iLength) + ")";
    }
    std::string sRowHeader(
        std::ostringstream& oss
        , const int32_t& iDigit
        , const std::string::size_type& size
    ) const
    {
        oss.fill('0');
        oss << std::setw(iDigit) << size;
        return oss.str();
    }
};

class tRaw
{
protected:
    std::string sGetLdrField(const int32_t& iLength) const
    {
        return "LONG VARRAW(" + boost::lexical_cast<std::string>(iLength) + ")";
    }
    std::string sRowHeader(
        std::ostringstream& oss
        , const int32_t& iDigit
        , const std::string::size_type& size
    ) const
    {
        char raw[4];
        raw[0] = static_cast<char>( size        & ~0x100);
        raw[1] = static_cast<char>((size >>  8) & ~0x100);
        raw[2] = static_cast<char>((size >> 16) & ~0x100);
        raw[3] = static_cast<char>((size >> 24) & ~0x100);
        oss << std::string(raw, 4);
        return oss.str();
    }
};

} // ps::lib::sql::occi::nsLob  

namespace nsReprVar  /* variable length representation */
{

template <
    class T
    , oracle::occi::Type occiT
>
class cLob
    : public cAttr
    , private cAttrImpl
    , private T
{
private:
    int32_t iPieceSize_;
    const size_t iSkip_;
    mutable ps::lib::sql::occi::cPieceVct pv_;
    struct tAttributes
    {
        char *szText;                             // LONG
        ps::lib::sql::ind_t iTextInd;                  // Indicator of szText.
        uint32_t iTextLen;                    // Length of szText.
    };
    tAttributes * rTable_;
    void vAllocMemory() 
    {
        rTable_ = new tAttributes[iBulkSize_];
        ::memset(rTable_, 0, iSkip_ * iBulkSize_);
        pv_.vSetAddr(&rTable_->szText, &rTable_->iTextInd, &rTable_->iTextLen);
    }
public:
    cLob(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , iPieceSize_(conf_.as<int32_t>("maxlongsize"))  
        , iSkip_(sizeof(tAttributes))
        , pv_(iSkip_)
        , rTable_(0)
    {
        type_ = occiT;
        size_ = iPieceSize_;
    }
    virtual ~cLob()
    {
        if (rTable_)
        {
            for (uint32_t iRow = 0; iRow < iBulkSize_; ++iRow)
            {
                const tAttributes * rCur = &rTable_[iRow];
                if (rCur->szText) delete [] rCur->szText;
            }
            delete [] rTable_;
        }
#ifndef NDEBUG
        trc_ << boost::format("%s; %s") % __PRETTY_FUNCTION__ % sName_ << std::endl;
#endif
    }
    virtual void vSetDataBuffer(ps::lib::sql::occi::cDefine& oDefine) 
    {
        vAllocMemory();
        oDefine.vAddItem(
            NULL, SB4MAXVAL, type_, (ps::lib::sql::ind_t *) NULL, NULL, NULL
            , 0, 0, 0, 0
            , OCI_DYNAMIC_FETCH, ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
            , 0, 0, 0, 0
        );
    }
    virtual std::string sGetFieldName() const {return cAttrImpl::sGetFieldName(); }
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter& oDelim) const
    {
        return ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + T::sGetLdrField(pv_.iMaxValSize());
        ;
    }
    virtual int32_t iGetBufMemSize() const
    {
        return iBulkSize_ * (iPieceSize_ + iSkip_); 
    }
    virtual void vConvertStringVct(
        ps::lib::str_vct& oRowBuf
        , const ub4& iNumIter
        , const bool& iSep
        , const ps::lib::cDelimiter& oDelim
    ) const
    {
        ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
        std::ostringstream oss;
        int32_t iDigit = oDelim.iGetVarDigit();
        for (ub4 iRow = 0; iRow < iNumIter; ++iRow)
        {
            oRowBuf[iRow] += T::sRowHeader(oss, iDigit, rTable_[iRow].iTextLen);
            oDelim.vUnCls(
                oRowBuf[iRow]
                , rTable_[iRow].szText
                , rTable_[iRow].iTextLen
                , rTable_[iRow].iTextInd
                , false
            );
            oss.str("");
        }
    }
    virtual std::string sGetFieldType() const { return cAttrImpl::sGetFieldType(); }
};

} // ps::lib::sql::occi::nsReprVar

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

