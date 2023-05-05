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

#include <pslib.h>

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cOciErr::cOciErr()
    : iOciRtn(oracle::occi::OCCI_SUCCESS)
    , env_(oracle::occi::Environment::createEnvironment(
        // oracle::occi::Environment::Mode(oracle::occi::Environment::THREADED_MUTEXED)
    ))
    , envhp(0)
    , errhp(0)
{
    envhp = env_->getOCIEnvironment();
    iOciRtn = OCIHandleAlloc(envhp, (void**) &errhp, OCI_HTYPE_ERROR, 0, NULL);
    BOOST_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS);
}

cOciErr::~cOciErr()
{
    if (errhp)
    {
        iOciRtn = OCIHandleFree((void*) errhp, OCI_HTYPE_ERROR);
        BOOST_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS);
    }
}

OCIError *cOciErr::oGetErrhp() const
{
    BOOST_ASSERT(errhp);
    return errhp;
}

OCIEnv *cOciErr::oGetEnvhp() const
{
    BOOST_ASSERT(envhp);
    return envhp;
}

std::string cOciErr::sPrtSqlErrm(sb4& iSqlCode) const
{
    BOOST_ASSERT(errhp);
    std::string sRtn;
    sb4 lErrCode;
    ub4 lRecNo = 1;
    char szErrBuf[1024];
    while (OCIErrorGet(
        (dvoid *) errhp, (ub4) lRecNo, (text *) NULL, &lErrCode
        , (text *) szErrBuf, (ub4) sizeof(szErrBuf), OCI_HTYPE_ERROR
    ) != OCI_NO_DATA)
    {
        if (1L == lRecNo) iSqlCode = lErrCode;
        sRtn += std::string(szErrBuf);
        lRecNo++;
    }
    sRtn.erase(sRtn.find_last_of('\n'));
    return sRtn;
}

std::string cOciErr::sGetErrm(const sword& iOciRtn, sb4& iSqlCode) const
{
    std::string sRtn;
    switch (iOciRtn)
    {
    case OCI_SUCCESS: /* equals 0. */
        break;
    case OCI_NEED_DATA: /* equals 99. */
        sRtn += "\nOCI_NEED_DATA\n";
        break;
    case OCI_SUCCESS_WITH_INFO: /* equals 1. */
        sRtn += "\nOCI_SUCCESS_WITH_INFO\n";
        sRtn += sPrtSqlErrm(iSqlCode);
        break;
    case OCI_NO_DATA: /* equals 100. */
        sRtn += "\nOCI_NO_DATA\n";
        break;
    case OCI_ERROR: /* equals -1. */
        sRtn += sPrtSqlErrm(iSqlCode);
        break;
    case OCI_INVALID_HANDLE: /* equals -2. */
        sRtn += "\nOCI_INVALID_HANDLE\n";
        break;
    case OCI_STILL_EXECUTING: /* equals -3123. */
        sRtn += "\nOCI_STILL_EXECUTE\n";
        break;
    case OCI_CONTINUE: /* equals -24200. */
        sRtn += "\nOCI_CONTINUE\n";
        break;
    default:
        sRtn += "\nUnknown error\n";
        break;
    }
    return sRtn;
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

