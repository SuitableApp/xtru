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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

class cBootstrap
{
    std::string sGetAffinityMask(void);
    boost::system::error_code ec;
protected:
    ps::lib::cRtn& rtn_;
    const ps::lib::cConfigures& conf_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    const bool iSkipScr_;
    const uint32_t iBulkSize_;
    const int32_t iConcurrency_;
    const std::string prc_affinity_mask_;
    ps::lib::cSet<uint32_t> core_ids_;     ///< Array of core's IDs.
    ps::lib::sql::occi::cSvc::tPtr oSvc_;  ///< Communication for the Oracle Database.
    ps::lib::sql::lite3::cSqliteDb oDb_;   ///< Communication for the SQLite 3database.
    ps::app::xtru::copydd::cCopyDd oCopyDd_;
    const boost::filesystem::path output_;
    const std::string src_user_;   ///< A list of schema names delimited by the comma, tab or whitespace
    /**
     * @brief
     * Fill with the candidate of the schema name.
     * Elements are not only literals, but also 
     *   identifiers containing SQL wildcard characters.
     * cCopyDd::oConvEffectiveOwnerName is useful to materialize 
     *   to the list of existing schema names in the DB (ALL_USERS).
     */
    ps::lib::str_vct oWildOwners_;
    ps::lib::str_vct oOwners_;
    cBootstrap();
};

} // ps::app::xtru

} // ps::app

} // ps
