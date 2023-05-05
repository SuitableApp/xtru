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

namespace occi
{

class cOciErr;
class cOciStmt;

struct cEnvDeleter
{
    void operator()(oracle::occi::Environment* env) const
    {
        BOOST_ASSERT(env);
        oracle::occi::Environment::terminateEnvironment(env);
    }
};

typedef std::unique_ptr<oracle::occi::Environment, cEnvDeleter> tOcciEnv;

class tSmartStmt
{
private:
    oracle::occi::Connection *conn_;
    oracle::occi::Statement *stmt_;
public:
    tSmartStmt(oracle::occi::Connection *conn)
        : conn_(conn)
        , stmt_(conn_->createStatement())
    {}
    ~tSmartStmt()
    {
        conn_->terminateStatement(stmt_);
    }
public:
    oracle::occi::Statement * operator->()
    {
        return stmt_;
    }
    const oracle::occi::Statement * operator->() const
    {
        return stmt_;
    }
};

    extern
void vDescriptorAlloc(
    cOciErr& oOciErr
    , dvoid **descpp
    , ub4 type
);

    extern
void vDescriptorFree(
    cOciErr& oOciErr
    , dvoid *descp
    , ub4 type
);

    extern
void vLobFileGetName(
    cOciStmt& oOciStmt
    , OCILobLocator *rLobs
    , char szAlias[]
    , ub2 *nALength
    , char szFName[]
    , ub2 *nFLength
    , boolean *bFlag
);

    extern
void vNumberToText(
    cOciErr& oOciErr
    , const OCINumber *val
    , const std::string& sNumFmt
    , char *szBuffer, ub4& iBuffer
);

    extern
void vDefineArrayOfStruct(
    cOciStmt& oOciStmt
    , int32_t pos
    , void *valuep
    , int32_t value_sz
    , uint16_t dty
    , ps::lib::sql::ind_t *indp
    , uint16_t *rlenp
    , uint16_t *rcodep
    , uint32_t pvskip
    , uint32_t indskip
    , uint32_t rlskip
    , uint32_t rcskip
    , uint32_t mode
    , OCICallbackDefine func
    , void* ctxp
    , uint8_t csfrm
    , uint16_t csid
    , uint16_t pdprc
    , int16_t pdscl
);

    extern
ub4 getNumArrayRows(cOciStmt& oOciStmt);

    extern
sword iStmtFetch2(
    cOciStmt& oOciStmt
    , ub4 nrows
    , const std::string& sSql
);

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps



