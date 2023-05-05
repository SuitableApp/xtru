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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cSqlFmt
 * - Add indentation or line feed to make it easy to read for
 *   the SQL sentence given to the constructor argument.
 * - When the output operator is applied to the stream,
 *   the processed result can be taken out.
 * - The definition query of the materialized view
 *   is processed using this object.
 */
class cSqlFmt
{
    friend std::ostream& operator<<(std::ostream& os, cSqlFmt oMe);
private:
    /// Copy of target SQL statement.
    const std::string sStmt_;
    /// Implementation of output operator
    std::ostream& oInvokeCbk(std::ostream& os);
public:
    cSqlFmt(const std::string& sStmt);
};

/**
 * @fn operator<<(std::ostream&, cSqlFmt)
 */
std::ostream& operator<<(std::ostream& os, cSqlFmt oMe);

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
