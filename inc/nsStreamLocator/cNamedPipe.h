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
namespace nsStreamLocator
{

class cNamedPipeImpl;

/**
 * @class cNamedPipe
 * @brief
 * This is a class which can create or remove any named pipe specified
 * a filname-path given by CTOR.
 * Wehn the stream data is written to the instance of this class,
 * it can be fowarded to the another process via the named pipe.
 * Instance can be executed operator such as operator<<().
 */
class cNamedPipe
    : public std::ostream
{
public:
    /**
     * @brief
     * 
     * @param[in] sName
     *   Name of the pipe to be created.
     * @exception
     *   Failed to create named pipe.
     */
    explicit cNamedPipe(const std::string& sName);
private:
    std::unique_ptr<cNamedPipeImpl, void(*)(cNamedPipeImpl *)> oImpl_;
};

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
