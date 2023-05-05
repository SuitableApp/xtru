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
/**
 * @details
 */
cStat::cStat()
    : iOutputBytes_(0)
    , time_at_started_(boost::posix_time::microsec_clock::local_time())
{}
/**
 * @details
 */
void cStat::vAddOutputBytes(const int64_t& iOutputBytes)
{
    iOutputBytes_ += iOutputBytes;
}
/**
 * @details
 */
int64_t cStat::iGetOutputBytes() const
{
    return iOutputBytes_;
}
/**
 * @details
 */
int32_t cStat::iDurationMilliSeconds() const
{
    const boost::posix_time::time_period period(
        time_at_started_
        , boost::posix_time::microsec_clock::local_time()
    );
    const boost::posix_time::time_duration duration = period.length();
    return duration.total_milliseconds();
}

/**
 * @details
 */
std::string cStat::sGetStartDateTime() const
{

    return boost::posix_time::to_simple_string(time_at_started_);
}
} // ps::lib

} // ps

