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
 * @class cAllPartIndexes
 * @brief
 * provides the subset data of ALL_PART_INDEXES.
 * - Owner of the partitioned index
 * - Name of the partitioned table
 * - Name of the partitioned index
 * - Type of the partitioning method:
 * - Type of the composite partitioning method:
 * - Number of partitions in the index.
 * - For a composite-partitioned index,
 * - Number of columns in the partitioning key
 * - For a composite-partitioned index,
 * - Indicates whether the partitioned index is local (LOCAL) or global (GLOBAL)
 * - Indicates whether the partitioned index is prefixed (PREFIXED) or non-prefixed (NON_PREFIXED)
 * - For a local index, the default tablespace to be used when adding or splitting a table partition
 * - For a local index, the default INITIAL value (in Oracle blocks)
 * - For a local index, the default NEXT value (in Oracle blocks)
 * - For a local index, the default MINEXTENTS value
 * - For a local index, the default MAXEXTENTS value
 * - For a local index, the default PCTINCREASE value
 * - For a local index, the default FREELISTS value to be used when adding a table partition
 * - For a local index, the default FREELIST GROUPS value to be used when adding a table partition
 * - For a local index, the default buffer pool to be used when adding a table partition:
 * - String of the interval value
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the partitioned tables.
 *   The information includes the following:
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cAllIndSubPartitions oAllIndSubPartitions;
    cAllIndPartitions oAllIndPartitions(...);
    cAllPartKeyColumns oAllPartKeyColumns;
    cAllSubPartKeyColumns oAllSubPartKeyColumns;
    ...
    ps::app::xtru::getmeta::cAllPartIndexes oObj(
        oAllIndSubPartitions, oAllIndPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21", "TEST21_IE01"), true) ;
   @endcode
 */
class cAllPartIndexes
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
    typedef void (cAllPartIndexes::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllPartIndexes&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllPartIndexes::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_PART_INDEXES
    static const char szAllPartIndexes[];
    static const char szMoreThan_8i[];
    static const char szMoreThan_11g[];
    cAllIndSubPartitions& oAllIndSubPartitions_;
    cAllIndPartitions& oAllIndPartitions_;
    cAllPartKeyColumns& oAllPartKeyColumns_;
    cAllSubPartKeyColumns& oAllSubPartKeyColumns_;
public:
    /**
     * @brief
     */
    cAllPartIndexes(
        cAllIndSubPartitions& oAllIndSubPartitions
        , cAllIndPartitions& oAllIndPartitions
        , cAllPartKeyColumns& oAllPartKeyColumns
        , cAllSubPartKeyColumns& oAllSubPartKeyColumns
    );
    virtual ~cAllPartIndexes();
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
