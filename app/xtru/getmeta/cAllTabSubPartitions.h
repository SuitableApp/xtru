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
 * @class cAllTabSubPartitions
 * @brief
 * provides the subset data of ALL_TAB_SUBPARTITIONS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the infomation of table's sub partition.
 *   The information includes the following:
 *   - Owner of the table
 *   - Name of the table
 *   - Name of the partition
 *   - Name of the subpartition
 *   - Position of the subpartition within the partition
 *   - Name of the tablespace containing the subpartition
 *   - assosiated with all_objects.generated
 *   - Indicates whether this subpartition is compressed (ENABLED) or not (DISABLED)
 *   - Default compression for what kind of operations:
 *   - Subpartition bound value expression
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cAllTabSubPartitions oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, conf_.as<int32_t>("cmntlvl")));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cAllTabSubPartitions
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
     * - therd value is passed to tAttributes::szPartitionName.
     * - fourth values is passed to tAttributes::szSubPartitionName.
     */
    typedef std::tuple<const char*, const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllTabSubPartitions::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllTabSubPartitions&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllTabSubPartitions::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_TAB_SUBPARTITIONS
    static const char szAllTabSubPartitions[];
    static const char szComprsReady[];
    static const char szComprsNotReady[];
    static const char szCompForReady[];
    static const char szCompForNotReady[];
    static const char szHvReady[];
    static const char szHvNotReady[];
    const char *szSubpartitioningType_;
    const char *szDefTablespaceName_;
    const cCompressObject *oCompressObject_;
    ub4 iSubPartitionCount_;
    ps::lib::cSet<std::string> oTbsNames_; ///< name of tablespaces.
public:
    /**
     * @brief
     */
    cAllTabSubPartitions();
    virtual ~cAllTabSubPartitions();
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
     * @param [in] szSubpartitioningType
     * @param [in] szDefTablespaceName
     * @param [in] oCompressObject
     */
    void vRptAllTabSubPartitions(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const char *const szSubpartitioningType
        , const char *const szDefTablespaceName
        , const cCompressObject *oCompressObject
    );
    size_t iSubtractCtxItemList(
        const tKeyTuple& oKey
    );
    void vRptTabSubPartByQuantity(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const ub4 iSubPartitionCount
    );
private:
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPostRepFullPartitionName(std::ostream&, const tAttributes&) const;
    void vPostRepeatTabPartByQty(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
