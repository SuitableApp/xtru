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
 * @class cAllSequences
 * @brief
 * provides the subset data of ALL_SEQUENCES.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the sequences.
 *   The information includes the following:
 *   - Name of the schema object.
 *   - Integer of minimum value.
 *   - Integer of maximum value.
 *   - Integer of increment value.
 *   - Flag of cyclic numbering.
 *   - Flag of originate the number in the order requested.
 *   - Amount of numbers to be stored before supply.
 *   - Last number.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    ...
    ps::app::xtru::getmeta::cAllSequences oObj(,...);
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT"), true) ;
   @endcode
 */
class cAllSequences
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
     * @brief
     * passes the key to select one or more matching rows from @ref oList_.
     *
     * @details
     * - first value is passed to tAttributes::szOwner.
     */
    typedef std::tuple<const char*> tKeyTuple;
    /**
     * @brief
     * - To cast nullptr to the type of callback function.
     * - It makes it possible to express the optionality in the case
     *   where it is not necessary to give a callback function.
     * @par Example way to be used.
     * @code
       void f0()
       {
           std::ostringstream os;
           // There is something to operate with @b oList_ element using cbk.
           f1(os, *this, oList_, &<class_identifier>::tAttributes::cbk);
           // Nothing to operate.
           f1(os, *this, oList_, static_cast<tOnRepeatAction>(nullptr));
       }
       @endcode
     */
    typedef void (cAllSequences::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllSequences&
    ) const;
    /**
     * @brief
     * - To cast nullptr to the type of callback function.
     * - It makes it possible to express the optionality in the case
     *   where it is not necessary to give a callback function.
     * @par Example way to be used.
     * @code
       void f0()
       {
           std::ostringstream os;
           // There is something to operate with self using cbk.
           f1(os, *this, &<class_identifier>::cbk);
           // Nothing to operate.
           f1(os, *this, static_cast<tOneTimeAction>(nullptr));
       }
       @endcode
     */
    typedef void (cAllSequences::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /**
     * @brief
     * @code
       @endcode
     */
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /**
     * @brief
     * - A list of struct tAttributes.
     * - stores rows retrieved from the data dictionary view.
     */
    ps::lib::cList<tAttributes> oList_;
    /// number of elements in oList_.
    size_t iNumRows_;
    static const char szAllSequences[];  ///< @brief A SQL-select to select ALL_SEQUENCES
    static const char sz12cSpec[];     ///< @brief Must be added it to adapt for Oracle 12c.
    /// @brief To output the granted privilege for the object.
    cAllTabPrivs& oAllTabPrivs_;
    /// @brief To output the options of audit action.
    cUserObjAuditOpts& oUserObjAuditOpts_;
    /// @brief To output the object audit trail.
    cUserAuditObject& oUserAuditObject_;
public:
    /**
     * @brief
     * @param [in] oAllTabPrivs
     *     To output the granted privilege for the object.
     * @param [in] oUserObjAuditOpts
     *     To output the options of audit action.
     * @param [in] oUserAuditObject
     *     To output the object audit trail.
     */
    cAllSequences(
        cAllTabPrivs& oAllTabPrivs
        , cUserObjAuditOpts& oUserObjAuditOpts
        , cUserAuditObject& oUserAuditObject
    );
    virtual ~cAllSequences();
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
     * @brief
     * @return
     *     size of @ref oList_.
     */
    size_t iGetNumRows(void) const
    {
        return iNumRows_;
    }
    /**
     * @brief
     * -# searches the list for elements matching the conditions given in the argument,
     * -# assembles all or part of the SQL statement and
     * -# outputs it to std::ostream&.
     * @return
     *     A reference of std::ostream& that swallowed the SQL statement.
     * @param [in,out] os
     *     A sink of processed data.
     * @param [in] oKey
     *     A data of condition to select from @ref oList_.
     * @param [in] iDispose
     *     true: Finally remove the element that matches the condition from @ref oList_.
     */
    void vPrintAllItems(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
private:
    /// @brief is a callback function that is indirectly called by vPrintAllItems.
    void vPrintPreRows(std::ostream&, const tAttributes&) const;
    /// @brief is a callback function that is indirectly called by vPrintAllItems.
    void vPrintPostRows(std::ostream&, const tAttributes&) const;
    /// @brief is a callback function that is indirectly called by vPrintAllItems.
    void vPrintNotFound(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

