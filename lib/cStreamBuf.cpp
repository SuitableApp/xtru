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
 
cStreamBuf::cStreamBuf(std::ostream& os, cOstream * stream)
    : os_(os), stream_(stream)
{}

int32_t cStreamBuf::sync(void)
{
    if (stream_->iGetTagDateTimeEnabled())
    {
        stream_->vAddTag(os_);
    }
    os_ << std::stringbuf::str();
    ASSERT_OR_RAISE(os_, std::runtime_error, ::strerror(errno));
    os_.flush();

    str("");

    return 0;
}

} // ps::lib

} // ps

