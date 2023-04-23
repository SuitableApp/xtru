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

class cTimestamp
    : public cAttr
    , private cAttrImpl
{
private:
    std::string sMask_;
public:
    cTimestamp(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
        , const std::string sMask
        , const char* szDataType
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , sMask_(sMask)
    {
        size_ = sMask_.size() + dScale_;
        iWidth_ = size_;
        sType_ = szDataType;
        type_ = oracle::occi::OCCI_SQLT_CHR;
    }
    virtual ~cTimestamp()
    {
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
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter& oDelim) const
    {
        return ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + sType_
            + " \"" + sMask_ + "\" "
            + oDelim.sGetClauseEncForCtrl()
        ;
    }
    virtual int32_t iGetBufMemSize() const { return cAttrImpl::iGetBufMemSize(); }
    virtual void vConvertStringVct(
        ps::lib::str_vct& oRowBuf
        , const ub4& iNumIter
        , const bool& iSep
        , const ps::lib::cDelimiter& oDelim
    ) const { cAttrImpl::vConvertStringVct(oRowBuf, iNumIter, iSep, oDelim); }
    virtual std::string sGetFieldType() const { return cAttrImpl::sGetFieldType(); }
};

} // ps::lib::sql::occi::nsReprVar

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
