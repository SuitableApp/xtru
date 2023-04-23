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
 * @class cChmod
 * A functor class for adding permissions to a file.
 * @par Example way to be used:
   @code
     const boost::filesystem::perms iPerms
         = boost::filesystem::add_perms
         | boost::filesystem::owner_write
         | boost::filesystem::owner_exe
         ;
     cChmod obj(output_, iPerms);
     obj(par0_txt_)(par1_txt_)(conn_sql_);
   @endcode
 */
class cChmod
{
private:
    const boost::filesystem::path sDirName_;
    const boost::filesystem::perms iPerms_;
public:
    cChmod(
        const boost::filesystem::path& sDirName
        , const boost::filesystem::perms& iPerms
    );
    /**
     * @param[in] stem
     *   Pass stem the name of the file that exists in sDirName.
     *   This name can accept exact match or regular expression.
     */
    cChmod& operator()(const boost::filesystem::path& stem);
};

} // ps::lib::system

} // ps::lib

} // ps
