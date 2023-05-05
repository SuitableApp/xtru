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

namespace app
{

namespace mpx
{

/**
 * @class cExtProc
 * @brief It controls the invocation of external program and relays
 *   delivery  of STDOUT, STDERR, STDIN and return codes
 *   during the lifetime.
 *
 */
class cExtProc
{
public:
    /**
     * @brief
     *   An instance of cExtProc type is created.
     *
     * @param[in] sExecutable
     * - A program name that is invoked.
     * - Not only the executable form but also scripts of various
     *   interpreters (i.e. such as sh, python, perl,...) to which
     *   execute permission is given.
     * @param[in] sParameters
     * - It is passed when sExecutable is accompanied by an argument.
     * @param[in] sProgress
     * - Display progress as a fraction.
     * @param[in] bIgnorable
     * - true: ignored even if non-ZERO is returned from external program.
     * - Generally non-ZERO indicates abnormal termination.
     * @param[in] iNice
     * - Specify the execution priority of external programs
     *   as a numerical value as necessary.
     * - -20 is the highest priority, +19 is the lowest priority.
     *   Only the root user can be specified below -1.
     * @param[in] sChildStdin
     * - A string sent to STDIN (standard input) of external program.
     *   (i.e. username, password.)
     * @param[in,out] oOut
     * - An output stream for redirecting messages received from
     *   STDOUT (standard output) of an external program.
     * @param[in,out] oErr
     * - An output stream for redirecting messages received from
     *   STDERR (standard outerr) of an external program.
     *
     * @return
     * - A pointer of cExtProc.
     * - Assume that a delete operator is passed to a smart pointer
     *   with a built-in delete operator.
     */
	static cExtProc * oMakeExtProc(
		const std::string& sExecutable
		, const std::string& sParameters
		, const std::string& sProgress
		, const bool bIgnorable
        , const int32_t iNice
		, const std::string& sChildStdin
        , std::ostream& oOut
        , std::ostream& oErr
	);
	virtual ~cExtProc();
    /**
     * @brief
     *   Control the process from the start to the end of the external program.
     *
     * @note
     *   Thread safe is guaranteed for any call.
     */
	virtual void vRun() = 0;
};

} // ps::app::mpx

} // ps::app

} // ps

