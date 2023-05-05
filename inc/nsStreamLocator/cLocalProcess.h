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
namespace nsStreamLocator
{

class cLocalProcessImpl;

/**
 * @class cLocalProcess
 * @brief
 * This is a class which can execute any CLI process specified
 * a statement given by CTOR and bidirectionally exchange
 * data streams with host process using three anonymous pipes.
 */
class cLocalProcess
    : public std::ostream
{
public:
    /**
     * @brief
     * CTOR activates the shell that executes the specified command
     * as an external process,
     * and outputs standard output and standard error output
     * from that process to the specified stream respectively.
     *
     * Output contents to each stream are collectively
     * forwarded when the external process is terminated.
     * CTOR can add the output generated using a function specified
     * respectively before and after the output of the content is started.
     *
     * operator<<() can be called for this instance,
     * becaouse of it inherits "std::ostream".
     * Passed R-value will be forward to standard input
     * of the external process.
     *
     * @param[in] sCommand
     *   Command string to be executed.
     * @exception
     *   Failed to initialize external process.
     */
    cLocalProcess(const std::string& sCommand);
private:
    std::unique_ptr<cLocalProcessImpl, void(*)(cLocalProcessImpl *)> oImpl_;
};

} // nsStreamLocator

} // lib

} // ps

