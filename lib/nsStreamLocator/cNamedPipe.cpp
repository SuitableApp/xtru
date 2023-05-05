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

#include <pslib.h>

namespace ps
{
namespace lib
{
namespace nsStreamLocator
{

/**
 * @class cNamedPipeImpl
 * @brief
 * A stream buffer for output to a named pipe.
 * This class basically assumes that it is used as an internal buffer
 * of the cNamedPipe class.
 */
class cNamedPipeImpl
    : public boost::iostreams::stream_buffer<boost::iostreams::file_sink>
{
public:
    explicit cNamedPipeImpl(const std::string& sName);
    ~cNamedPipeImpl();
private:
    /// @brief A mutex for synchronizing states between each instance.
    static std::mutex mtx_;
    /// @brief Map for managing the state of each output destination.
    static ps::lib::cMap<std::string, std::pair<int, bool>> oNamedPipes;
    /// @brief Object for trace output.
    ps::lib::cTracer& trc_;
    /// @brief Output destination pipe name.
    std::string name_;
};

std::mutex cNamedPipeImpl::mtx_;
ps::lib::cMap<std::string, std::pair<int, bool>> cNamedPipeImpl::oNamedPipes;

cNamedPipeImpl::cNamedPipeImpl(const std::string& sName)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , name_(sName)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto& oItem = oNamedPipes[name_];
    if (oItem.first == 0)
    {
        const int rc = ::mkfifo(name_.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        oItem.second = (rc == 0);
        const std::string sSupplimental = 
            (boost::format(" and it will%s be removed at finished" )
             % (oItem.second ? "" : " NOT")).str();
        trc_ << boost::format("%s created%s") % name_ % sSupplimental << std::endl;
    }
    struct ::stat statBuf;
    ASSERT_OR_RAISE(::stat(name_.c_str(), &statBuf) == 0
        , std::runtime_error
        , boost::format("stat : errno=%d") % errno);
    ASSERT_OR_RAISE(S_ISFIFO(statBuf.st_mode)
        , std::runtime_error
        , name_ + " exists and is not a named pipe");
    ++oItem.first;
    trc_ << boost::format("%s opening - referenced %d times in process")
        % name_ % oItem.first << std::endl;
    this->open(name_, std::ios::out | std::ios::binary);
    trc_ << boost::format("cNamedPipe is opend: \"%s\"") % name_ << std::endl;
}

cNamedPipeImpl::~cNamedPipeImpl()
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto& oItem = oNamedPipes[name_];
    try
    {
        trc_ << boost::format("%s closing - referenced %d times in process")
            % name_ % oItem.first << std::endl;
        this->close();
    }
    catch (...)
    {
        // do nothing
    }
    if (--oItem.first == 0)
    {
        if (oItem.second)
        {
            ::unlink(name_.c_str());
            trc_ << boost::format("%s removed") % name_ << std::endl;
        }
        oNamedPipes.erase(name_);
    }
    trc_ << boost::format("cNamedPipe is closed:\"%s\"") % name_ << std::endl;
}

/**
 * works to mediate between the interface and the implementation.
 */
cNamedPipe::cNamedPipe(const std::string& sName)
     : oImpl_(new cNamedPipeImpl(sName)
    , vRegularDeleter<cNamedPipeImpl>)
{
    this->rdbuf(oImpl_.get());
}

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
