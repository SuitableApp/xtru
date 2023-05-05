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

const ps::lib::cMap<int, const char *> cLocale::oDecoder_
{ {LC_CTYPE, "LC_CTYPE"}
, {LC_NUMERIC, "LC_NUMERIC"}
, {LC_TIME, "LC_TIME"}
, {LC_COLLATE, "LC_COLLATE"}
, {LC_MONETARY, "LC_MONETARY"}
, {LC_MESSAGES, "LC_MESSAGES"}
, {LC_ALL, "LC_ALL"}
};

cLocale::cLocale()
    : oMap_ 
    { { LC_CTYPE, std::setlocale(LC_CTYPE, nullptr) }
    , { LC_NUMERIC, std::setlocale(LC_NUMERIC, nullptr) }
    , { LC_TIME, std::setlocale(LC_TIME, nullptr) }
    , { LC_COLLATE, std::setlocale(LC_COLLATE, nullptr) }
    , { LC_MONETARY, std::setlocale(LC_MONETARY, nullptr) }
    , { LC_MESSAGES, std::setlocale(LC_MESSAGES, nullptr) }
    , { LC_ALL, std::setlocale(LC_ALL, nullptr) }
    }
{}

const char* cLocale::szSetLocale(const int iCategory, const std::string& sLocale)
{
    if (ps::lib::iCompBinary(strcmp, oMap_[iCategory].c_str(), "C"))
    {
        oMap_[iCategory] = std::setlocale(iCategory, sLocale.c_str());
    }
    return oMap_[iCategory].c_str();
}

std::ostream& operator<<(std::ostream& os, const cLocale& rhs)
{
    int i=0;
    boost::for_each(
        rhs.oMap_
        , [&](const ps::lib::cMap<int, std::string>::value_type& val)
        {
            os << boost::format("%s%s=%s")
                % (i++ > 0 ? ", " : "") % rhs.oDecoder_.at(val.first) % val.second;
        }
    );
    return os;
}

} // ps::lib

} // ps

