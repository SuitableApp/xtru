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
 * @class cUserIndexes
 * @brief
 * provides the subset data of ALL_INDEXES.
 * - Owner of the index
 * - Name of the indexed object
 * - Name of the index
 * - Type of the index:
 * - Threshold percentage of block space allowed per index entry
 * - Column ID of the last column
 * - Minimum percentage of free space in a block
 * - Initial number of transactions
 * - Maximum number of transactions
 * - Indicates whether or not changes to the index are logged YES or NO.
 * - Percentage increase in extent size
 * - Number of process freelists allocated to this segment
 * - Number of freelist groups allocated to this segment
 * - Buffer pool to be used for index blocks:
 * - Name of the tablespace containing the index
 * - Number of threads per instance for scanning the index, or DEFAULT
 * - Number of instances across which the indexes to be scanned, or DEFAULT
 * - Indicates whether the index is partitioned (YES) or not (NO)
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the indexes.
 *   The information includes the following:
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cAllIndColumns oAllIndColumns(...);
    cUserSegments oUserSegments;
    cAllTables oAllTables(...);
    cAllTabColumns oAllTabColumns(...);
    cAllPartIndexes oAllPartIndexes(...);
    ...
    ps::app::xtru::getmeta::cUserIndexes oObj(
        oAllIndColumns, oUserSegments
        , oAllTables, oAllTabColumns, oAllPartIndexes
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21", "TEST21_IE01"), true) ;
   @endcode
 */
class cUserIndexes
{
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
     * - third value is passed to tAttributes::szIndexName.
     */
    typedef std::tuple<const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cUserIndexes::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cUserIndexes&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cUserIndexes::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_INDEXES
    static const char szUserIndexes[];
    cAllIndColumns& oAllIndColumns_;
    cUserSegments& oUserSegments_;
    cAllTables& oAllTables_;
    cAllTabColumns& oAllTabColumns_;
    cAllPartIndexes& oAllPartIndexes_;
public:
    /**
     * @brief
     */
    cUserIndexes(
        cAllIndColumns& oAllIndColumns
        , cUserSegments& oUserSegments
        , cAllTables& oAllTables
        , cAllTabColumns& oAllTabColumns
        , cAllPartIndexes& oAllPartIndexes
    );
    virtual ~cUserIndexes();
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
     * @param [in] iIot true: Index organized table.
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const bool& iIot
    );
    /**
     * @brief
     * Output definition of USING INDEX clause
     */
    void vRptUsingIndexClause(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
    /**
     * @copydoc cAllSequences::vPrintAllItems()
     * @param [in] iWayToNext
     * - iForward: to describe 'DROP INDEX'
     * - iBackward: to describe 'CREATE INDEX'
     */
    void vRptDrpCreUserIndexes(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const ps::app::xtru::getmeta::tWayToNext iWayToNext
    );
    /**
     * @brief
     */
    size_t iSubtractCtxItemList(
        const tKeyTuple& oKey
    );
    /**
     * @brief
     */
    void vReverse();
private:
    /// @copybrief cAllSequences::vPrintPostRows()
    void vOnPreDropCreIndex(std::ostream& os, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
