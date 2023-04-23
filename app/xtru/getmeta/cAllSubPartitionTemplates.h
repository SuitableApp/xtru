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
 * @class cAllSubPartitionTemplates
 * @brief
 * provides the subset data of ALL_SUBPARTITION_TEMPLATES
 *
 * @note
 * ALL_SUBPARTITION_TEMPLATES was appeared in Release 9.2 or later.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the infomation of table's sub partition.
 *   The information includes the following:
 *   - Owner of the table
 *   - Name of the table
 *   - Name of the subpartition
 *   - Position of the subpartition
 *   - Name of the Tablespace name of the subpartition.
 *   - Literal list values of the subpartition
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    cFunctionalReplace oFunctionalReplace;
    ...
    cAllTabSubPartitions oAllTabSubPartitions;
    oQueue_.push_back(oAllTabSubPartitions.oSubmit(*oSvc_, oOwners_, ...));
    ...
    ps::app::xtru::getmeta::cAllSubPartitionTemplates oObj(oAllTabSubPartitions);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cAllSubPartitionTemplates
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
    typedef void (cAllSubPartitionTemplates::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllSubPartitionTemplates&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllSubPartitionTemplates::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    ps::lib::cTracer& trc_;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_SUBPARTITION_TEMPLATES
    static const char szAllSubPartitionTemplates[];
	cAllTabSubPartitions& oAllTabSubPartitions_;
    std::string sSubpartitioningType_;
public:
    /**
     * @brief
     */
    cAllSubPartitionTemplates(
        cAllTabSubPartitions& oAllTabSubPartitions
    );
    virtual ~cAllSubPartitionTemplates();
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
     * @param [in] sSubpartitioningType
     *    "RANGE", "LIST" or "HASH"
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
        , const std::string& sSubpartitioningType
    );
private:
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
