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

#pragma once

namespace ps
{

namespace lib
{

/**
 * @class cStat
 * @brief 
 * This class provides time piriod and cumulating the counters of statistics.<br/>
 *   For the statistics shown below, hold the total since the instance was created.<br/>
 *
 * -# Erapsed time. 
 * -# Total output bytes.
 *
 * It is implemented as a singleton.<br/>
 * Accessing to member variables for counting is atomically kept.<br/>
 * Therefore calling each member function in this class is thread-safe.<br/>
 */
class cStat
    : public boost::serialization::singleton< cStat >
{
    friend class boost::serialization::singleton< cStat >;
private:
    std::atomic<int64_t> iOutputBytes_;  ///< @brief Total output bytes.
    /// @brief holds a time at instanciation.
    const boost::posix_time::ptime time_at_started_;
    /**
     * @brief
     * Constructor and Destructor will recursively call by <br/>
     * ctor and dtor of "singleton_wrapper<cStat>".<br/>
     * Therefore it may not be published.
     */
    cStat();
    ~cStat()
    {}
public:
    /**
     * @brief
     * @param[in] iOutputBytes
     *   Pass the difference since the last call. The unit is bytes.
     */
    void vAddOutputBytes(const int64_t& iOutputBytes);
    /**
     * @brief
     * @return Total amount of bytes.<br/>
     *   The total amount of bytes entered in all vAddOutputBytes() calls.
     */
    int64_t iGetOutputBytes() const;
    /**
     * @brief
     * @return The elapsed time starting from the time 
     *   when this singleton was constructed. <br>
     *   The unit is milli-seconds.
     */
    int32_t iDurationMilliSeconds() const;
    /**
     * @brief
     */
    std::string sGetStartDateTime() const;
};

} // ps::lib

} // ps

