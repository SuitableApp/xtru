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

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

class cNumberImpl
{
private:
    const ps::lib::cConfigures& conf_;
    std::string sFpNumFmt_;
    int32_t& dPrecision_;
    int32_t& dScale_;
protected:
    enum
    { NUM_IS_FP = -127
    , NUM_SIGN_DIGITS = 1
    , NUM_POINT_DIGITS = 1
    , NUM_EXPO_DIGITS = 5
        /* Maximum number of digits in the exponent part
          of the floating-point representation at the time. */
    , NUM_MAXIMAL_SCALE = 10
        /* When representing a number of extremely small or large scale
          in excess of this value, the format model is switched
          from fixed-point to floating-point.*/
    , MAX_PRECISION = 39
        /* Maximum precision of type Number */
    };
    cNumberImpl(int32_t& dPrecision, int32_t& dScale)
        : conf_(ps::lib::cConfigures::get_const_instance())
        , sFpNumFmt_(conf_.as<std::string>("fpnumfmt"))
        , dPrecision_(dPrecision)
        , dScale_(dScale)
    {
        if (0 == dPrecision_ && 0 == dScale_)
        {
            dScale_ = NUM_IS_FP; // this is a patching for event 10499
        }
    }
    sb4 iGetPrtSize() const
    {
        sb4 iRet = 0;
        iRet = NUM_SIGN_DIGITS; /* Sign takes the width of one digit. */
        if (/* PTN_1 */ dScale_ < dPrecision_ && dScale_ >= 0) {
            /* Point is displayed on the significant digits within.
               e.g.) number(7,2), number(7,0) -> represents 12345.67, 1234567 */
            iRet += dPrecision_;
            iRet += (dScale_ > 0 ? NUM_POINT_DIGITS: 0);
        } else if (/* PTN_2 */ dPrecision_ <= dScale_) {
            /* Point is displayed on the left side of significant digits,
               character 0 of one or more are embedded between the significant digits. */
            if ((dScale_ - dPrecision_) > NUM_MAXIMAL_SCALE){
                /* e.g.) number(7,18) -> represents 1.234567E-012
                                         instead of 0.000000000001234567 */
                iRet += dPrecision_ + NUM_EXPO_DIGITS;
            } else {
                /* e.g.) number(7,13) -> 0.0000001234567 */
                iRet += dScale_ + 1;
            }
            iRet += NUM_POINT_DIGITS;
        } else if (/* PTN_3 */ dScale_ == NUM_IS_FP) {
            if (0 == dPrecision_){
                if (sFpNumFmt_.size()){
                    iRet += sFpNumFmt_.size();
                } else {
                    /* Number type of argument without
                     * e.g.) number */
                    iRet += (MAX_PRECISION + NUM_EXPO_DIGITS);
                    iRet += NUM_POINT_DIGITS;
                }
            } else {
                /* e.g.) float, float(50) */
                iRet += (sb4) ceil((double) dPrecision_ * log10(2.0)) + NUM_EXPO_DIGITS;
                iRet += NUM_POINT_DIGITS;
            }
        } else /* PTN_4 */ {
            /* Point is displayed on the right side of significant digits,
               character 0 of one or more are embedded between the significant digits.
               It is always positive result of subtracting iScale from iPrecision.
               iScale so negative.*/
            if ((0 - dScale_) > NUM_MAXIMAL_SCALE){
                /* e.g.) number(7,-11) -> represents 1.234567E+018
                                          instead of 123456700000000000 */
                iRet += (dPrecision_ + NUM_EXPO_DIGITS);
                iRet += NUM_POINT_DIGITS;
            } else {
                /* e.g.) number(7,-5) -> 123456700000 */
                iRet += (dPrecision_ - dScale_);
            }
        }
        return iRet;
    }
    std::string sMakeNumFmt() const
    {
        int32_t i;
        std::string sRet = "fm";
        if (/* PTN_1 */ dScale_ < dPrecision_ && dScale_ >= 0){
            for (i = 0; i < dPrecision_; i++){
                if (i == (dPrecision_ - dScale_)){
                    sRet += '.';
                }
                sRet += '9';
            }
        } else if (/* PTN_2 */ dPrecision_ <= dScale_){
            if ((dScale_ - dPrecision_) > NUM_MAXIMAL_SCALE){
                for (i = 0; i < dPrecision_; i++){
                    if (1 == i){
                        sRet += '.';
                    }
                    sRet += '9';
                }
                sRet += "EEEE";
            } else {
                for (i = 0; i < dScale_ + 1; i++){
                    if (1 == i){
                        sRet += '.';
                    }
                    sRet += '9';
                }
            }
        } else if (/* PTN_3 */ dScale_ == NUM_IS_FP){
            if (0 == dPrecision_){
                if (sFpNumFmt_.size()){
                    sRet += sFpNumFmt_;
                } else {
                    // '9' of 39 digits
                    sRet += "9.99999999999999999999999999999999999999";
                    sRet += "EEEE";
                }
            } else {
                sb2 iDPrecision = (sb2) ceil((double) dPrecision_ * log10 (2.0));
                for (i = 0; i < iDPrecision; i++){
                    if (1 == i){
                        sRet += '.';
                    }
                    sRet += '9';
                }
                sRet += "EEEE";
            }
        } else /* PTN_4 */ {
            if ((0 - dScale_) > NUM_MAXIMAL_SCALE){
                for (i = 0; i < dPrecision_; i++){
                    if (1 == i){
                        sRet += '.';
                    }
                    sRet += '9';
                }
                sRet += "EEEE";
            } else {
                for (i = 0; i < (dPrecision_ - dScale_); i++){
                    if (i < dPrecision_){
                        sRet += '9';
                    } else {
                        sRet += '0';
                    }
                }
            }
        }
        return sRet;
    }
};

namespace nsReprVar  /* variable length representation */
{

class cFixedNumber /* For numeric in low precision and normal scale. */
    : public cAttr
    , private cAttrImpl
    , private cNumberImpl
{
private:
    sb4 iPrtSize_;
public:
    cFixedNumber(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , cNumberImpl(cAttrImpl::dPrecision_, cAttrImpl::dScale_)
        , iPrtSize_(0)
    {
        type_ = oracle::occi::OCCI_SQLT_CHR;
        size_ = iGetPrtSize();
        iPrtSize_ = size_;
        iWidth_ = iPrtSize_;
        sType_ = "DECIMAL EXTERNAL";
    }
    virtual ~cFixedNumber()
    {
#ifndef NDEBUG
        trc_ << boost::format("%s; %s") % __PRETTY_FUNCTION__ % sName_ << std::endl;
#endif
    }
    virtual std::string sGetFieldName() const {return cAttrImpl::sGetFieldName(); }
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter& ) const
    {
        return ps::lib::sMakeEnclosedName(sName_, MINIMUM_CTRFLD_LENGTH) + " "
            + sType_ + "(" + boost::lexical_cast<std::string>(iWidth_) + ")"
        ;
    }
    virtual int32_t iGetBufMemSize() const { return cAttrImpl::iGetBufMemSize(); }
    virtual void vSetDataBuffer(ps::lib::sql::occi::cDefine& oDefine)
    {
        vAllocMemory();
        cAttrImpl::vSetDataBuffer(oDefine);
    }
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
            oDelim.vUnCls(
                oRowBuf[iRow]
                , sBuf.assign(static_cast<char *>(data_) + (size_ * iRow), length_[iRow])
                , static_cast<ps::lib::sql::ind_t>(ind_[iRow])
                , iSep
            );
        }
    }
    virtual std::string sGetFieldType() const { return cAttrImpl::sGetFieldType(); }
};

class cOtherNumber /* For numeric in high precision or real number. */
    : public cAttr
    , private cAttrImpl
    , private cNumberImpl
{
private:
    typedef OCINumber tValueType;
    sb4 iPrtSize_;
    std::string sNumFmt_;
    char *szBuffer_;
    mutable ps::lib::sql::occi::cOciErr oOciErr_;
public:
    cOtherNumber(
        ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::Type& dType
        , const std::string& sName
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    )
        : cAttrImpl(oOciStmt, pos, dType, sName, meta, iBulkSize)
        , cNumberImpl(cAttrImpl::dPrecision_, cAttrImpl::dScale_)
        , iPrtSize_(0), sNumFmt_(""), szBuffer_(0)
    {
        type_ = oracle::occi::OCCI_SQLT_VNU;
        size_ = sizeof(tValueType);
        iPrtSize_ = iGetPrtSize();
        sNumFmt_ = sMakeNumFmt();
        iWidth_ = iPrtSize_;
        sType_ = "DECIMAL EXTERNAL";
        szBuffer_ = new char[iPrtSize_ + 1];
    }
    virtual ~cOtherNumber()
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
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter& ) const
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
            ub4 iBuffer = iPrtSize_;
            ps::lib::sql::ind_t ind = static_cast<ps::lib::sql::ind_t>(ind_[iRow]);
            if (ind == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
            {
                ps::lib::sql::occi::vNumberToText(
                    oOciErr_, &static_cast<tValueType*>(data_)[iRow], sNumFmt_
                    , szBuffer_, iBuffer
                );
                if (szBuffer_[iBuffer - 1] == '.')
                {
                    szBuffer_[iBuffer - 1] = '\0';
                    --iBuffer;
                }
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

