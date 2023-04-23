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

namespace lite3
{

/**
 * @class cDefine
 *   Bind the host variable to the selected column and get the value.
 */
class cDefine
{
private:
    cSqliteStmt& oStmt_;
    cAttr::tContainer oArr_;   ///< An array of cAttr.
    cDefine& operator=(const cDefine& rhs);
public:
    cDefine(cSqliteStmt& oStmt);
    cDefine(cDefine& rhs);
    /**
     * @brief
     *   Notify SQL of the length and address of the host variable.
     *
     * @param[in,out] pData
     *   @copydetails cAttrImpl::pData_
     * @param[in] iType
     *   to select concrete class.
     * @param[in,out] iInd
     *   @copydetails cAttrImpl::iInd_
     * @param[in] iDSkip
     *   @copydetails cAttrImpl::iDSkip_
     * @param[in] iISkip
     *   @copydetails cAttrImpl::iISkip_
     */
    template<typename T>
    void vAddItem(
        T& pData
        , const cAttr::tLite3Type& iType
        , ps::lib::sql::ind_t* iInd
        , const size_t& iDSkip
        , const size_t& iISkip
    ){
        const int32_t pos = oArr_.size(); // "+1" is not done, but it is not a mistake.
        oArr_.push_back(
            cAttr::oMakeInstance(
                (boost::format("%s_%d") % typeid(T).name() % pos).str()
                , oStmt_, pos, sizeof(T), iType
                , &pData, iInd, iDSkip, iISkip
            )
        );
    }
    /**
     * @brief
     * - Transfer the internal data acquired by sqlite3_step()
     *   to the host variables.
     * - It is premised that host variables are associated
     *   with a statement in advance using cDefine::vAddItem().
     *
     * @param[in] i
     * - Subscript of the transfer destination when the host
     *   variable forms an array.
     * - Always pass 0 if the host variable is not an array.
     */
    void vAcquire(const size_t& i);
    /**
     * @brief
     */
    size_t iGetSize(void) const
    {
        return oArr_.size();
    }
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps


