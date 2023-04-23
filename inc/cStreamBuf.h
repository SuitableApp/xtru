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
 
class cOstream;

class cStreamBuf 
    : public std::stringbuf
{
private:
    std::ostream& os_;
    cOstream * stream_;
    cStreamBuf(const cStreamBuf& rhs) =delete;
    cStreamBuf& operator=(const cStreamBuf& rhs) =delete;
protected:
    virtual int32_t sync(void);
public:
    cStreamBuf(std::ostream& os, cOstream * stream);
};

} // ps::lib

} // ps

