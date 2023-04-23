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

#define ALIAS_NAME_LENGTH    30
#define FILE_NAME_LENGTH   2048

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

class cBfile
    : public cAttr
    , private cAttrImpl
{
private:
    char *szAlias_;
    char *szFName_;
    mutable ps::lib::sql::occi::cOciErr oOciErr_;
    void vAllocMemory() 
    {
        data_ = new char[size_ * iBulkSize_];
        vAllocCommon(); 
        for (uint32_t i = 0; i < iBulkSize_; ++i)
        {
            ps::lib::sql::occi::vDescriptorAlloc(
                oOciErr_, (dvoid **) &((OCILobLocator **) data_)[i], OCI_DTYPE_FILE
            );
        }
    }
public:
    cBfile(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , szAlias_(0), szFName_(0)
    {
        size_ = sizeof(OCILobLocator *);
        iWidth_ = size_; // iWidth_ will never used.
        sType_ = "BFILE";
        type_ = dType;
        szAlias_ = new char[ALIAS_NAME_LENGTH + 1];
        szFName_ = new char[FILE_NAME_LENGTH + 1];
    }
    virtual ~cBfile()
    {
        if (szFName_) delete [] szFName_;
        if (szAlias_) delete [] szAlias_;
        for (uint32_t i = 0; i < iBulkSize_; ++i)
        {
            ps::lib::sql::occi::vDescriptorFree(
                oOciErr_, ((OCILobLocator **) data_)[i],  OCI_DTYPE_FILE
            );
        }
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
        return " " + sName_ + "_DRN" + " FILLER CHAR(" + boost::lexical_cast<std::string>(ALIAS_NAME_LENGTH) + ") " 
            + oDelim.sGetClauseEncForCtrl() + "\n"
            + ", " + sName_ + "_FLN" + " FILLER CHAR(" + boost::lexical_cast<std::string>(FILE_NAME_LENGTH) + ") " 
            + oDelim.sGetClauseEncForCtrl() + "\n"
            + ", " + ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + sType_ + "(" + sName_ + "_DRN" + ", " +  sName_ + "_FLN" + ")"
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
            boolean bFlag = true;  // true: OS file exists.
            ub2 nALength = ALIAS_NAME_LENGTH;
            ub2 nFLength = FILE_NAME_LENGTH;
            ps::lib::sql::ind_t ind = static_cast<ps::lib::sql::ind_t>(ind_[iRow]);
            if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
            {
                ps::lib::sql::occi::vLobFileGetName(
                    oOciStmt_, ((OCILobLocator **)data_)[iRow]
                    , szAlias_, &nALength, szFName_, &nFLength, &bFlag
                );
            }
            // An alias name outputting.
            oDelim.vEnCls(
                oRowBuf[iRow]
                , sBuf.assign(szAlias_, nALength)
                , ind
                , true
            );
            // A file name outputting.
            oDelim.vEnCls(
                oRowBuf[iRow]
                , sBuf.assign(szFName_, nFLength)
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

