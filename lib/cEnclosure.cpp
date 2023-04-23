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

/**
 * @details
 */
cEnclosure cEnclosure::oSqlInList = cEnclosure({{"(",")"}}, {{"'","'"}});

/**
 * @details
 */
cEnclosure::cEnclosure()
    : aOuter_({{"[","]"}})
    , aQuate_({{"",""}})
{}

/**
 * @details
 */
cEnclosure::cEnclosure(const tDelimType& aOuter, const tDelimType& aQuate)
    : aOuter_(aOuter)
    , aQuate_(aQuate)
{}

/**
 * @details
 */
cEnclosure::cEnclosure(const cEnclosure& rhs)
    : aOuter_(rhs.aOuter_)
    , aQuate_(rhs.aQuate_)
{}

/**
 * @details
 */
cEnclosure& cEnclosure::operator=(const cEnclosure& rhs)
{
    aOuter_ = rhs.aOuter_;
    aQuate_ = rhs.aQuate_;
    return *this;
}

/**
 * @details
 */
void cEnclosure::vChange(const tDelimType& aOuter, const tDelimType& aQuate)
{
    aOuter_ = aOuter;
    aQuate_ = aQuate;
}

/**
 * @details
 */
const cEnclosure::tDelimType& cEnclosure::aGetOuter() const
{
    return aOuter_;
}

/**
 * @details
 */
const cEnclosure::tDelimType& cEnclosure::aGetQuate() const
{
    return aQuate_;
}

} // ps::lib

} // ps

