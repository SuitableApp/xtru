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
#include <mpx.h>

namespace ps
{

namespace app
{

namespace mpx
{

/**
 * @class cStartValues
 * @brief Analyze and store the parameters used by the program.
 * 
 */
class cStartValues
{
protected:
    const ps::lib::cEnv& env_;
    ps::lib::cRtn& rtn_;
    const ps::lib::cConfigures& conf_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    ps::lib::cStat& stat_;
    /**
     * @brief
     * - Maximum number of processes to be executed in parallel.
     * - An integer from 1 to 64 is valid.
     */
    const int32_t iParallelism_;
    /**
     * @brief
     * - Specify the nice of the forking process.
     * - An integer from -20 to +19 is valid.
     */
    const int32_t iNice_;
    /**
     * @brief
     */
    ps::lib::tSep rule_;
    /**
     * @brief
     */
    ps::lib::cSet<std::string> oMaxList_;
    /**
     * @brief
     */
    ps::lib::cSet<std::string> oIgnList_;
    /**
     * @brief
     */
    boost::filesystem::path sStartup_;
    /**
     * @brief
     */
    std::string sCsi_;
    /**
     * @brief
     */
    std::string sCso_;
    /**
     * @brief
     */
    std::string sCse_;
    /**
     * @brief
     */
    boost::filesystem::path sCmdFile_;
    /**
     * @brief
     */
    ps::lib::str_vct sParamValue_;
    /**
     * @brief
     * A map of placeholder name and value pairs.
     */
    ps::lib::cMap<std::string, std::string> oTableOfReplacement_;
    /**
     * @brief
     */
    cStartValues();
    /**
     * @brief
     */
    ~cStartValues();
};

} // ps::app::mpx

} // ps::app

} // ps
