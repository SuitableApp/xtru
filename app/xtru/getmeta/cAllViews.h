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
 * @class cAllViews
 * @brief
 * provides the subset data of ALL_VIEWS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the comment of tables.
 *   The information includes the following:
 *   - Owner of the view
 *   - Name of the view
 *   - View text
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    cAllTabPrivs oAllTabPrivs;
    ...
    cUserObjAuditOpts oUserObjAuditOpts;
    ...
    cUserAuditObject oUserAuditObject;
    ...
    cAllTriggers oAllTriggers;
    ...
    cAllTabComments oAllTabComments;
    ...
    cAllColComments oAllColComments;
    ...
    cAllTabColumns oAllTabColumns(
    ...
    ps::app::xtru::getmeta::cAllViews oObj(
        oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
		, oAllTriggers, oAllTabComments, oAllColComments, oAllTabColumns
    );
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, conf_.as<int32_t>("cmntlvl")));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT"), true) ;
   @endcode
 */
class cAllViews
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
    typedef void (cAllViews::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllViews&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllViews::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// number of elements in oList_.
    size_t iNumRows_;
    /// @brief A SQL-select to select ALL_VIEWS
    static const char szAllViews[];
    cAllTabPrivs& oAllTabPrivs_;
    cUserObjAuditOpts& oUserObjAuditOpts_;
    cUserAuditObject& oUserAuditObject_;
    cAllTriggers& oAllTriggers_;
    cAllTabComments& oAllTabComments_;
    cAllColComments& oAllColComments_;
    cAllTabColumns& oAllTabColumns_;
public:
    /**
     * @brief
     */
    cAllViews(
		cAllTabPrivs& oAllTabPrivs
        , cUserObjAuditOpts& oUserObjAuditOpts
        , cUserAuditObject& oUserAuditObject
		, cAllTriggers& oAllTriggers
        , cAllTabComments& oAllTabComments
        , cAllColComments& oAllColComments
        , cAllTabColumns& oAllTabColumns
    );
    virtual ~cAllViews();
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
