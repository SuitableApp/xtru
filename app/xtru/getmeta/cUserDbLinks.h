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
 * @class cUserDbLinks
 * @brief
 * provides the subset data of DBA_DB_LINKS
 *   or ALL_DB_LINKS if "DBA_..." is not accessivle for user.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the database links.
 *   The information includes the following:
 *   - Name of the schema object.
 *   - Name of the database link.
 *   - Name of Oracle Net Service.
 *   - Whether an authorized user can grant the same rights to other users.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cUserDbLinks oObj(,...);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cUserDbLinks
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
     * - second value is passed to tAttributes::szDbLink.
     */
    typedef std::tuple<const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cUserDbLinks::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cUserDbLinks&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cUserDbLinks::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// number of elements in oList_.
    size_t iNumRows_;
    /// @brief A SQL-select to select DBA_DB_LINKS
    static const char szDbaDbLinks[];
    /// @brief A SQL-select to select ALL_DB_LINKS
    static const char szAllDbLinks[];
    /**
     * @brief
     * To output the object audit trail.
     */
    cUserAuditObject& oUserAuditObject_;
public:
    /**
     * @brief
     */
    cUserDbLinks(
        cUserAuditObject& oUserAuditObject
    );
    virtual ~cUserDbLinks();
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
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

