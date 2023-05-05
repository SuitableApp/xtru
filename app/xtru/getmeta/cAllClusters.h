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
 * @class cAllClusters
 * @brief
 * provides the subset data of ALL_CLUSTERS.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the partitioned tables.
 *   The information includes the following:
 *   - Owner of the cluster
 *   - Name of the cluster
 *   - Minimum percentage of free space in a block
 *   - Minimum percentage of used space in a block
 *   - Initial number of transactions
 *   - Maximum number of transactions
 *   - Estimated size of cluster key plus associated rows
 *   - Type of the cluster:
 *   - If the cluster is a hash cluster, the hash function
 *   - If the cluster is a hash cluster, the number of hash keys (hash buckets)
 *   - Number of threads per instance for scanning the cluster, or DEFAULT
 *   - Number of instances across which the cluster is to be scanned , or DEFAULT
 *   - Indicates whether the cluster is to be cached in the buffer cache (Y) or not (N)
 *   - Indicates whether this is a single-table cluster (Y) or not (N)
 *   - Percentage increase in extent size
 *   - Number of process freelists allocated to this segment
 *   - Number of freelist groups allocated to this segment
 *   - Buffer pool to be used for cluster blocks:
 *   - Name of the tablespace containing the cluster
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    cUserSegments oUserSegments;
    cAllPartTables oAllPartTables(...);
    ...
    ps::app::xtru::getmeta::cAllClusters oObj(
        oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
        , oUserSegments, oAllTabColumns, oHashExpr, oUserIndexes
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21"), true) ;
   @endcode
 */
class cAllClusters
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
     */
    typedef std::tuple<const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllClusters::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllClusters&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllClusters::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// number of elements in oList_.
    size_t iNumRows_;
    /// @brief A SQL-select to select ALL_CLUSTERS
    static const char szAllClusters[];
    static const char szSingleTableReady[];
    static const char szSingleTableNotReady[];
	cAllTabPrivs& oAllTabPrivs_;
	cUserObjAuditOpts& oUserObjAuditOpts_;
	cUserAuditObject& oUserAuditObject_;
	cUserSegments& oUserSegments_;
	cAllTabColumns& oAllTabColumns_;
	cHashExpr& oHashExpr_;
	cUserIndexes& oUserIndexes_;
public:
    /**
     * @brief
     */
    cAllClusters(
        cAllTabPrivs& oAllTabPrivs
        , cUserObjAuditOpts& oUserObjAuditOpts
        , cUserAuditObject& oUserAuditObject
        , cUserSegments& oUserSegments
        , cAllTabColumns& oAllTabColumns
        , cHashExpr& oHashExpr
        , cUserIndexes& oUserIndexes
    );
    virtual ~cAllClusters();
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
     * @copydoc cAllSequences::iGetNumRows()
     */
    size_t iGetNumRows(void) const
    {
        return iNumRows_;
    }
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
