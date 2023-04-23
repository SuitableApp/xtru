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
 * @class cAllConsColumns
 * @brief
 * provides the subset data of ALL_CONS_COLUMNS.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of partitioning key columns.
 *   The information includes the following:
 *   - Owner of the constraint definition
 *   - Name of the table with the constraint definition
 *   - Name of the constraint definition
 *   - Name of the column or attribute of the object type column specified in the constraint definition
 *   - Original position of the column or attribute in the definition of the object
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    cAllTabColumns oAllTabColumnsa;
    ...
    ps::app::xtru::getmeta::cAllConsColumns oObj(true, oAllTabColumnsa);
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "PK_EMP"), true);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "FK_DEPTNO"), true);
   @endcode
 */
class cAllConsColumns
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
     * - third value is passed to tAttributes::szConstraintName.
     */
    typedef std::tuple<const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllConsColumns::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllConsColumns&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllConsColumns::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief Return column names that compose constraints (PK, AK, FK)
    static const char szAllConsColumnsKeyA[];
    /// @brief Returns the column names that compose
    /// the referential constraint (PK, AK) of the referential constraint
    static const char szAllConsColumnsRefA[];
    const bool iQueryType_;
    cAllTabColumns& oAllTabColumns_;
public:
    /**
     * @brief
     * @param [in] iQueryType
     *     true: szAllConsColumnsKeyA
     *     false: szAllConsColumnsRefA
     * @param [in] oAllTabColumns
     */
    cAllConsColumns(
        const bool iQueryType
        , cAllTabColumns& oAllTabColumns
    );
    virtual ~cAllConsColumns();
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
     * @copydoc cAllSequences::vPrintAllItems()
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
private:
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
