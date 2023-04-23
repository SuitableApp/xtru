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

cTracer::cTracer()
    : cOstream(ofs_, this)
    , file_(boost::filesystem::current_path())
{
    std::ostringstream oss;
    oss << std::dec << boost::format("_%s_%d.%s")
        % ps::lib::system::sGetCurrentProcessName()
        % boost::this_process::get_id()
        % "log";
    file_ /= oss.str();
    ofs_.open(file_);
    BOOST_ASSERT_MSG(ofs_, szBuildMessage(ps::lib::E, file_, ::strerror(errno)));
}

cTracer::~cTracer()
{
    (*this) << boost::format("This file name is %s") % file_.string() << std::endl;
}

void cTracer::vAddTag(std::ostream& os)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    os << "[" << boost::posix_time::to_simple_string(now) << "]";
    os << "[" << std::hex << std::this_thread::get_id() << "]";
    os << " ";
}

cTracer& cTracer::oRedirectTo(const boost::filesystem::path& fn)
{
    if (!fn.empty())
    {
        ofs_.close();
        boost::system::error_code ec;
        boost::filesystem::exists(fn.parent_path(), ec);
        BOOST_ASSERT_MSG(!ec, szBuildMessage(ps::lib::E, fn.parent_path(), ec.message()));
        if (boost::filesystem::exists(fn))
        {
            boost::filesystem::remove(fn, ec);
            BOOST_ASSERT_MSG(!ec, szBuildMessage(ps::lib::E, fn, ec.message()));
        }
        boost::filesystem::exists(file_, ec);
        BOOST_ASSERT_MSG(!ec, szBuildMessage(ps::lib::E, file_, ec.message()));
        boost::filesystem::copy_file(file_, fn, ec);
        BOOST_ASSERT_MSG(!ec, szBuildMessage(ps::lib::E, fn, ec.message()));
        boost::filesystem::remove(file_, ec);
        BOOST_ASSERT_MSG(!ec, szBuildMessage(ps::lib::E, file_, ec.message()));
        ofs_.open(fn, std::ios_base::app);
        BOOST_ASSERT_MSG(ofs_, szBuildMessage(ps::lib::E, fn, ::strerror(errno)));
        file_ = fn;
    }
    return *this;
}

boost::filesystem::path cTracer::sGetName() const
{
    return file_;
}

} // ps::lib

} // ps
