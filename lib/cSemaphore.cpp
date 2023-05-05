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

cSemaphore::cSemaphore(const uint32_t& count)
    : count_(count)
{}
void cSemaphore::vNotify()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ++count_;
    condition_.notify_one();
}
void cSemaphore::vWait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] {return count_;});
    --count_;
}

} // ps::lib

} // ps

