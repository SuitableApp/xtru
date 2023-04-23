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

class cTargetOwners
{
private:
    ps::lib::sql::lite3::cSqliteDb& oDb_;
    struct tAttributes
    {
        char szOwner[OBJECT_NAME_LEN]; // NOT NULL VARCHAR2(30)
    };
    const size_t iNumIter_;
    const size_t iSkip_;
    ps::lib::str_vct oOwners_;
    tAttributes *rTable_;
    ps::lib::sql::ind_t* iInd_;
    void vInitTable(ps::lib::sql::lite3::cSqliteDb& oDb);
public:
    cTargetOwners(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , const ps::lib::str_vct& oOwners
    );
    ~cTargetOwners();
    void vRun();
    ps::lib::str_vct oGetOwners();
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

