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

/**
 * @class cSignalImpl
 */
class cSignalImpl
{
private:
    ps::lib::cMap<int32_t, std::string> oSigs_;
    // io_service is deprecated.
    // io_context should be used instead of io_service.
    /// See also [io_service](https://www.boost.org/doc/libs/1_68_0/doc/html/boost_asio/reference/io_service.html)
    boost::asio::io_context oIoCtx_;
    /// Assign a user-defined handler to the signal and wait for signal reception.
    boost::asio::signal_set oReciver_;
    /// A functor that has a user-defined handler.
    cSignal::tHandlerType oHandler_;
    /// An output stream for handling console and trace files simultaneously.
    ps::lib::cDistributor& mos_;
    /// A function driven when receiving a signal. See also
    /// [Signal handler requirements](https://www.boost.org/doc/libs/1_68_0/doc/html/boost_asio/reference/SignalHandler.html)
    void vHandler(const boost::system::error_code& err, int32_t);
    /// Start signal reception
    void vStartReception();
public:
    /// Construct a signal set registered for process termination.
    cSignalImpl(cSignal::tHandlerType);
    ~cSignalImpl();
    std::thread thr_;
};

void cSignalImpl::vHandler(const boost::system::error_code& err, int32_t sig)
{
    oHandler_(); // Called back a user-defined handler.
    mos_ << boost::format(
            R"(%s Requested to break. %s was signaled. msg="%s")"
        )
            % sClass(err ? ps::lib::E : ps::lib::I)
            % oSigs_.at(sig) % err.message()
        << std::endl;
}

void cSignalImpl::vStartReception()
{
    // Start an asynchronous operation to prepare a signal to be delivered.
    // It always returns immediately.
    oReciver_.async_wait(
        boost::bind(
            &cSignalImpl::vHandler, this
            , boost::asio::placeholders::error
            , boost::asio::placeholders::signal_number
        )
    );
    oIoCtx_.run();
}

cSignalImpl::cSignalImpl(cSignal::tHandlerType oHandler)
    : oSigs_{{SIGINT, "SIGINT"}, {SIGTERM, "SIGTERM"}}
    , oReciver_(oIoCtx_)
    , oHandler_(oHandler)
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , thr_(std::bind(&cSignalImpl::vStartReception, this))
{
    for (auto sig: oSigs_)
    {
        oReciver_.add(sig.first);
    }
}

cSignalImpl::~cSignalImpl()
{
    for (auto sig: oSigs_)
    {
        oReciver_.remove(sig.first);
    }
    oIoCtx_.stop();
    // Forgetting to wait, "terminate called without an active exception" will be encountered.
    thr_.join();
}

cSignal::cSignal(tHandlerType oHandler)
    :oImpl_(new cSignalImpl(oHandler))
{}

cSignal::~cSignal()
{}

} // ps::lib

} // ps
