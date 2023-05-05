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

#pragma once

#define  MINIMUM_CTRFLD_LENGTH               36  ///< Field name width (in control file of SQL*Loader)

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

class cAttrImpl
{
protected:
    const ps::lib::cConfigures& conf_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;

protected:
    ps::lib::sql::occi::cOciStmt& oOciStmt_;
    const uint32_t pos_;             // column of posision, represents 1 origin.
    oracle::occi::Type dType_;  // take from an implicit describe
    std::string   sName_;   // column name
    bool          iIsNull_; // nullability 
    sb4           iWidth_;  // max data width by string.
    std::string   sType_;   // type name of SQL*Loader
    int32_t dSize_;
    int32_t dPrecision_;
    int32_t dScale_;
    const uint32_t& iBulkSize_;
    void *data_;
    oracle::occi::Type type_;
    ub4 size_;  // indicates length of data in bytes.
    sb2 *ind_;
    ub2 *length_;
    ub2 *rc_;
    cAttrImpl(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : conf_(ps::lib::cConfigures::get_const_instance())  
        , trc_(ps::lib::cTracer::get_mutable_instance())
        , mos_(ps::lib::cDistributor::get_mutable_instance())
        , oOciStmt_(oOciStmt)
        , pos_(pos)
        , dType_(dType)
        , sName_(sName)
        , iIsNull_(meta.getBoolean(oracle::occi::MetaData::ATTR_IS_NULL))
        , dSize_(meta.getInt(oracle::occi::MetaData::ATTR_DATA_SIZE))
        , dPrecision_(meta.getInt(oracle::occi::MetaData::ATTR_PRECISION))
        , dScale_(meta.getInt(oracle::occi::MetaData::ATTR_SCALE))
        , iBulkSize_(iBulkSize)
        , data_(0)  
        , ind_(0)
        , length_(0)
        , rc_(0)
    {}
    ~cAttrImpl()
    {
        if (data_) delete [] (char *) data_;
        if (ind_) delete [] ind_;
        if (length_) delete [] length_;
        if (rc_) delete [] rc_;
    }
    void vAllocCommon()
    {
        ind_ = new sb2[iBulkSize_];
        length_ = new ub2[iBulkSize_];
        rc_ = new ub2[iBulkSize_];
        /*
         * Cleaning up length_[] is nessasory to prebent unconformity
         * datagram outputting when parallel degree is especialy (>=8) high.
         */
        ::memset(length_, 0, sizeof(ub2) * iBulkSize_);
    }
    void vAllocMemory() 
    {
        data_ = new char[size_ * iBulkSize_];
        vAllocCommon(); 
    }
    void vSetDataBuffer(ps::lib::sql::occi::cDefine& oDefine) 
    {
        BOOST_ASSERT(data_);
        BOOST_ASSERT(ind_);
        BOOST_ASSERT(length_);
        BOOST_ASSERT(rc_);
        oDefine.vAddItem(
            data_, size_, type_, (ps::lib::sql::ind_t *) ind_, length_, rc_
            , size_, sizeof(sb2), sizeof(ub2), sizeof(ub2)
            , OCI_DEFAULT, (OCICallbackDefine) 0, (void*) 0
            , 0, 0, 0, 0
        );
    }
    std::string sGetFieldName() const {return sName_; }
    std::string sGetFieldForCtrl(const ps::lib::cDelimiter& oDelim) const
    {
        return ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + sType_ + "(" + boost::lexical_cast<std::string>(iWidth_) + ") " 
            + oDelim.sGetClauseEncForCtrl()
        ;
    }
    int32_t iGetBufMemSize() const
    {
        return iBulkSize_ * ( size_ + sizeof(sb2) + sizeof(ub2) + sizeof(ub2) ); 
    }
    void vConvertStringVct(
        ps::lib::str_vct& oRowBuf
        , const ub4& iNumIter
        , const bool& iSep
        , const ps::lib::cDelimiter& oDelim
    ) const
    {
        std::string sBuf;
        for (ub4 iRow = 0; iRow < iNumIter; ++iRow)
        {
            oDelim.vEnCls(
                oRowBuf[iRow]
                , sBuf.assign(static_cast<char *>(data_) + (size_ * iRow), length_[iRow])
                , static_cast<ps::lib::sql::ind_t>(ind_[iRow])
                , iSep
            );
        }
        ::memset(length_, 0, sizeof(ub2) * iBulkSize_);
    }
    std::string sGetFieldType() const
    {
        return sType_;
    }
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

