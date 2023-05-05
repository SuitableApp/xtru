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

namespace ps
{

namespace lib
{

/**
 * @brief
 */
class cEnclosure
{
public:
    typedef std::array<std::string, 2> tDelimType;
private:
    tDelimType aOuter_, aQuate_;
public:
    static cEnclosure oSqlInList; ///  Difinition of a sspecal format "('htem-A','item-B', ... )"
    cEnclosure();
    cEnclosure(const tDelimType& aOuter, const tDelimType& aQuate);
    cEnclosure(const cEnclosure& rhs);
    cEnclosure& operator=(const cEnclosure& rhs);
    void vChange(const tDelimType& aOuter, const tDelimType& aQuate);
    const tDelimType& aGetOuter() const;
    const tDelimType& aGetQuate() const;
};

} // ps::lib

} // ps
