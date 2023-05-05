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

namespace lib
{

namespace sql
{

namespace lite3
{

class cSqliteStmt
{
private:
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    cSqliteDb* oDb_;
    /**
     * @struct deleter 
     * A functor to release the data pointed to by sqlite3_stmt *.
     * Operator()() is called when cSqliteStmt::ite3_stmt_ is dismantled.
     */
    struct deleter
    {
        void operator()(sqlite3_stmt *to_delete) { sqlite3_finalize(to_delete); };
    };
    /**
     * Prepared resource is deallocated surely while ite3_stmt_ is destructing. Because,
     * deleter::operator()() is called by DTOR of std::unique_ptr<sqlite3_stmt, deleter>.
     */
    std::unique_ptr<sqlite3_stmt, deleter> ite3_stmt_;
    /**
     * @brief
     * does not declares as const. Because it allowes to modify
     * by vConvPlaceHolder().
     */
    std::string sql_;
    /**
     * Number of placeholders like '?', in the SQL.
     */
    size_t iNumBindParam_;
    /**
     * Number of columns in a SELECT statement.
     * It is always 0 if the statement os not SELECT.
     */
    size_t iNumColumns_;
    /**
     * An object for supplying the value of one or more
     * host variables to the place folder in a SQL.
     */
    cBind oBind_;
    /**
     * An object that associates to obtain data
     * returned by SELECT via host variables.
     */
    cDefine oDefine_;
    const char *szTail_;
    cSqliteStmt& operator=(cSqliteStmt& rhs);
public:
    /**
     * @brief
     * @param [in,out] oDb
     * @param [in] sql
     * - It accepts all input of valid SQL in Sqlite3.
     * - It is possible to include expressions of valid place folders
     *   (eg question mark) in Sqlite3.
     * - As a unique function of cSqliteStmt, it is possible to include
     *   one or more place folders with the expression '%%s'.
     * - Calling vConvPlaceHolder() completes the embedding of the value
     *   into '%%s', but it must do it before calling iParse().
     */
    cSqliteStmt(
        cSqliteDb& oDb
        , const std::string& sql
    );
    cSqliteStmt(cSqliteStmt& rhs);
    ~cSqliteStmt();
    /**
     * @brief
     */
    sqlite3_stmt* oGetHndl();
    /**
     * @brief
     */
    cSqliteDb& oGetDb();
    /**
     * @brief
     */
    cBind& oGetBind();
    /**
     * @brief
     */
    cDefine& oGetDefine();
    /**
     * @brief
     */
    int32_t iParse(void);
    /**
     * @brief
     * - Place-holders (=%%s) in SQL-statement will be replaced
     *   by given "opts" which is arraied string.
     * - It must be called before iParse().
     *
     * @param[in] opts
     *   stores values for %s place-holder in cSqliteStmt::sql_.
     */
    void vConvPlaceHolder(
        const ps::lib::str_vct& opts
    );
    int32_t iExecute(
        const size_t& iNumIter
    );
    int32_t iFetch(
        ps::lib::sql::cFetchable& fetchable
    );
    int32_t iGetNumRows() const;
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

