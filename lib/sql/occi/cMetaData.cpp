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

bool cMetaData::iGetAvailability() const
{
    tVctNameAndType::size_type iNumSUccess = 0;
    std::shared_ptr<oracle::occi::Connection> conn_ (
        oSvc_.oGetCon()
        , [&](oracle::occi::Connection* conn){oSvc_.vRelCon(conn);}
    );
    for (const auto& oPair : oVct_)
    {
        try
        {
            oracle::occi::MetaData metaData = conn_->getMetaData(oPair.first, oPair.second);
            ++iNumSUccess;
        }
        catch (const oracle::occi::SQLException& ex)
        {
            trc_ << boost::format("%s Exception was thrown from Connection::getMetaData() - %s")
                % sClass(ps::lib::W) % ex.getMessage();
            /// e.g. ORA-04043: object "<owner>"."<object>" does not exist
            trc_.flush();
        }
    }
    return iNumSUccess == oVct_.size();
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
