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
#ifdef __sun
#endif
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

cStartValues::cStartValues()
    : dependent_(conf_.as<bool>("dependent"))
    , compress_(conf_.as<bool>("compress"))
    , metadiv_(conf_.as<bool>("metadiv"))
    , cmntlvl_(conf_.as<int32_t>("cmntlvl"))
    , numdays_audit_(conf_.as<int32_t>("numdays_audit"))
    , longtransit_(conf_.as<int32_t>("longtransit"))
    , date_mask_(conf_.as<std::string>("date_mask"))
    , timestamp_mask_(conf_.as<std::string>("timestamp_mask"))
    , timestamp_tz_mask_(conf_.as<std::string>("timestamp_tz_mask"))
{
}

cStartValues::~cStartValues()
{}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
