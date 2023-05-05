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

class cFileSystemImpl;

/**
 * @class cFileSystem
 * @brief
 */
class cFileSystem
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
    explicit cFileSystem(const std::string& sName);
private:
    std::unique_ptr<cFileSystemImpl, void(*)(cFileSystemImpl *)> oImpl_;
};

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps

