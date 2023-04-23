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
 * @class cDataDefault
 * @brief
 * <b>UNDER CONSTRUCTION.</b><br />
 * provides the subset data of ALL_TAB_COLUMNS
 *
 * @details
 * This class plays following role.
 * -# collects and stores the comment of tables.
 *   The information includes the following:
 *   - Name of owner.
 *   - Name of schema object.
 *   - Comment of table.
 *
 * @par Example way to be used:
 * @code
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    ps::lib::cVector<std::string> oOwners_ = {"SCOTT"};
    ...
    ps::app::xtru::getmeta::cDataDefault oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::cout << oPrintItems(oObj, std::make_tuple("SCOTT", "EMP"), true) ;
   @endcode
 */
class cDataDefault
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
     * - third value is passed to tAttributes::sColumnName.
     *   (This is referenced at calling not vPrintAllItems but sGetVirtualColumnClause.)
     */
    typedef std::tuple<const char*, const char*, const char*> tKeyTuple;
    /// @copydoc cAllSequences::tOnRepeatAction
    typedef void (cDataDefault::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cDataDefault&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cDataDefault::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    /// @brief A SQL-select to select ALL_TAB_COLUMNS
    static const char szDataDefault[];
public:
    /**
     * @brief
     */
    cDataDefault();
    virtual ~cDataDefault();
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
    /**
     * @brief
     * To retrieve [virtual_column_definition](https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#BABCHBHE).
     * @param [in] oKey
     * -# std::make_tuple(szOwner, szTableName, szColumnName)
     * @return 
     * - virtual_column_definition will be returned, if szColumnName is virtual.
     * - Empty string will be returnd, if szColumnName is real.
     */
    std::string sGetVirtualColumnClause(const tKeyTuple& oKey);
private:
    /// @copybrief cAllSequences::vPrintPreRows()
    void vPrintPreRows(std::ostream& os, const tAttributes&) const;
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRows(std::ostream& os, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
