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
 * @class cAllMviewLogs
 * @brief
 * provides the subset data of ALL_MVIEW_LOGS.
 * - Owner of the materialized view log
 * - Name of the master table or master materialized view whose changes are logged
 * - Name of the table where the changes to the master table or master materialized view are logged
 * - Indicates whether rowid information is recorded (YES) or not (NO)
 * - Indicates whether primary key information is recorded (YES) or not (NO)
 * - Indicates whether object identifier information in an object table is recorded (YES) or not (NO)
 * - Indicates whether filter column information is recorded (YES) or not (NO)
 * - Indicates whether the sequence value, which provides additional ordering information, is recorded (YES) or not (NO)
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
    cAllTabPrivs oAllTabPrivs;
    cUserObjAuditOpts oUserObjAuditOpts(...);
    cUserAuditObject oUserAuditObject;
    cAllTabComments oAllTabComments;
    std::shared_ptr<cDbaMviewLogFilterCols> oDbaMviewLogFilterCols(
    ...
    ps::app::xtru::getmeta::cAllMviewLogs oObj(
        oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
        , oAllTabComments, oDbaMviewLogFilterCols
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "TEST21"), true) ;
   @endcode
 */
class cAllMviewLogs
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
     * - second value is passed to tAttributes::szMaster.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllMviewLogs::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllMviewLogs&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllMviewLogs::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_MVIEW_LOGS
    static const char szAllMviewLogs[];
	cUserSegments& oUserSegments_;
    cAllTabPrivs& oAllTabPrivs_;
    cUserObjAuditOpts& oUserObjAuditOpts_;
    cUserAuditObject& oUserAuditObject_;
    cAllTabComments& oAllTabComments_;
    cAllColComments& oAllColComments_;
    std::shared_ptr<cDbaMviewLogFilterCols>& oDbaMviewLogFilterCols_;
public:
    /**
     * @brief
     */
    cAllMviewLogs(
        cUserSegments& oUserSegments
        , cAllTabPrivs& oAllTabPrivs
        , cUserObjAuditOpts& oUserObjAuditOpts
        , cUserAuditObject& oUserAuditObject
        , cAllTabComments& oAllTabComments
        , cAllColComments& oAllColComments
        , std::shared_ptr<cDbaMviewLogFilterCols>& oDbaMviewLogFilterCols
    );
    virtual ~cAllMviewLogs();
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
