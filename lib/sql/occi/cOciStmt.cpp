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

cOciStmt::cOciStmt()
    : stmt_(0)
{}

void cOciStmt::vAddHndl(oracle::occi::Statement *stmt)
{
    stmt_ = stmt;
}

ps::lib::sql::occi::cOciErr& cOciStmt::oGetOciErr() 
{
    return oOciErr_;
}

OCIStmt * cOciStmt::oGetOciStmt() const
{
    BOOST_ASSERT(stmt_);
    return stmt_->getOCIStatement();
}

OCISvcCtx * cOciStmt::oGetOciSvcCtx() const
{
    BOOST_ASSERT(stmt_);
    oracle::occi::Connection *conn = stmt_->getConnection();
    return conn->getOCIServiceContext();
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps


