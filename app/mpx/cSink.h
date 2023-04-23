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

namespace app
{

namespace mpx
{

/**
 * @class cSink
 * @brief Implement singletons of output stream that can be thread-safe
 * operations (operator<<).
 * Multiple singletons can be prepared by changing the type of T.
 * 
 */
template<class T>
class cSink
    : public ps::lib::cOstream
    , public boost::serialization::singleton< cSink<T> >
{
    friend class boost::serialization::singleton< cSink<T> >;
private:
    boost::filesystem::path file_;       ///< Name of trace file.
    bool once_;
    boost::filesystem::ofstream ofs_;    ///< a trace file.
    cSink()  // will call by ctor of "singleton_wrapper<cSink<T> >".
        : cOstream(ofs_, this)
        , file_(boost::filesystem::current_path())
        , once_(true)
    {
        std::ostringstream oss;
        oss << std::dec << boost::format("_%s_%d_%d.%s")
            % ps::lib::system::sGetCurrentProcessName()
            % boost::this_process::get_id()
            % T::fno_
            % "log";
        file_ /= oss.str();
        ofs_.open(file_);
        BOOST_ASSERT_MSG(ofs_, szBuildMessage(ps::lib::E, file_, ::strerror(errno)));
    }
    ~cSink()
    {
        (*this) << boost::format("This file name is %s") % file_.string() << std::endl;
    }
public:
    cSink& open(const boost::filesystem::path& fn)
    {
        if (!fn.empty())
        {
            BOOST_ASSERT_MSG(once_, "Already open() has been called.");
            BOOST_ASSERT_MSG(fn.size(), "File name must not be empty.");
            ofs_.close();
            boost::system::error_code ec;
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
            once_ = false;
        }
        return *this;
    }
    virtual void vAddTag(std::ostream& os)
    {}
};

} // ps::app::mpx

} // ps::app

} // ps
