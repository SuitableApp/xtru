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

/**
 * @class cIntervalTimer
 *
 * @brief
 */
class cIntervalTimer
{
private:
    using unit_of_time = boost::asio::chrono::seconds;
    enum { iDefaultInterval = 2 ///< unit: second
         , iDefaultIncrnext = 1 ///< unit: second
    };
    boost::asio::io_context::strand strand_;
    std::function<void()> oActionAtBreak_;
    bool iCont_;
    unit_of_time iInterval_;
    const unit_of_time iIncrnext_;
    boost::asio::steady_timer oTimer_;
    /**
     * @details
     */
    void vHookCbk(void);
    /**
     * @details
     */
    void vAction(void);
public:
    /**
     * @details
     *
     * @param[in,out] io_ctx
     * - 
     * @param[in] oActionAtBreak
     * - Processing to be executed when timeout occurs.
     * @param[in] iInitIntervalSecs
     * - Pass wait time until the next timeout. The unit is seconds.
     * @param[in] iNextIncreaseSecs
     * - Pass differentiation when gradually increasing
     *   the waiting time. The unit is seconds.
     */
    cIntervalTimer(
        boost::asio::io_context& io_ctx
        , std::function<void(void)> oActionAtBreak
        , const int& iInitIntervalSecs
        , const int& iNextIncreaseSecs
    );
    cIntervalTimer(
        boost::asio::io_context& io_ctx
        , std::function<void(void)> oActionAtBreak
    )
        : cIntervalTimer(
            io_ctx, oActionAtBreak
            , iDefaultInterval, iDefaultIncrnext
        )
    {}
    /**
     * @details
     * - To break blocking by async_wait().
     * - To stop subsequent hooking the callback function.
     */
    void vSuspend(void);
    /**
     * @details
     * - To restart subsequent hooking the callback function.
     */
    void vResume(void);
    /**
     * @details
     * - To reset interval time.
     */
    void vResetInterval(const int& iInitIntervalSecs);
    void vResetInterval(void)
    {
        vResetInterval(iDefaultInterval);
    }
};

} // ps::lib

} // ps
