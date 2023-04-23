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

void cIntervalTimer::vHookCbk(void)
{
    oTimer_.async_wait(boost::asio::bind_executor(strand_
        , boost::bind(&cIntervalTimer::vAction, this)));
}

void cIntervalTimer::vAction(void)
{
    if (!iCont_) return;
    oActionAtBreak_();
    oTimer_.expires_at(oTimer_.expiry() + iInterval_);
    vHookCbk();
    // Less than 10 seconds, prolong the next interval
    if (iInterval_ < unit_of_time(10))
    {
        iInterval_ += unit_of_time(iIncrnext_);
    }
}

cIntervalTimer::cIntervalTimer(
    boost::asio::io_context& io_ctx
    , std::function<void(void)> oActionAtBreak
    , const int& iInitIntervalSecs
    , const int& iNextIncreaseSecs
)
    : strand_(io_ctx)
    , oActionAtBreak_(oActionAtBreak)
    , iCont_(true)
    , iInterval_(unit_of_time(iInitIntervalSecs))
    , iIncrnext_(unit_of_time(iNextIncreaseSecs))
    , oTimer_(io_ctx, iInterval_)
{
    vHookCbk();
}

void cIntervalTimer::vSuspend(void)
{
    iCont_ = false;
    oTimer_.cancel();
}

void cIntervalTimer::vResume(void)
{
    iCont_ = true;
    vHookCbk();
}

void cIntervalTimer::vResetInterval(const int& iInitIntervalSecs)
{
    iInterval_ = unit_of_time(iInitIntervalSecs);
}

} // ps::lib

} // ps
