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
 * @class cAllIndPartitions
 * @brief
 * provides the subset data of ALL_IND_PARTITIONS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the infomation of table's sub partition.
 *   The information includes the following:
 *   - Owner of the index
 *   - Name of the index
 *   - Indicates whether the partition belongs
 *   - Name of the partition
 *   - the number of subpartitions in the partition
 *   - Position of the partition within the index
 *   - Name of the tablespace containing the partition
 *   - Indicates the actual compression property
 *   - Subpartition bound value expression
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::app::xtru::getmeta::cAllIndSubPartitions oAllIndSubPartitions;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    oFunctionalReplace(...);
    ...
    ps::app::xtru::getmeta::cAllIndPartitions oObj(oAllIndSubPartitions);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    oObj.vRptAllIndPartitions(std::cout
        , std::make_tuple("SCOTT", "TEST21_IE01")
        , true, "RANGE", "HASH", "SYSTEM", "LOCAL"
    );
   @endcode
 */
class cAllIndPartitions
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
     * - second value is passed to tAttributes::szIndexName.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllIndPartitions::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllIndPartitions&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllIndPartitions::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_IND_PARTITIONS
    static const char szAllIndPartitions[];
    static const char szComprsReady[];
    static const char szComprsNotReady[];
	cAllIndSubPartitions& oAllIndSubPartitions_;
    const char *szPartitioningType_;
    const char *szSubpartitioningType_;
    const char *szDefTablespaceName_;
    const char *szLocality_;
    ps::lib::cSet<std::string> oTbsNames_; ///< name of tablespaces.
public:
    /**
     * @brief
     */
    cAllIndPartitions(
        cAllIndSubPartitions& oAllIndSubPartitions
    );
    virtual ~cAllIndPartitions();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     * @param [in] oFunctionalReplace
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const cFunctionalReplace& oFunctionalReplace
    );
    /**
     * @copydoc cAllSequences::vPrintAllItems()
     * @param [in] szPartitioningType
     * @param [in] szSubpartitioningType
     * @param [in] szDefTablespaceName
     * @param [in] szLocality
     */
    void vRptAllIndPartitions(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const char *const szPartitioningType
        , const char *const szSubpartitioningType
        , const char *const szDefTablespaceName
        , const char *const szLocality
    );
    /**
     * @copydoc cAllSequences::vPrintAllItems()
     */
    void vRptIndPartByQuantity(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
private:
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPostRepFullPartitionName(std::ostream&, const tAttributes&) const;
    void vPostRepeatIndPartByQty(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
