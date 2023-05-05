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
 * @class cUserObjAuditOpts
 *
 * @brief
 * provides the subset data of DBA_OBJ_AUDIT_OPTS
 *   or USER_OBJ_AUDIT_OPTS if "DBA_..." is not accessivle for user.
 *
 * @details
 * This class plays following role.
 * -# collects the information of the privileges exercisable
 *   , and keeps them in this instance as a list container
 *   to generate the SQL statements.
 *   Following items are included in the information:
 *   - Name of object owner.
 *   - Name of object name. 
 *   - 15 types of audit actions is stored about whether audit trails
 *      are recorded for each access or for each session.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cUserObjAuditOpts oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cUserObjAuditOpts
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
    typedef void (cUserObjAuditOpts::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cUserObjAuditOpts&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cUserObjAuditOpts::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select DBA_OBJ_AUDIT_OPTS
    static const char szDbaObjAuditOpts[];
    /// @brief A SQL-select to select ALL_OBJ_AUDIT_OPTS
    static const char szUserObjAuditOpts[];
    /// Must be added it to adapt for Oracle 10g1 or later.
    static const char sz10g1Spec1[];
    /// Must be added it to adapt for Oracle 10g1 or later.
    static const char sz10g1Spec2[];
public:
    /**
     * @brief
     */
    cUserObjAuditOpts();
    virtual ~cUserObjAuditOpts();
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
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

