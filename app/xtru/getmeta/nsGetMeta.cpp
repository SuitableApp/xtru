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
#include "nsGetMeta.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

std::string sColumnIdCooment(const uint32_t& iColumnId)
{
    const static auto& conf = ps::lib::cConfigures::get_const_instance();
    const static auto iPrintcolid = conf.as<bool>("printcolid");
	if (iPrintcolid)
    {
        return (boost::format("/* %04d */ ") % iColumnId).str();
	}
    else
    {
        return std::string("");
	}
}

std::string sGetSubpertitionElement(
    const std::string& sSubpartitioningType
    , const size_t iThreshOnRange
    , const size_t iThreshOnList
    , const size_t iNumSpacesBfr
    , const size_t iNumSpacesAft
    , const std::string& sExpression
){
    std::ostringstream os;
    using ps::lib::iCompBinary;
    const bool iRange = sSubpartitioningType == std::string("RANGE");
    const bool iWidth = iRange
        ? (sExpression.size() > iThreshOnRange)
        : (sExpression.size() > iThreshOnList);
    const std::string sHead = iRange ? "less than " : "";
    os << std::string("values ") << sHead << "(";
    if (iWidth) { os << std::endl << std::string(iNumSpacesBfr, ' '); }
    os << sExpression;
    if (iWidth) { os << std::endl << std::string(iNumSpacesAft, ' '); }
    os << std::string(") ");
    return os.str();
}

std::string sPrintLabel(
    const std::string& sObjectType
    , const ub4& iOrder
    , const std::string& sObjectName
){
    std::ostringstream oss;
    oss << std::string("/*") << std::endl
        << boost::format(" * %s: %06u. %s")
            % sObjectType % iOrder % sObjectName
        << std::endl
        << std::string(" */") << std::endl
        ;
    return oss.str();
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
