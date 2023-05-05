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
 * @class cAllMviews
 * @brief
 * provides the subset data of ALL_MVIEWS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the properties of the materialized views.
 *   The information includes the following:
 *   - 
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cAllMviews oObj(
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cAllMviews
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
     */
    typedef std::tuple<const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllMviews::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllMviews&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllMviews::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// number of elements in oList_.
    size_t iNumRows_;
    /// @brief A SQL-select to select ALL_MVIEWS
    static const char szAllMviews[];
    cAllTabPrivs& oAllTabPrivs_;
    cUserObjAuditOpts& oUserObjAuditOpts_;
    cUserAuditObject& oUserAuditObject_;
    cConsOfAKey& oConsOfAKey_;
    cEffectiveKeyCons& oEffectiveKeyCons_;
    cAllTables& oAllTables_;
    cAllTriggers& oAllTriggers_;
    cAllMviewLogs& oAllMviewLogs_;
    cAllTabComments& oAllTabComments_;
    cAllColComments& oAllColComments_;
    cUserCluColumns& oUserCluColumns_;
    cUserIndexes& oUserIndexes_;
public:
    /**
     * @brief
     */
    cAllMviews(
        cAllTabPrivs& oAllTabPrivs
        , cUserObjAuditOpts& oUserObjAuditOpts
        , cUserAuditObject& oUserAuditObject
        , cConsOfAKey& oConsOfAKey
        , cEffectiveKeyCons& oEffectiveKeyCons
        , cAllTables& oAllTables
        , cAllTriggers& oAllTriggers
        , cAllMviewLogs& oAllMviewLogs
        , cAllTabComments& oAllTabComments
        , cAllColComments& oAllColComments
        , cUserCluColumns& oUserCluColumns
        , cUserIndexes& oUserIndexes
    );
    virtual ~cAllMviews();
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
