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

class cEffectiveCons
{
private:
    typedef struct tAllConstraints tAttributes;
    static const uint32_t iBulkSize_;
    static const std::string tag_;
    static const size_t iSkip_;
    ps::lib::sql::lite3::cSqliteDb& oDb_;
    std::list<tAttributes> oContainer_;
    cEffectiveCons& operator=(const cEffectiveCons& rhs) =delete;
public:
    static const std::string Z;
    static const std::string A;
    static const std::string B;
    static const char szCreStmt[]; ///< Creating newly.
    static const char szIdx1Stmt[]; ///< Indexing newly.
    static const char szIdx2Stmt[]; ///< Indexing newly.
    cEffectiveCons(ps::lib::sql::lite3::cSqliteDb& oDb, const std::string& szStmt = Z);
    cEffectiveCons(const cEffectiveCons& rhs);
    ~cEffectiveCons();
    cEffectiveCons& operator-=(const cEffectiveCons& rhs);
    void vSqlDel() const;
    void vSqlIns() const;
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

