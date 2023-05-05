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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cAllConstraints
 * @brief
 * <b>UNDER CONSTRUCTION.</b><br />
 * provides the subset data of ALL_CONSTRAINTS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the comment of tables.
 *   The information includes the following:
 *   - Name of object owner.
 *   - Name of constraint.
 *   - Type of constraint.
 *   - Name of table.
 *   - Content of the check constraint.
 *   - PK or AK name referenced by FK.
 *   - Name of PK or AK name.
 *   - Instruction to keep referential integrity.
 *   - Enforcement status of the constraint.
 *   - Indicates whether the constraint is deferrable or not.
 *   - Indicates whether the constraint was initially deferred or not.
 *   - The meaning depends on the value of tAttributes::szStatus.
 *   - Indicates whether the name of the constraint is user-generated or system-generated
 *   - Name of the user owning the index.
 *   - Name of the index (only shown for unique and primary-key constraints).
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cAllConstraints oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "P"), true) ;
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "U"), true) ;
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "C"), true) ;
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP", "R"), true) ;
   @endcode
 */
class cAllConstraints
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
     * - second value is passed to tAttributes::szConstraintName.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllConstraints::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllConstraints&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllConstraints::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_CONSTRAINTS
    static const char szAllConstraints[];
    static const char szAplddT03_r9[];
    static const char szAplddT03_r8[];
public:
    /**
     * @brief
     */
    cAllConstraints();
    virtual ~cAllConstraints();
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
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintNotFound()
    void vPrintNotFound(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
