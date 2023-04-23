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

class cRtn
    : public boost::serialization::singleton< cRtn >
{
    friend class boost::serialization::singleton< cRtn >;
private:
    std::mutex mtx_;
    ps::lib::cSet<int32_t> oPidSet;
    int32_t rtn_;
    bool cont_;
    cRtn(); // will call by ctor of "singleton_wrapper<cRtn>".
    ~cRtn() {}
public:
    void vOrValue(const int32_t rc);
    cRtn& operator()(const int32_t& val);
    operator int32_t() const;
    void vBreak();
    void vAddPid(int32_t pid);
    void vDelPid(int32_t pid);
    /**
     * @return 
     *   true indicates that it may continue.
     */
    bool iCotinue() const;
};

} // ps::lib

} // ps

