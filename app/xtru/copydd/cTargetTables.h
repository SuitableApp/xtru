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

namespace copydd
{

class cTargetTables
{
private:
    ps::lib::cTracer& trc_;
    ps::lib::sql::lite3::cSqliteDb& oDb_;
    struct tAttributes
    {
        char szOwner[OBJECT_NAME_LEN]; // NOT NULL VARCHAR2(120)
        char szTable[OBJECT_NAME_LEN]; // NOT NULL VARCHAR2(120)
        char szIotType[IOT_TYPE_LEN];  // NOT NULL VARCHAR2(12)
        int32_t iDataFmt;
        int32_t iNumLongs;
        int32_t iNumLobs;
        tAttributes(
            const char* szO =0
            , const char* szT =0
            , const char* szI =0
            , int32_t iD =0
            , int32_t iL =0
            , int32_t iB =0
        )
            : iDataFmt(iD)
            , iNumLongs(iL)
            , iNumLobs(iB)
        {
            if (szO) strcpy(szOwner, szO);
            if (szT) strcpy(szTable, szT);
            if (szI) strcpy(szIotType, szI);
        }
        tAttributes(const tAttributes& rhs)
        {
            strcpy(szOwner, rhs.szOwner);
            strcpy(szTable, rhs.szTable);
            strcpy(szIotType, rhs.szIotType);
            iDataFmt = rhs.iDataFmt;
            iNumLongs = rhs.iNumLongs;
            iNumLobs = rhs.iNumLobs;
        }
    };
    const size_t iSkip_;
    ps::lib::cVector<tAttributes> oTables_;
    tAttributes *rTable_;
    ps::lib::sql::ind_t* iInd_;
    int32_t iAllocAndSet(const ps::lib::str_vct& oTables);
    void vInsItems(const ps::lib::str_vct& oListTable);
    void vChgItems(const ps::lib::str_vct& oListFixed);
    void vDelItems(const ps::lib::str_vct& oListExcpt);
    void vDelOptionals();
    void vCountSpecialColumn(); // LONG, LONG RAW, CLOB, BLOB and NCLOB
public:
    static const char szCreStmt[]; ///< Creating newly.
    static const char szDelStmt[]; ///< Deleting all rows.
    cTargetTables(ps::lib::sql::lite3::cSqliteDb& oDb
        , const ps::lib::str_vct& oListTable
        , const ps::lib::str_vct& oListFixed
        , const ps::lib::str_vct& oListExcpt
    );
    ~cTargetTables();
    ps::app::xtru::cTableList oRmWhereNumRows(ps::lib::sql::occi::cSvc& oSvc, const int32_t iNumRows);
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps
