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

namespace lib
{

namespace sql
{

namespace lite3
{

extern std::unique_ptr<sqlite3_api_routines> sqlite3_api;

class cSqliteDb
{
private:
    cSqliteDb(const cSqliteDb&) =delete;
    cSqliteDb& operator=(const cSqliteDb&) =delete;
    const ps::lib::cConfigures& conf_;
    ps::lib::cDistributor& mos_;
    sqlite3* db_;
    std::string sVersion_;
    char *errmsg_;
    const std::string fn_;
public:
    cSqliteDb();
    ~cSqliteDb();
    std::string sShowVersion() const;
    int32_t iExecSql(
        const char* sql
        , int32_t (*cbk_fn)(void*,int32_t,char**,char**)
        , void* ctx
    );
    int32_t iExecSql(const char* sql);
    int32_t iExecSql(const ps::lib::str_vct& arr);
    sqlite3* oGetHndl() const;
    std::string sGetFn() const;
};

class cCheckErr
{
private:
    typedef ps::lib::cMap<int32_t, std::string> tErrSymMap;
    static tErrSymMap oErrMap;  // Result Codes and it symbols.
    const cSqliteDb& oSqliteDb_;
public:
    cCheckErr(const cSqliteDb& oSqliteDb)
        : oSqliteDb_(oSqliteDb)
    {}
    const std::string operator()() const ;
};

class cTransactional
{
private:
    cSqliteDb& oDb_;
public:
    cTransactional(cSqliteDb& oDb)
        : oDb_(oDb)
    {
        ASSERT_OR_RAISE_FNC(
            ! oDb_.iExecSql("BEGIN"), std::runtime_error, cCheckErr(oDb_)
        );
    }
    ~cTransactional()
    {
        ASSERT_OR_RAISE_FNC(
            ! oDb_.iExecSql("COMMIT"), std::runtime_error,cCheckErr(oDb_)
        );
    }
};

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

