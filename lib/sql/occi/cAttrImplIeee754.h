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

namespace nsReprVar  /* variable length representation */
{

template <
    typename hostT
    , oracle::occi::Type occiT
    , int32_t iPrecision   // Precision of the mantissa part in decimal
    , int32_t iPrtSize     // Maximum display width of literal representation in decimal
>
class cIeee754
    : public cAttr
    , private cAttrImpl
{
private:
    std::string sMask_;
    char *szBuffer_;
public:
    cIeee754(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , sMask_("%0.*G"), szBuffer_(0)
    {
        size_ = sizeof(hostT);
        iWidth_ = iPrtSize;
        sType_ = "DECIMAL EXTERNAL";
        type_ = occiT;
        szBuffer_ = new char[iPrtSize + 1];
    }
    virtual ~cIeee754()
    {
        if (szBuffer_) delete [] szBuffer_;
#ifndef NDEBUG
        trc_ << boost::format("%s; %s") % __PRETTY_FUNCTION__ % sName_ << std::endl;
#endif
    }
    virtual void vSetDataBuffer(ps::lib::sql::occi::cDefine& oDefine) 
    {
        vAllocMemory();
        cAttrImpl::vSetDataBuffer(oDefine);
    }
    virtual std::string sGetFieldName() const {return cAttrImpl::sGetFieldName(); }
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter&) const
    {
        return ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + sType_ + "(" + boost::lexical_cast<std::string>(iWidth_) + ")"
        ;
    }
    virtual int32_t iGetBufMemSize() const { return cAttrImpl::iGetBufMemSize(); }
    virtual void vConvertStringVct(
        ps::lib::str_vct& oRowBuf
        , const ub4& iNumIter
        , const bool& iSep
        , const ps::lib::cDelimiter& oDelim
    ) const
    {
        std::string sBuf;
        for (ub4 iRow = 0; iRow < iNumIter; ++iRow)
        {
            ub4 iBuffer;
            ps::lib::sql::ind_t ind = static_cast<ps::lib::sql::ind_t>(ind_[iRow]);
            if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
            {
                iBuffer = ::snprintf(
                    szBuffer_, iPrtSize + 1, sMask_.c_str()
                    , iPrecision, static_cast<hostT*>(data_)[iRow]
                );
            }
            else
            {
                szBuffer_[0] = '\0';
                iBuffer = 0;
            }
            oDelim.vUnCls(
                oRowBuf[iRow]
                , sBuf.assign(szBuffer_, iBuffer)
                , ind
                , iSep
            );
        }
    }
    virtual std::string sGetFieldType() const { return cAttrImpl::sGetFieldType(); }
};

} // ps::lib::sql::occi::nsReprVar

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps


