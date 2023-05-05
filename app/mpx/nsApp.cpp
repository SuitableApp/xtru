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
#include "nsApp.h"

namespace ps
{

namespace app
{

namespace mpx
{

const int cStdOut::fno_ = STDOUT_FILENO;
const char* cStdOut::file_ = "_stdout.log";

const int cStdErr::fno_ = STDERR_FILENO;
const char* cStdErr::file_ = "_stderr.log";

} // ps::app::mpx

} // ps::app

} // ps
