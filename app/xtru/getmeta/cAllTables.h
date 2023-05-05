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
 * @class cAllTables
 * @brief
 * provides the subset data of ALL_TABLES.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the partitioned tables.
 *   The information includes the following:
 *   - Owner of the table
 *   - Name of the table
 *   - Indicates whether the table is partitioned (YES) or not (NO)
 *   - Name of the cluster, if any, to which the table belongs
 *   - Name of the index-organized table (IOT),
 *   - Buffer pool for the table. Indicates one of:
 *   - If the table is an index-organized table, this indicates one of:
 *   - Name of the tablespace containing the table
 *   - Indicates whether partitioned row movement is enabled (ENABLED) or disabled (DISABLED)
 *   - Indicates whether table compression is enabled (ENABLED) or not (DISABLED)
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cUserSegments oUserSegments;
    cAllPartTables oAllPartTables(...);
    ...
    ps::app::xtru::getmeta::cAllTables oObj(oUserSegments, oAllPartTables);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21"), true) ;
   @endcode
 */
class cAllTables
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
    typedef void (cAllTables::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllTables&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllTables::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_TABLES
    static const char szAllTables[];
    static const char szAllTables_00_r10g[];
    static const char szComprsReady[];
    static const char szComprsNotReady[];
    static const char szCompForReady[];
    static const char szCompForNotReady[];
    cUserSegments& oUserSegments_;
    cAllPartTables& oAllPartTables_;
public:
    /**
     * @brief
     */
    cAllTables(
        cUserSegments& oUserSegments
        , cAllPartTables& oAllPartTables
    );
    virtual ~cAllTables();
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
        , const bool& iDispose
    );
    /**
     * to describe the overflow segment on the IOT.
     */
    void vPrintIotOvfItems(
        std::ostream& os
        , const tKeyTuple& oKey
    );
    /**
     * @brief
     *   checks table is whether a part of the cluster or not.
     * @param [in] oKey
     *   Give a owner and table name to check.
     * @param [out] szClusterName
     *   Give a pointer to char array to copy the cluster name.
     * @return
     *   true: table is a part of the cluster.
     */
    bool iCheckClusterTable(
        const tKeyTuple& oKey
        , char *const szClusterName
    ) const;
    /**
     * @brief
     *   checks table is whether an index organized table (IOT) or not.
     * @param [in] oKey
     *   Give a owner and table name to check.
     * @return
     *   true: table is OIT.
     */
    bool iCheckIot(
        const tKeyTuple& oKey
    ) const;
private:
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
