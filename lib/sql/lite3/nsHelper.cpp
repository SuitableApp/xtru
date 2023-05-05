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
#include <sqlite3ext.h>

namespace ps 
{

namespace lib
{

namespace sql
{

namespace lite3
{

namespace nsHelper
{

bool iDoesTableNotExist(
    const char* szTableName
    , ps::lib::sql::lite3::cSqliteDb& oDb
){
    int32_t iCount = 0;
    ps::lib::sql::ind_t iTableNameInd = ps::lib::sql::ind_t::VAL_IS_NOTNULL;
    ps::lib::sql::ind_t iCountInd = ps::lib::sql::ind_t::VAL_IS_NOTNULL;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb,
        "SELECT COUNT(*) FROM SQLITE_MASTER "
        "WHERE TYPE = 'table' AND NAME = ? "
    );
    ASSERT_OR_RAISE_FNC(
        oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb)
    );
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(iCount, cAttr::tLite3Type::INT32, &iCountInd, 0, 0);
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(szTableName, cAttr::tLite3Type::STR, &iTableNameInd, 0, 0);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&]{} // Nothing to do.
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
            , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb));
    return 0 == iCount;
}

} // ps::lib::sql::lite3::nsHelper

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

