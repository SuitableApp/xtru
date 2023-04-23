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

namespace system
{

struct dir_filter_iter
    : public boost::iterator_facade<
        dir_filter_iter
        , boost::filesystem::path
        , boost::forward_traversal_tag
        , boost::filesystem::path
    >
{
    using bf_path = boost::filesystem::path;
    using impl_type = boost::filesystem::directory_iterator;

    dir_filter_iter()
        : impl_()
    {}
    dir_filter_iter(
        bf_path p
        , boost::regex rgx
    )
        : impl_(std::move(p))
        , rgx_(std::move(rgx))
    {
        if (! boost::filesystem::is_directory(p))
        {
            BOOST_THROW_EXCEPTION(
            boost::enable_error_info(std::domain_error("not a dir"))
            << boost::errinfo_file_name(p.string()));
        }
    }
private:
    friend class boost::iterator_core_access;
    bool equal(const dir_filter_iter& that) const
    {
        return this->impl_ == that.impl_;
    }
    void increment()
    {
        assert(impl_ != impl_type());
        for (;;)
        {
            ++impl_;
            if (impl_ == impl_type())
            {
                break;
            }
            std::string s(impl_->path().string());
            if (boost::regex_match( s, rgx_ ))
            {
                break;
            }
        }
    }
    bf_path dereference() const
    {
        assert(impl_ != impl_type());
        return *impl_;
    }
    impl_type impl_;
    boost::regex rgx_;
};

struct dir_filter_iter_maker
{
    using value_type = dir_filter_iter;
    explicit dir_filter_iter_maker(const boost::regex& rgx)
        : rgx_(rgx)
    {}
    value_type make() const
    {
        return value_type();
    }
    value_type make(boost::filesystem::path p) const
    {
        return value_type(std::move(p), rgx_);
    }
    template<typename... Args>
    auto operator()(Args&&... args)->decltype(make(args...))
    {
        return this->make(std::forward<Args>(args)...);
    }
private:
    boost::regex rgx_;
};

/**
 * @details
 */
cChmod::cChmod(
    const boost::filesystem::path& sDirName
    , const boost::filesystem::perms& iPerms
)
    : sDirName_(sDirName)
    , iPerms_(iPerms)
{
}

/**
 * @details
 */
cChmod& cChmod::operator()(const boost::filesystem::path& stem)
{
    ASSERT_OR_RAISE(iPerms_ != 0, std::runtime_error, std::string("Specified ZERO permission "));
    if (stem.string().find('*') != std::string::npos)
    {
        // Case by regular expression
        dir_filter_iter_maker di_maker(boost::regex(stem.c_str()));
        std::for_each(
            di_maker(sDirName_)  // start of iteration.
            , di_maker()         // end of iteration.
            , [this](const boost::filesystem::path& path)
            {
                boost::filesystem::permissions(path, iPerms_);
            }
        );
    }
    else
    {
        // Case by exact match
        const auto path = sDirName_ / stem;
        if (boost::filesystem::exists(path))
        {
            boost::filesystem::permissions(path, iPerms_);
        }
    }
    return *this;
}

} // ps::lib::system

} // ps::lib

} // ps
