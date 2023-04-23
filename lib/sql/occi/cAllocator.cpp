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

namespace ps
{

namespace lib
{

namespace sql
{

namespace occi
{

cAllocator::cAllocator()
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , iMaloc(0), iRaloc(0), iMfree(0)
{}
cAllocator::~cAllocator()
{
    trc_ << boost::format("@0x%p:maloc=%d, raloc=%d, mfree=%d")
        % this % iMaloc % iRaloc % iMfree  << std::endl;
}


void* cAllocator::maloc(size_t size)
{
    std::lock_guard<std::mutex> lk(mtx_);
    ++iMaloc;
    auto len = size ? size : 1;
    uint8_t* ptr = new uint8_t[len];
    ::memset(ptr, 0, len);
    m_.insert(ptr);
    return ptr;
}

void* cAllocator::raloc(void* memptr, size_t newsize)
{
    std::lock_guard<std::mutex> lk(mtx_);
    ++iRaloc;
    auto it = m_.find(static_cast<uint8_t*>(memptr));
    delete [] *it;
    m_.erase(it);
    auto len = newsize ? newsize : 1;
    uint8_t* ptr = new uint8_t[len];
    ::memset(ptr, 0, len);
    m_.insert(ptr);
    return ptr;
}

void cAllocator::mfree(void* memptr)
{
    std::lock_guard<std::mutex> lk(mtx_);
    ++iMfree;
    auto it = m_.find(static_cast<uint8_t*>(memptr));
    delete [] *it;
    m_.erase(it);
}

void *maloc(void* ctxp, size_t size)
{
    return static_cast<ps::lib::sql::occi::cAllocator*>(ctxp)->maloc(size);
}

void *raloc(void* ctxp, void* memptr, size_t size)
{
    return static_cast<ps::lib::sql::occi::cAllocator*>(ctxp)->raloc(memptr, size);
}

void mfree(void* ctxp, void* memptr)
{
    return static_cast<ps::lib::sql::occi::cAllocator*>(ctxp)->mfree(memptr);
}

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps
