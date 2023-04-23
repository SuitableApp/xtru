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

namespace lite3
{

cDefine::cDefine(cSqliteStmt& oStmt)
    : oStmt_(oStmt)
{}

cDefine::cDefine(cDefine& rhs)
    : oStmt_(rhs.oStmt_)
{
    rhs.oArr_.swap(oArr_);
}

void cDefine::vAcquire(const size_t& i)
{
    for (auto& oAttr: oArr_)
    {
        oAttr.vPick(i);
    }
}

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

