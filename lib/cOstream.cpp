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

cOstream::mutex_type* cOstream::oGetMutex(void)
{
    return &mtx_;
}

void cOstream::vManipulate(std::ostream& (*pf)(std::ostream&))
{
    pf(*static_cast<std::ostream*>(this));
}

cOstream::cOstream(std::ostream& os, cOstream* ptr)
    : std::ostream(&oBuf_)
    , oBuf_(os, ptr)
    , iTagDateTimeEnabled_(false)
{}

cOstream::~cOstream()
{}

cSafeOstream<cOstream, cOstream::mutex_type> cOstream::operator<<(std::ostream& (*pf)(std::ostream&))
{
    cSafeOstream<cOstream, mutex_type> os(*this, mtx_
        , std::bind(&cOstream::vManipulate, this, std::ref(pf)));
    return os;
}

bool cOstream::iGetTagDateTimeEnabled() const
{
    return iTagDateTimeEnabled_;
}

void cOstream::vSwitchTagDateTime()
{
    iTagDateTimeEnabled_ = !iTagDateTimeEnabled_;
}

} // ps::lib

} // ps

