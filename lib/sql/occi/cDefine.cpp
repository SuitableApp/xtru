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

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cDefine::tDefine::tDefine(
    uint32_t pos
    , void *valuep
    , int32_t value_sz
    , uint16_t dty
    , ps::lib::sql::ind_t *indp
    , uint16_t *rlenp
    , uint16_t *rcodep
    , uint32_t pvskip
    , uint32_t indskip
    , uint32_t rlskip
    , uint32_t rcskip
    , uint32_t mode
    , OCICallbackDefine func
    , void* ctxp
    , uint8_t csfrm
    , uint16_t csid
    , uint16_t pdprc
    , int16_t pdscl
)
    : pos_(pos), valuep_(valuep) , value_sz_(value_sz) , dty_(dty) , indp_(indp)
    , rlenp_(rlenp) , rcodep_(rcodep)
    , pvskip_(pvskip) , indskip_(indskip) , rlskip_(rlskip) , rcskip_(rcskip)
    , mode_(mode), func_(func), ctxp_(ctxp)
    , csfrm_(csfrm) , csid_(csid) , pdprc_(pdprc) , pdscl_(pdscl)
{
    BOOST_ASSERT(pos_);
//    BOOST_ASSERT(valuep_);
//    BOOST_ASSERT(value_sz_);
    BOOST_ASSERT(dty_);
}

cDefine::tDefine::tDefine(
    uint32_t pos
    , void *valuep
    , int32_t value_sz
    , uint16_t dty
    , ps::lib::sql::ind_t *indp
    , uint16_t *rlenp
    , uint16_t *rcodep
    , uint32_t pvskip
    , uint32_t indskip
    , uint32_t rlskip
    , uint32_t rcskip
) : tDefine(
        pos, valuep, value_sz, dty, indp, rlenp, rcodep, pvskip, indskip, rlskip, rcskip
        , OCI_DEFAULT, (OCICallbackDefine) 0, (void*) 0, 0, 0, 0, 0
    )
{}

cDefine::tDefine::tDefine(const tDefine& rhs)
    : pos_(rhs.pos_), valuep_(rhs.valuep_) , value_sz_(rhs.value_sz_) , dty_(rhs.dty_) , indp_(rhs.indp_)
    , rlenp_(rhs.rlenp_) , rcodep_(rhs.rcodep_)
    , pvskip_(rhs.pvskip_) , indskip_(rhs.indskip_) , rlskip_(rhs.rlskip_) , rcskip_(rhs.rcskip_)
    , mode_(rhs.mode_), func_(rhs.func_), ctxp_(rhs.ctxp_)
    , csfrm_(rhs.csfrm_) , csid_(rhs.csid_) , pdprc_(rhs.pdprc_) , pdscl_(rhs.pdscl_)
{}

cDefine::tDefine& cDefine::tDefine::operator=(const tDefine& rhs)
{
    pos_ = rhs.pos_; valuep_ = rhs.valuep_; value_sz_ = rhs.value_sz_; dty_ = rhs.dty_; indp_ = rhs.indp_;
    rlenp_ = rhs.rlenp_; rcodep_ = rhs.rcodep_ ;
    pvskip_ = rhs.pvskip_; indskip_ = rhs.indskip_; rlskip_ = rhs.rlskip_; rcskip_ = rhs.rcskip_;
    mode_ = rhs.mode_; func_ = rhs.func_; ctxp_ = rhs.ctxp_;
    csfrm_ = rhs.csfrm_; csid_ = rhs.csid_; pdprc_ = rhs.pdprc_; pdscl_ = rhs.pdscl_;
    return *this;
}

cDefine::tDefine& cDefine::tDefine::oAssign(
    void *valuep
    , int32_t value_sz
    , ps::lib::sql::ind_t *indp
    , uint16_t *rlenp
    , uint16_t *rcodep
){
    valuep_ = valuep;
    value_sz_ = value_sz;
    indp_ = indp;
    rlenp_ = rlenp;
    rcodep_ = rcodep;
    return *this;
}

cDefine::cDefine(const tTiming& iTiming)
    : iTiming_(iTiming)
{}

cDefine::cDefine(const cDefine& rhs)
    : oMap_(rhs.oMap_)
    , iTiming_(rhs.iTiming_)
{}

cDefine& cDefine::operator=(const cDefine& rhs)
{
    oMap_ = rhs.oMap_;
    iTiming_ = rhs.iTiming_;
    return *this;
}

void cDefine::vSetTiming(const tTiming& iTiming)
{
    BOOST_ASSERT(iTiming_ != iTiming);
    iTiming_ = iTiming;
}

const cDefine::tTiming& cDefine::iGetTiming() const
{
    return iTiming_;
}

void cDefine::vAddItem(
    void *valuep
    , int32_t value_sz
    , uint16_t dty
    , ps::lib::sql::ind_t *indp
    , uint16_t *rlenp
    , uint16_t *rcodep
    , uint32_t pvskip
    , uint32_t indskip
    , uint32_t rlskip
    , uint32_t rcskip
    , uint32_t mode
    , OCICallbackDefine func
    , void* ctxp
    , uint8_t csfrm
    , uint16_t csid
    , uint16_t pdprc
    , int16_t pdscl
)
{
//  BOOST_ASSERT(iTiming_ == tTiming::iRepeat);
    oMap_.push_back(tDefine(
        oMap_.size() + 1, valuep, value_sz
        , dty, indp, rlenp, rcodep
        , pvskip, indskip, rlskip, rcskip
        , mode, func, ctxp
        , csfrm, csid, pdprc, pdscl
    ));
}

void cDefine::vAddItem(
    int32_t value_sz
    , uint16_t dty
    , uint32_t mode
    , OCICallbackDefine func
    , void* ctxp
){
    BOOST_ASSERT(iTiming_ == tTiming::iOnce);
    oMap_.push_back(tDefine(
        oMap_.size() + 1, NULL, value_sz
        , dty, NULL, NULL, NULL
        , 0, 0, 0, 0
        , mode, func, ctxp
        , 0, 0, 0, 0
    ));
}

void cDefine::vAddItem(
    uint16_t dty
    , uint32_t skip
){
    vAddItem(
        (void *)0, 0, dty
        , (ps::lib::sql::ind_t *)0
        , (uint16_t *)0
        , (uint16_t *)0
        , skip, skip, skip, skip
        , OCI_DEFAULT, (OCICallbackDefine) 0, (void*) 0
        , 0, 0, 0, 0
    );
}

void cDefine::vAttachTo(cOciStmt& oOciStmt)
{
    BOOST_ASSERT(oMap_.size());
    for (auto oItem: oMap_)
    {
        vDefineArrayOfStruct(
            oOciStmt
            , oItem.pos_
            , oItem.valuep_
            , oItem.value_sz_
            , oItem.dty_
            , oItem.indp_
            , oItem.rlenp_
            , oItem.rcodep_
            , oItem.pvskip_
            , oItem.indskip_
            , oItem.rlskip_
            , oItem.rcskip_
            , oItem.mode_
            , oItem.func_
            , oItem.ctxp_
            , oItem.csfrm_
            , oItem.csid_
            , oItem.pdprc_
            , oItem.pdscl_
        );
    }
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
