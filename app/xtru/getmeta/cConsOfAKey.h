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
 * @class cConsOfAKey
 * @brief
 * provides the subset data of ALL_CONSTRAINTS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the privileges exercisable.
 *   The information includes the following:
 *   - Name of object owner
 *   - Name of object
 *   - 1:PK, 2:UK
 *   - Name of the constraint definition
 *   - Name associated with the table (or view) with the constraint definition
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cAllConsColumns oAllConsColumnsKey(true, ...);;
    ...
    ps::app::xtru::getmeta::cConsOfAKey oObj(oAllConsColumnsKey);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    oObj.oPrintItems(
        std::cout, std::make_tuple("SCOTT", "EMP", "PK_EMP")
        , true, true, cConsOfAKey::iConsInTable);
   @endcode
 */
class cConsOfAKey
{
public:
    /// @enum tConsInAny
    typedef enum
    { iConsInTable ///< context in the create table statement.
    , iConsInSnap  ///< context in the create matirialized view statement.
    } tConsInAny
    ;
private:
    /**
     * @struct tAttributes
     * @brief
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
     *     (These are only referenced when vPrintAllItems is called.)
     * - third value is passed to tAttributes::szConstraintName.
     *     (These are only referenced when iSubtractCtxItemList is called.)
     */
    typedef std::tuple<const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cConsOfAKey::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cConsOfAKey&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cConsOfAKey::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_CONSTRAINTS
    static const char szConsOfAKey[];
    cAllConsColumns& oAllConsColumnsKey_; ///< Columns constituting PK/AK/FK.
    bool iIotType_;
    tConsInAny iConsInAny_;
public:
    /**
     * @brief
     */
    cConsOfAKey(
        cAllConsColumns& oAllConsColumnsKey
    );
    virtual ~cConsOfAKey();
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
     * @param [in] iIotType
     * @param [in] iConsInAny
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const bool& iIotType
        , const tConsInAny& iConsInAny
    );
    /**
     * @param [in] oKey
     */
    size_t iSubtractCtxItemList(
        const tKeyTuple& oKey
    );
private:
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
