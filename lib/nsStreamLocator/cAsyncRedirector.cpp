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
 * @class cAsyncRedirectorImpl
 * @brief
 * This is an implementation of cAsyncRedirector which continues
 * to forward a stream of data from a file descriptor given by vReadAsync
 * to a std::ostream given by CTOR until encountered EOF of the descriptor.
 * Forwarding is dealt by o thread other than main.
 * vWait waits completion of Forwarding.
 */
class cAsyncRedirectorImpl
{
    constexpr static int iBuf = (64<<10); // Maximum buffer length [bytes].
    ps::lib::cTracer& trc_;
    /// A destination of the streaming.
    std::ostringstream oDst_;
    /// stores messages from STDOUT or STDERR of external process.
    std::string sMsg_;
    boost::asio::io_service io_;
    boost::asio::posix::stream_descriptor oSrc_;
    boost::thread thr_;
public:
    cAsyncRedirectorImpl()
        : trc_(ps::lib::cTracer::get_mutable_instance())
        , oSrc_(io_)
    {}
    ~cAsyncRedirectorImpl()
    {}
    void vReadAsync(const int& iSrc)
    {
        oSrc_.assign(iSrc);
        thr_ = boost::thread(boost::bind(&boost::asio::io_service::run, &io_));
        io_.post([this]()
        {
            /// A buffer recieving datas fomr oSrc_ .
            boost::array<char, iBuf> buf;
            boost::system::error_code ec;
            do
            {
                const size_t len = oSrc_.read_some(boost::asio::buffer(buf), ec);
                oDst_.write(buf.data(), len);
            } while (!ec);
            if (ec && ec != boost::asio::error::eof)
            {
               throw boost::system::system_error{ec, boost::system::system_category().name()};
            }
        });
    }
    void vWait()
    {
        thr_.join();
        sMsg_.assign(oDst_.str());
    }
    const std::string& sGetMsg() const
    {
        return sMsg_;
    }
};

cAsyncRedirector::cAsyncRedirector()
: oImpl_(new cAsyncRedirectorImpl()
    , vRegularDeleter<cAsyncRedirectorImpl>)
{}

cAsyncRedirector::~cAsyncRedirector()
{}

void cAsyncRedirector::vReadAsync(const int& iSrc)
{
    oImpl_->vReadAsync(iSrc);
}

void cAsyncRedirector::vWait()
{
    oImpl_->vWait();
}

const std::string& cAsyncRedirector::sGetMsg() const
{
    return oImpl_->sGetMsg();
}

} // nsStreamLocator

} // lib

} // ps
