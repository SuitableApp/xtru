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

char *& cPieceVct::pGetDataAddr(void *src, const size_t& diff)
{
    return *(
        reinterpret_cast<char **>(
            static_cast<char *>(src) + diff
        )
    );
}

ub4 * cPieceVct::pGetLengthAddr(ub4 *src, const size_t& diff)
{
    return 
        reinterpret_cast<ub4 *>(
            reinterpret_cast<char *>(src) + diff
        );
}

cPieceVct::cPieceVct(
    const size_t iSkip
    , char **data
    , ps::lib::sql::ind_t *ind
    , ub4 *length
)
    : conf_(ps::lib::cConfigures::get_const_instance())
    , iSkip_(iSkip), iNumPcs_(0), iActual_(0), iLongest_(0)
    , iPcsLen_(conf_.as<int32_t>("maxlongsize")), data_(data), ind_(ind), length_(length)
{
    BOOST_ASSERT(iSkip);
    BOOST_ASSERT(iPcsLen_);
}

cPieceVct::cPieceVct(
    const size_t iSkip
)
    : cPieceVct(iSkip, nullptr, nullptr, nullptr)
{}

cPieceVct::~cPieceVct()
{}

void cPieceVct::vSetAddr(
    char **data
    , ps::lib::sql::ind_t *ind
    , ub4 *length
){
    BOOST_ASSERT(data);
    BOOST_ASSERT(ind);
    BOOST_ASSERT(length);
    data_ = data;
    ind_ = ind;
    length_ = length;
}

void cPieceVct::vTerminateLatest(cPieceVct *pv, const ub4& iter)
{
    const size_t diff = pv->iSkip_ * (iter - 1);
    ub4 *length = pGetLengthAddr(pv->length_, diff);
    *length += pv->iActual_;
    char *data = pGetDataAddr(pv->data_, diff);
    data[*length] = '\0';
    // Updates if value length exceed the maximum so far.
    if (pv->iLongest_ < *length)
    {
        pv->iLongest_ = *length;
    }
}

sb4 cPieceVct::iCbkFunc(
    dvoid *octxp
    , OCIDefine *defnp
    , ub4 iter
    , dvoid **bufpp
    , ub4 **alenpp
    , ub1 *piecep
    , dvoid **indpp
    , ub2 **rcodep
){
    cPieceVct *pv = static_cast<cPieceVct *>(octxp);
    BOOST_ASSERT(pv->data_);
    BOOST_ASSERT(pv->ind_);
    BOOST_ASSERT(pv->length_);
    const size_t diff = pv->iSkip_ * iter;
    char *data;     // Address of the start of the current piece.
    ub4 *length;
    char *tmp;
    switch (*piecep)
    {
    case OCI_ONE_PIECE:
    case OCI_FIRST_PIECE:
        if (iter) { vTerminateLatest(pv, iter); }
        pv->iNumPcs_ = 1;
        data = pGetDataAddr(pv->data_, diff);
        if (data) { delete [] data; }
        *bufpp = pGetDataAddr(pv->data_, diff) =
            new char[pv->iPcsLen_ * pv->iNumPcs_];
        length = pGetLengthAddr(pv->length_, diff);
        *length = 0;
        break;
    case OCI_NEXT_PIECE:
    case OCI_LAST_PIECE:
        ++pv->iNumPcs_;
        data = pGetDataAddr(pv->data_, diff);
        // Resize buffer.
        tmp = new char[pv->iPcsLen_ * pv->iNumPcs_];
        memcpy(tmp, data, pv->iPcsLen_ * (pv->iNumPcs_ - 1));
        delete [] data;
        pGetDataAddr(pv->data_, diff) = tmp;
        *bufpp = tmp + pv->iPcsLen_ * (pv->iNumPcs_ - 1);
        length = pGetLengthAddr(pv->length_, diff);
        *length += pv->iActual_;
        break;
    default:
        return OCI_ERROR;
    }
    pv->iActual_ = pv->iPcsLen_; // Reset to maximum length.
    /*
     * Pass a pointer to the variable that stores the length of
     * the next piece to be fetched to the OCI.
     */
    *alenpp = &pv->iActual_;
    *indpp = (char *) pv->ind_ + diff;
    *rcodep = NULL;
    return OCI_CONTINUE;
}

std::string::size_type cPieceVct::iMaxValSize() const
{
    return iLongest_;
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

