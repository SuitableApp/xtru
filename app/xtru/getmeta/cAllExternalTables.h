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
 * @class cAllExternalTables
 * @brief
 * provides the subset data of ALL_EXTERNAL_TABLES
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the external tables.
 *   The information includes the following:
 *   - Owner of the external table
 *   - Name of the external table
 *   - Owner of the implementation type for the external table access driver
 *   - Name of the implementation type for the external table access driver
 *   - Owner of the default directory for the external table
 *   - Name of the default directory for the external table
 *   - Reject limit for the external table, or UNLIMITED
 *   - Type of access parameters for the external table
 *   - Access parameters for the external table
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    cAllExternalLocations oAllExternalLocations;
    ...
    ps::app::xtru::getmeta::cAllExternalTables oObj(oAllExternalLocations);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST25"), true);
   @endcode
 */
class cAllExternalTables
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
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllExternalTables::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllExternalTables&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllExternalTables::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_EXTERNAL_TABLES
    static const char szAllExternalTables[];
    cAllExternalLocations& oAllExternalLocations_;
public:
    /**
     * @brief
     * @param [in] oAllExternalLocations
     */
    cAllExternalTables(
        cAllExternalLocations& oAllExternalLocations
    );
    virtual ~cAllExternalTables();
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
    /**
     * @brief
     *   wether oKey is a external table?
     * @param [in] oKey
     */
    bool iCheckExternal(
        const tKeyTuple& oKey
    );
private:
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
