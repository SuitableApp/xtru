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

#pragma once

#ifdef __sun
#define PLATFORM_LC_TYPE_STR "en_US.UTF-8"
#else
#define PLATFORM_LC_TYPE_STR "en_US.utf8"
#endif

namespace ps
{

namespace lib
{

class cLocale
    : public boost::serialization::singleton< cLocale >
{
    friend class boost::serialization::singleton< cLocale >;
private:
    static const ps::lib::cMap<int, const char *> oDecoder_;
    cLocale(); // will call by ctor of "singleton_wrapper<cLocale>".
    ~cLocale() {}
    ps::lib::cMap<int, std::string> oMap_;
public:
    const char* szSetLocale(const int iCategory, const std::string& sLocale);
    friend std::ostream& operator<<(std::ostream& os, const cLocale& rhs);
};

} // ps::lib

} // ps

