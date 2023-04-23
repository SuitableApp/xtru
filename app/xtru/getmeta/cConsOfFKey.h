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
 * @class cConsOfFKey
 * @brief
 * provides the subset data of ALL_CONSTRAINTS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the privileges exercisable.
 *   The information includes the following:
 *   - Name of object owner
 *   - Name of object
 *   - Name of the constraint definition
 *   - Name associated with the table (or view) with the constraint definition
 *   - Name of the unique constraint definition for the referenced table
 *   - Delete rule for a referential constraint:
 *   - Indicates whether the constraint is deferrable or not
 *   - Indicates whether the constraint was initially deferred or not
 *   - The meaning depends on the value of tAttributes::szStatus.
 *   - Indicates whether the name of the constraint is user-generated or system-generated
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cAllConsColumns oAllConsColumnsKey(true, ...);;
    cAllConsColumns oAllConsColumnsRef(false, ...);
    ...
    ps::app::xtru::getmeta::cConsOfFKey oObj(oAllConsColumnsKey, oAllConsColumnsRef);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    oObj.oPrintItems(std::cout, std::make_tuple("SCOTT", "EMP")
        , true, cConsOfFKey::iInCreTbl);
   @endcode
 */
class cConsOfFKey
{
public:
    /// @enum tInvCtx
    typedef enum
    { iInCreTbl ///< context in the create table statement.
    , iInAltTbl ///< context in the alter table add statement.
    } tInvCtx
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
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cConsOfFKey::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cConsOfFKey&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cConsOfFKey::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_CONSTRAINTS
    static const char szConsOfFKey[];
    cAllConsColumns& oAllConsColumnsKey_; ///< Columns constituting PK/AK/FK.
    cAllConsColumns& oAllConsColumnsRef_; ///< Columns referenced by FK.
    tInvCtx iInvCtx_;
public:
    /**
     * @brief
     */
    cConsOfFKey(
        cAllConsColumns& oAllConsColumnsKey
        , cAllConsColumns& oAllConsColumnsRef
    );
    virtual ~cConsOfFKey();
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
     * @param [in] iInvCtx
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const tInvCtx& iInvCtx
    );
private:
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
