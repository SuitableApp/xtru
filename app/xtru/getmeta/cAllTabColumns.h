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
 * @class cAllTabColumns
 *
 * @brief
 * provides the subset data of ALL_TAB_COLUMNS
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
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    ...
    ps::app::xtru::getmeta::cAllTabColumns oObj;
    oQueue_.push_back(oObj.oSubmit(*oSvc_, oOwners_, conf_.as<int32_t>("cmntlvl")));
    ps::lib::vSynchronize(iConcurrency_, oQueue_);
    std::ofstream ofs("meta.sql");
    oObj.vPrintAllTabColumns(ofs, std::make_tuple("SCOTT", "EMP"), true);
   @endcode
 */
class cAllTabColumns
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
    typedef void (cAllTabColumns::tAttributes::*tOnRepeatAction)(
        std::ostream&
        , const size_t&
        , cAllTabColumns&
    ) const;
    /// @copydoc cAllSequences::tOneTimeAction
    typedef void (cAllTabColumns::*tOneTimeAction)(
        std::ostream&
        , const tAttributes&
    ) const;
    /// @copydoc cAllSequences::tIter
    typedef typename ps::lib::cList<tAttributes>::iterator tIter;
    /// @copydoc cAllSequences::oList_
    ps::lib::cList<tAttributes> oList_;
    bool iCloneable_;                              ///< It is used to check cloneable.
    /// @brief A SQL-select to select ALL_TAB_COLUMNS
    static const char szAllTabColumns[];
    static const char szUsualCharLength[];        ///< less then Oracle 9i
    static const char szNewFeauturedCharLength[]; ///< more then Oracle 9i
    static const char szVirtualColumnDisable[];   ///< less than Oracle 10g R1
    static const char szVirtualColumnEnable[];    ///< more than Oracle 10g R1
    /**
     * @brief
     *   - Object definition order is based on object mutual dependency.
     */
    const bool iDependent_;
    /**
     * @brief
     *   - is 0,1 or 2. Regulates the policy of migrating values of LONG or LONG RAW type.
     */
    const int32_t iLongTransit_;
    cDataDefault& oDataDefault_; ///< Used to search the virtual column definition.
public:
    /**
     * @brief
     * @param [in] iDependent
     *   - Object definition order is based on object mutual dependency.
     * @param [in] iLongTransit
     *   - is 0,1 or 2. Regulates the policy of migrating values of LONG or LONG RAW type.
     *   - 0: LONG type / LONG RAW type is output to CSV / fixed length data file. (No conversion)
     *   - 1: Create an OS file that can be loaded as CLOB type / BLOB type.
     *   - 2: Create an OS file that can be loaded as BFILE type.
     * @param [in,out] oDataDefault
     */
    cAllTabColumns(
        const bool& iDependent
        , const int32_t& iLongTransit
        , cDataDefault& oDataDefault
    );
    /**
     * @brief
     * This can be copied self.
     *
     * @param [in] rhs
     */
    cAllTabColumns(
        const cAllTabColumns& rhs
    );
    /**
     * @brief
     * Prohibition of assignment.
     */
    cAllTabColumns& operator=( const cAllTabColumns&) =delete;
    /**
     * @brief
     */
    virtual ~cAllTabColumns();
    /**
     * @brief
     * @param [in,out] oSvc
     *     is a pool of sessions to connect to Oracle Database.
     * @param [in] oOwners
     *     is concrete name of owners.
     * @param [in,out] oKeyColumns
     */
    ps::lib::sql::cFetchable* oSubmit(
        ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , cKeyColumns& oKeyColumns
    );
    /**
     * @brief
     * -# searches the list for elements matching the conditions given in the argument,
     * -# assembles all or part of the SQL statement and
     * -# outputs it to std::ostream&.
     * @param [in,out] os
     *     A sink of processed data.
     * @param [in] oKey
     *     A data of condition to select from @ref oList_.
     * @param [in] iDispose
     *     true: Finally remove the element that matches the condition from @ref oList_.
     * @return
     *     A reference of std::ostream& that swallowed the SQL statement.
     */
    void vPrintAllTabColumns(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
    /**
     * @brief
     * -# searches the list for elements matching the conditions given in the argument,
     * -# assembles all or part of the SQL statement and
     * -# outputs it to std::ostream&.
     * @param [in,out] os
     *     A sink of processed data.
     * @param [in] oKey
     *     A data of condition to select from @ref oList_.
     * @param [in] iDispose
     *     true: Finally remove the element that matches the condition from @ref oList_.
     * @return
     *     A reference of std::ostream& that swallowed the SQL statement.
     */
    void vPrintAllViewColumns(
        std::ostream& os
        , const tKeyTuple& oKey
        , const bool& iDispose
    );
    /**
     * @brief
     *
     * @param [in] szOwner
     * @param [in] szTableName
     * @param [in] szColumnName
     * @return
     * - A column id will be returnd, if column name is found.
     * - 9999 will be returnd, if not found. 
     */
    uint32_t iGetColumnId(
        const char* szOwner
        , const char* szTableName
        , const char* szColumnName
    ) const;
    /**
     * @brief
     *
     * @param [in] szOwner
     * @param [in] szTableName
     * @param [in] iColumnId
     * @return
     * - A column name will be returnd, if column id is found.
     * - A pointer to empty string will be returnd, if not found. 
     */
    const char* szGetColumnName(
        const char* szOwner
        , const char* szTableName
        , const uint32_t iColumnId
    ) const;
private:
    /// @copybrief cAllSequences::vPrintPostRows()
    void vPrintPostRowsView(std::ostream&, const tAttributes&) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
