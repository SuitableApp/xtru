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

cRtn::cRtn()
    : rtn_(EXIT_SUCCESS)
    , cont_(true)
{}

cRtn& cRtn::operator()(const int32_t& val)
{
    std::lock_guard<std::mutex> lk(mtx_);
    rtn_ = val;
    return *this;
}

void cRtn::vOrValue(const int32_t rc)
{
    std::lock_guard<std::mutex> lk(mtx_);
    rtn_ |= rc;
    if (rtn_) cont_ = false;
}

cRtn::operator int32_t() const
{
    return rtn_;
}

void cRtn::vBreak()
{
    std::lock_guard<std::mutex> lk(mtx_);
    cont_ = false;
    for (auto pid : oPidSet)
    {
        ::kill(pid, SIGTERM);
    }
}


void cRtn::vAddPid(int32_t pid)
{
    std::lock_guard<std::mutex> lk(mtx_);
    oPidSet.insert(pid);
}

void cRtn::vDelPid(int32_t pid)
{
    std::lock_guard<std::mutex> lk(mtx_);
    oPidSet.erase(pid);
}

bool cRtn::iCotinue() const
{
    return cont_;
}

} // ps::lib

} // ps
