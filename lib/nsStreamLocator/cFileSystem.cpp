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
namespace nsStreamLocator
{

/**
 * @class cFileSystemImpl
 * @brief
 */
class cFileSystemImpl
    : public boost::iostreams::stream_buffer<boost::iostreams::file_sink>
{
public:
    explicit cFileSystemImpl(const std::string& sName);
    ~cFileSystemImpl();
private:
    /// @brief A mutex for synchronizing states between each instance.
    static std::mutex mtx_;
    /// @brief Object for trace output.
    ps::lib::cTracer& trc_;
    /// @brief Output destination pipe name.
    boost::filesystem::path name_;
};

std::mutex cFileSystemImpl::mtx_;

cFileSystemImpl::cFileSystemImpl(const std::string& sName)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , name_(sName)
{
    std::lock_guard<std::mutex> lock(mtx_);
    boost::filesystem::path sParentPath(name_.parent_path());
    boost::system::error_code ec;
    boost::filesystem::exists(sParentPath, ec);
    ASSERT_OR_RAISE(
        !ec
        , std::runtime_error
        , boost::format("%s: specified %s") % ec.message() % sParentPath
    );
    this->open(name_.string(), std::ios::out | std::ios::binary);
    trc_ << boost::format("cFileSystem is opend: %s") % name_ << std::endl;
}

cFileSystemImpl::~cFileSystemImpl()
{
    std::lock_guard<std::mutex> lock(mtx_);
    try
    {
        this->close();
        trc_ << boost::format("cFileSystem is closed:%s") % name_ << std::endl;
    }
    catch (...)
    {
        // do nothing
    }
}

/**
 * works to mediate between the interface and the implementation.
 */
cFileSystem::cFileSystem(const std::string& sName)
     : oImpl_(new cFileSystemImpl(sName)
    , vRegularDeleter<cFileSystemImpl>)
{
    this->rdbuf(oImpl_.get());
}

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps

