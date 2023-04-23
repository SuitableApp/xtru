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

namespace system
{

/**
 * @brief
 * - Execute subprocess using popen system call.
 * - Messages output by the subprocess to the standard output can be read
 *   using the "in,out" mode argument.
 *
 * @param[in] cmd
 *   A command statement of subprocess.
 * @param[in,out] output
 * - The message output by the subprocess is stored.
 * - It is desirable to pass an empty string.
 *   Since the string stored before passing is lost at all after the call.
 * @return
 *   If encountered errors it will return -1.
 */
extern int32_t iStartSubProc(const std::string cmd, std::string& output);

/**
 * @brief
 *   returns current process name.
 */
extern std::string sGetCurrentProcessName(void);

/**
 * @brief
 */
std::vector<boost::filesystem::path> oGetPathsArray(void);

} // ps::lib::system

} // ps::lib

} // ps
