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

namespace lite3
{

class cSqliteStmt;

/**
 * @class cAttr
 *
 * - Unify the interface of objects that bind
 *   host variables to SQL
 * - Abstraction of an object for binding
 *   host variables and variables in SQL
 *   (column names or place holders).
 * - Since the data types of place folders and
 *   selected columns are various, we try to solve
 *   problems by applying polymorphism.
 * - See cAttrImpl to study implementation.
 */
class cAttr
{
public:
    /**
     * @brief
     * - Sequence of pointers to abstract class type.
     * - Expecting objects to behave polymorphically.
     */
    typedef boost::ptr_vector<cAttr> tContainer;
    /**
     * @enum tLite3Type
     * - Icon for selecting the concrete class
     *   to be created.
     */
    enum tLite3Type
    { INT32  ///< cAttrImplInt32
    , INT64  ///< cAttrImplInt64
    , DOUBLE ///< cAttrImplFlt
    , STR    ///< cAttrImplStr
    , RAW    ///< cAttrImplRaw
    };
    /**
     * @brief
     *
     * @param[in] tag
     * - A string for identifying the problem occurrence.
     * - It is represented by combination of typeid
     *   and integer of position
     * @param[in] oStmt
     * - An envelopes for retrieving cAttrImpl::oDb_
     *   and cAttrImpl::sqlite3_stmt_.
     * @param[in] pos
     *   @copydetails cAttrImpl::pos_
     * @param[in] iDLength
     *   @copydetails cAttrImpl::iDLength_
     * @param[in] iType
     *   to select concrete class.
     * @param[in] pData
     *   @copydetails cAttrImpl::pData_
     * @param[in] iInd
     *   @copydetails cAttrImpl::iInd_
     * @param[in] iDSkip
     *   @copydetails cAttrImpl::iDSkip_
     * @param[in] iISkip
     *   @copydetails cAttrImpl::iISkip_
     */
    static cAttr * oMakeInstance(
        const std::string& tag
        , cSqliteStmt& oStmt
        , const int32_t& pos
        , const int32_t& iDLength
        , const tLite3Type& iType
        , void * pData
        , const ps::lib::sql::ind_t* iInd  // ture: Data present.
        , const size_t& iDSkip
        , const size_t& iISkip
    );
    /**
     * @brief
     * - Bind a host variable to a place folder column
     * - See cBind::vAttachTo() to study detail.
     * - Called by cSqliteStmt::iExecute() or cSqliteStmt::iFetch()
     */
    virtual void vBind(const size_t& i) const =0;
    /**
     * @brief
     * - Fill the host variable with the value
     *   of the selected column.
     * - See cDefine::vAcquire() to study detail.
     * - Called by cSqliteStmt::iFetch()
     */
    virtual void vPick(const size_t& i) =0;
    virtual ~cAttr();
protected:
    cAttr() =default;
private:
    cAttr(const cAttr&);
    cAttr& operator=(const cAttr&);
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

