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
 * @class cUserAuditObject
 *
 * @brief
 * provides the subset data of DBA_AUDIT_OBJECT
 *   or USER_AUDIT_OBJECT if "DBA_..." is not accessivle for user.
 *
 * @details
 * This class plays following role.
 * -# collects the information of the audit trail on database
 *   , and keeps them in this instance as a list container
 *   to generate the SQL statements.
 *   Following items are included in the information:
 *   - Name of object owner.
 *   - Name of object name. 
 *   - Type of object that is represented as integer. 
 *   - Timestamp of audit action.
 *   - Session id and entry-id.
 *   - Action name. (e.g. INSERT, UPDATE, DELETE, INDEX, ... etc.)
 *   - Weather success or fail.
 *   - indicates statement is succeeded (=0) or failed (>0).
 *   - Privilege used to execute.
 *   - Name of os accout of the OCI client.
 *   - Hostname of the OCI client.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cUserAuditObject oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_
       , conf_.as<std::int32_t>("numdays_audit")));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cUserAuditObject
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
     * - second value is passed to tAttributes::szObjectName.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cUserAuditObject::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cUserAuditObject&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cUserAuditObject::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select DBA_AUDIT_OBJECT
    static const char szUserAuditObject[];
public:
    /**
     * @brief
     */
    cUserAuditObject();
    virtual ~cUserAuditObject();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     * @param [in] iNumdays
     *   - is -1 or more and less or equal INT32_MAX.
     *   - Set a positive number of days to go back to check the audit trail.
     *   - When -1 is given, it can suppress to be listed the trail.
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const int32_t& iNumdays
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
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
