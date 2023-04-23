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

/**
 * RAW memory allocate pool
 */
class cAllocator
{
private:
    ps::lib::cTracer& trc_;
    ps::lib::cSet<uint8_t*> m_;
    std::mutex mtx_;
    uint64_t iMaloc, iRaloc, iMfree;
public:
    cAllocator();
    ~cAllocator();
    void* maloc(size_t size);
    void* raloc(void* memptr, size_t newsize);
    void mfree(void* memptr);
};

/**
 * Helper for a handle of cAllocate instance
 */

void *maloc(void* ctxp, size_t size);
void *raloc(void* ctxp, void* memptr, size_t size);
void mfree(void* ctxp, void* memptr);

} // ps::lib::sql::occi

} // ps::lib::sql

} // ps::lib

} // ps

