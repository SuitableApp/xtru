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

class cAttr
{
public:
    typedef boost::ptr_vector<cAttr> tContainer;
    typedef tContainer::auto_type tPtr;
    static cAttr * oMakeInstance(
        const std::string& tag
        , ps::lib::sql::occi::cOciStmt& oOciStmt
        , const uint32_t& pos
        , const oracle::occi::MetaData& meta
        , const uint32_t& iBulkSize
    );
    virtual ~cAttr();
    virtual void vSetDataBuffer(ps::lib::sql::occi::cDefine& oDefine) =0;
    virtual std::string sGetFieldName() const =0;
    virtual std::string sGetFieldForCtrl(const ps::lib::cDelimiter& oDelim) const =0;
    virtual int32_t iGetBufMemSize() const =0;
    /**
     * @brief
     *
     * @param[in,out] oRowBuf 
     *   xxxx
     * @param[in] iNumIter 
     *   xxxx
     * @param[in] iSep 
     *   xxxx
     * @param[in] oDelim 
     *   xxxx
     *
     */
    virtual void vConvertStringVct(
        ps::lib::str_vct& oRowBuf
        , const ub4& iNumIter
        , const bool& iSep
        , const ps::lib::cDelimiter& oDelim
    ) const =0;
    virtual std::string sGetFieldType() const =0;
protected:
    cAttr() =default;
private:
    cAttr(const cAttr&) =delete;
    cAttr& operator=(const cAttr&) =delete;
};
 
void vCheckCompatibility(const cAttr::tContainer& lhs, const cAttr::tContainer& rhs) ;

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps


