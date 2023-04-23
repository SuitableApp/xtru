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
 * @class cAppConf
 * @brief Define program-specific parameters.
 */
class cAppConf
    : public ps::lib::cConfigures::cOptionExtender
{
private:
    enum tNumericConst
        { iMaxParalellism = 64 ///< Maximum value of parallel degree
        , iMinNice = -20       ///< Maximum priority.
        , iMaxNice = 19        ///< Minimum priority.
        };
    ps::lib::cConfigures& conf_;
    int32_t iParallelism_;     ///< Degree of parallelism.
    int32_t iNice_;            ///< Priority given to child process.
    std::string sMpxlist_;     ///< Comma-separated list of processes to be executed in parallel.
    std::string sIgnlist_;     ///< Comma-separated list of non-parallel execution target processes.
    std::string sCsi_;         ///< String to pass to standard input of child process.
    std::string sCso_;         ///< The location of the message received from the standard output of the child process.
    std::string sCse_;         ///< The location of the message received from the standard error of the child process.
    std::string sMakeSh_;      ///< Script with execution order of child process and argument.
public:
    /**
     * @brief
     * @param [in,out] conf
     */
    cAppConf(ps::lib::cConfigures& conf);
    /**
     * @brief
     *   Validation of input parameters.
     *
     * @param [in] os
     *   Message output destination
     * @param [in,out] vm
     *   Map data of constructed parameters.
     */
    virtual int32_t iValidate(
        std::ostream& os
        , const boost::program_options::variables_map& vm
    ) const;
};

} // ps::app::mpx

} // ps::app

} // ps

