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
 * @class cAllTabPrivs
 * @brief
 * provides the subset data of DBA_TAB_PRIVS
 *   or ALL_TAB_PRIVS if "DBA_..." is not accessivle for user.
 *
 * @details
 * This class plays following role.
 * -# collects and stores the information of the privileges exercisable.
 *   The information includes the following:
 *   - A representation of the object type as an integer.<br />
 *      For example, it is expressed that SEQUENCE is 2, TABLE is 4, VIEW is 6, and so on.
 *   - Name of schema object. 
 *   - Name of authorized authority. 
 *   - Authorized user names.
 *   - Whether an authorized user can grant the same rights to other users.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    ...
    ps::app::xtru::getmeta::cAllTabPrivs oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cAllTabPrivs
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
     * - second value is passed to tAttributes::szTableName.
     */
    typedef std::tuple<const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cAllTabPrivs::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllTabPrivs&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllTabPrivs::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select DBA_TAB_PRIVS
    static const char szDbaTabPrivs[];
    /// @brief A SQL-select to select ALL_TAB_PRIVS
    static const char szAllTabPrivs[];
    /// @brief Must be added it to adapt for Oracle 12c.
    static const char sz12cSpec[];
public:
    /**
     * @brief
     */
    cAllTabPrivs();
    virtual ~cAllTabPrivs();
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
