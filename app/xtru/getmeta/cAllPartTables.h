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
 * @class cAllPartTables
 * @brief
 * provides the subset data of ALL_PART_TABLES.
 * - Owner of the partitioned table
 * - Name of the partitioned table
 * - Type of the partitioning method:
 * - Type of the composite partitioning method:
 * - Number of partitions in the table.
 * - For a composite-partitioned table,
 * - Number of columns in the partitioning key
 * - For a composite-partitioned table,
 * - Default tablespace to be used when adding a partition
 * - Default value of PCTFREE to be used when adding a partition
 * - Default value of PCTUSED to be used when adding a partition
 * - Default value of INITRANS to be used when adding a partition
 * - Default value of MAXTRANS to be used when adding a partition
 * - Default LOGGING attribute to be used when adding a partition:
 * - Default value of INITIAL (in Oracle blocks) to be used when adding a partition,
 * - Default value of NEXT (in Oracle blocks) to be used when adding a partition,
 * - Default value of MINEXTENTS to be used when adding a partition,
 * - Default value of MAXEXTENTS to be used when adding a partition,
 * - Default value of PCTINCREASE to be used when adding a partition,
 * - Default value of FREELISTS to be used when adding a partition
 * - Default value of FREELIST GROUPS to be used when adding a partition
 * - Default compression to be used when adding a partition:
 * - Default compression for what kind of operations to be used when adding a partition:
 * - Default buffer pool to be used when adding a partition:
 * - Name of the partitioning referential constraint for reference-partitioned tables
 * - String of the interval value
 * - Function dependence on tAttributes::iDefMaxExtents
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
    cAllTabSubPartitions oAllTabSubPartitions;
    cAllTabPartitions oAllTabPartitions(...);
    cAllPartKeyColumns oAllPartKeyColumns;
    cAllSubPartKeyColumns oAllSubPartKeyColumns;
    std::shared_ptr<cAllSubPartitionTemplates> oAllSubPartitionTemplates(
        new cAllSubPartitionTemplates(oAllTabSubPartitions)
    );
    ...
    ps::app::xtru::getmeta::cAllPartTables oObj(
        oAllTabSubPartitions, oAllTabPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns, oAllSubPartitionTemplates
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21"), true) ;
   @endcode
 */
class cAllPartTables
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
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllPartTables::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllPartTables&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllPartTables::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_PART_TABLES
    static const char szAllPartTables[];
    static const char szComprsReady[];     ///< adapt for Oracle 9i Release 2.
    static const char szComprsNotReady[];
    static const char szCompForReady[];    ///< adapt for Oracle 11g Release 1.
    static const char szCompForNotReady[];
    static const char szMoreThan_8i[];
    static const char szMoreThan_11g[];
    cAllTabSubPartitions& oAllTabSubPartitions_;
    cAllTabPartitions& oAllTabPartitions_;
    cAllPartKeyColumns& oAllPartKeyColumns_;
    cAllSubPartKeyColumns& oAllSubPartKeyColumns_;
    std::shared_ptr<cAllSubPartitionTemplates>& oAllSubPartitionTemplates_;
public:
    /**
     * @brief
     */
    cAllPartTables(
        cAllTabSubPartitions& oAllTabSubPartitions
        , cAllTabPartitions& oAllTabPartitions
        , cAllPartKeyColumns& oAllPartKeyColumns
        , cAllSubPartKeyColumns& oAllSubPartKeyColumns
        , std::shared_ptr<cAllSubPartitionTemplates>& oAllSubPartitionTemplates
    );
    virtual ~cAllPartTables();
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
    void vPrintFullItems(
        std::ostream& os
        , const tKeyTuple& oKey
    );
    void vPrintStorageItems(
        std::ostream& os
        , const tKeyTuple& oKey
    );
private:
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
