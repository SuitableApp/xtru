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
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

cFeature * cFeature::oMakeInstance(const std::string& feature)
{
    BOOST_ASSERT(feature.size());
    if      (feature == "unload")  { return new ps::app::xtru::getdata::cUnload;  }
    else if (feature == "query")   { return new ps::app::xtru::getdata::cQuery;   }
    else if (feature == "getmeta") { return new ps::app::xtru::getmeta::cGetMeta; }
    else 
    {
        std::ostringstream oss;
        oss << "Specified '" << feature << "' has not been featured.";
        RAISE_EX_CONVERT(std::runtime_error, oss.str());
    }
    return nullptr;
}

cFeature::~cFeature()
{}

} // ps::app::xtru

} // ps::app

} // ps
