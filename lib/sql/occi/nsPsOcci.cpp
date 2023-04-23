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

#include <pslib.h>


namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

void vDescriptorAlloc(
    cOciErr& oOciErr
    , dvoid **descpp
    , ub4 type
){
    sword iOciRtn = OCIDescriptorAlloc(
        (CONST dvoid *) oOciErr.oGetEnvhp(), descpp, type
        , 0 /*xtramem_sz*/, NULL /*usrmempp*/
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
}

void vDescriptorFree(
    cOciErr& oOciErr
    , dvoid *descp
    , ub4 type
){
    if (descp)
    {
        sword iOciRtn = OCIDescriptorFree(descp, type);
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
}

void vLobFileGetName(
    cOciStmt& oOciStmt
    , OCILobLocator *rLobs
    , char szAlias[]
    , ub2 *nALength
    , char szFName[]
    , ub2 *nFLength
    , boolean *bFlag
){
    cOciErr& oOciErr = oOciStmt.oGetOciErr();
    sword iOciRtn = OCILobFileExists(
        oOciStmt.oGetOciSvcCtx(), oOciErr.oGetErrhp(), rLobs, bFlag
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    if (*bFlag)
    {
        iOciRtn = OCILobFileGetName(oOciErr.oGetEnvhp(), oOciErr.oGetErrhp()
            , rLobs, (text *) szAlias, nALength, (text *) szFName, nFLength
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
    else
    { 
        *nALength = 0; szAlias[*nALength] = '\0';
        *nFLength = 0; szFName[*nFLength] = '\0';
    }
}

void vNumberToText(
    cOciErr& oOciErr
    , const OCINumber *val
    , const std::string& sNumFmt
    , char *szBuffer, ub4& iBuffer
){
    sword iOciRtn = OCINumberToText(
        oOciErr.oGetErrhp(), val
        , (oratext *) sNumFmt.c_str(), sNumFmt.size()
        , NULL, 0, &iBuffer, (oratext *) szBuffer
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
}

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
){
    cOciErr& oOciErr = oOciStmt.oGetOciErr();

    OCIDefine *defnp = 0;

    sword iOciRtn = OCIDefineByPos(
        oOciStmt.oGetOciStmt(), &defnp, oOciErr.oGetErrhp()
        , pos 
        , valuep
        , value_sz
        , dty
        , indp
        , rlenp 
        , rcodep 
        , mode
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);

    if (mode == OCI_DYNAMIC_FETCH)
    {
        // for LONG and LONG RAW data types.
        // piecewise fetch with user defined callback function.
        iOciRtn = OCIDefineDynamic(
            defnp, oOciErr.oGetErrhp(), ctxp, func
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }

    iOciRtn = OCIDefineArrayOfStruct(
        (OCIDefine*) defnp, oOciErr.oGetErrhp()
        , pvskip
        , indskip
        , rlskip
        , rcskip
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);

    ub4 iLen;
    /* Setting Charset form. */
    if (csfrm)
    {
        iLen = sizeof(ub1);
        iOciRtn = OCIAttrSet(
            defnp, OCI_HTYPE_DEFINE, &csfrm, iLen, OCI_ATTR_CHARSET_FORM, oOciErr.oGetErrhp()
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
    /* Setting Charset-ID. */
    if (csid)
    {
        iLen = sizeof(ub2);
        iOciRtn = OCIAttrSet(
            defnp, OCI_HTYPE_DEFINE, &csid, iLen, OCI_ATTR_CHARSET_ID, oOciErr.oGetErrhp()
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
    /* BCD number precision. */
    if (pdprc)
    {
        iLen = sizeof(ub2);
        iOciRtn = OCIAttrSet(
            defnp, OCI_HTYPE_DEFINE, &pdprc, iLen, OCI_ATTR_PDPRC, oOciErr.oGetErrhp()
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
    /* BCD number scaling. */
    if (pdscl)
    {
        iLen = sizeof(sb2);
        iOciRtn = OCIAttrSet(
            defnp, OCI_HTYPE_DEFINE, &pdscl, iLen, OCI_ATTR_PDSCL, oOciErr.oGetErrhp()
        );
        PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    }
}

ub4 getNumArrayRows(cOciStmt& oOciStmt)
{
    cOciErr& oOciErr = oOciStmt.oGetOciErr();
    ub4 iRtn = 0;
    ub4 iLen = sizeof(iRtn);

    sword iOciRtn = OCIAttrGet (
        oOciStmt.oGetOciStmt()
        , OCI_HTYPE_STMT
        , (void*) &iRtn
        , (ub4*) &iLen
        , OCI_ATTR_ROWS_FETCHED
        , oOciErr.oGetErrhp()
    );
    PS_OCI_ASSERT(iOciRtn == oracle::occi::OCCI_SUCCESS, oOciErr, iOciRtn);
    return iRtn;
}

sword iStmtFetch2(
    cOciStmt& oOciStmt
    , ub4 nrows
    , const std::string& sSql
){
    cOciErr& oOciErr = oOciStmt.oGetOciErr();

    sword iOciRtn = OCIStmtFetch2(
        oOciStmt.oGetOciStmt(), oOciErr.oGetErrhp()
        , nrows, OCI_FETCH_NEXT, 0, OCI_DEFAULT
    );
    if (iOciRtn && iOciRtn != OCI_NO_DATA)
    {
        sb4 iSqlCode = 0;
        const auto sMsg = oOciErr.sGetErrm(iOciRtn, iSqlCode);
        ASSERT_OR_RAISE(1013 == iSqlCode, std::runtime_error
            , boost::format("%s: %s") % sMsg % sSql
        );
        ps::lib::cRtn& rtn_ = ps::lib::cRtn::get_mutable_instance();
        rtn_.vBreak();
        iOciRtn = OCI_NO_DATA;
    }
    return iOciRtn;
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

