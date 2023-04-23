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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cKeyColumns
 *
 * @brief
 * <b>UNDER CONSTRUCTION.</b><br />
 * provides the subset data of ALL_CONSTRAINTS, ALL_CONS_COLUMNS and ALL_TAB_COLS
 *
 * @details
 * This class plays following role.
 * -# collects the information of the audit trail on database
 *   , and keeps them in this instance as a list container
 *   to generate the SQL statements.
 *   Following items are included in the information:
 *   - Name of object owner.
 *   - Name of object name. 
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cKeyColumns oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP","EMPNO","P"), true) ; // primary key
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP","DEPTNO","R"), true) ; // foreign key
   @endcode
 */
class cKeyColumns
{
private:
    /**
     * @struct tAttributes
     * The structure of the row data is defined.
     * Details are defined in cpp.
     */
    struct tAttributes;
    /**
     * @class cRetriever
     * Details are defined in cpp.
     */
    class cRetriever;
    /**
     * @copybrief cAllSequences::tKeyTuple
     *
     * @details
     * - first value is passed to tAttributes::szOwner.
     * - second value is passed to tAttributes::szTableName.
     * - second value is passed to tAttributes::szColumnName.
     * - second value is passed to tAttributes::szConstraintType.
     */
    typedef std::tuple<const char*, const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cKeyColumns::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cKeyColumns&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cKeyColumns::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_CONSTRAINTS, ALL_CONS_COLUMNS and ALL_TAB_COLS
    static const char szKeyColumns[];
public:
    /**
     * @brief
     */
    cKeyColumns();
    virtual ~cKeyColumns();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
    );
    /**
     * @brief
     */
    ps::lib::sql::ind_t iDoesConstraintHas(
        const tKeyTuple& oKey
    );
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
