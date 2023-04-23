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

namespace lib
{

namespace sql
{

namespace occi
{

class cMetaData
{
public:
    typedef std::pair<std::string, oracle::occi::MetaData::ParamType> tNameAndType;
    typedef ps::lib::cVector<tNameAndType> tVctNameAndType;
private:
    ps::lib::sql::occi::cSvc& oSvc_;
    ps::lib::cTracer& trc_;
    tVctNameAndType oVct_;
public:
    cMetaData(ps::lib::sql::occi::cSvc& oSvc)
        : oSvc_(oSvc)
        , trc_(ps::lib::cTracer::get_mutable_instance())
    {}
    virtual ~cMetaData()
    {}
    void vAddNamesAndTypes(tVctNameAndType&& oVct)
    {
        oVct_ = oVct;
    }
    bool iGetAvailability() const;
};

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
