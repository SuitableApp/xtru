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
 
cConsole::cConsole()   // will call by ctor of "singleton_wrapper<cConsole >".
    : cOstream(std::cout, this)
    , bkp_(nullptr)
{}

cConsole::~cConsole()
{
    if (ofs_.is_open())
    {
        ofs_.close();
        std::cout.rdbuf(bkp_);
    }
}

void cConsole::vAddTag(std::ostream& os)
{
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    os << "[" << boost::posix_time::to_simple_string(now) << "]";
    os << " ";
}

cConsole& cConsole::oRedirectTo(const boost::filesystem::path& fn)
{
    if (ofs_.is_open())
    {
        // Do nothing, if this has already invoked.
        return *this;
    }
    auto append = false;
    auto it = fn.leaf().string().cbegin(), ite = fn.leaf().string().cend();
    if (*it == '+')
    {
        append = true;
        it++;
    }
    std::string name(it, ite);
    auto new_fn = fn.branch_path() / name;
    if (!new_fn.empty())
    {
        ofs_.open(new_fn, append ? std::ios_base::app : std::ios_base::trunc);
        BOOST_ASSERT_MSG(ofs_, szBuildMessage(ps::lib::E, new_fn, ::strerror(errno)));
        bkp_ = std::cout.rdbuf();
        std::cout.rdbuf(ofs_.rdbuf());
        std::cerr << "STDOUT is redirected to the file: " << new_fn 
            << " (with " << (append ? "append" : "truncate" ) << " mode)"
            << std::endl;
    }
    return *this;
}

} // ps::lib

} // ps

