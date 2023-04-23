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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

class cStartValues
    : virtual protected cBootstrap
{
protected:
    /**
     * @brief
     * - true: Object definition order is based on object mutual dependency
     */
    const bool dependent_;
    /**
     * @brief
     * - true: In the representation of storage attributes of segments, 
     * extra extents are collectively defined as initial extents.
     */
    const bool compress_;
    /**
     * @brief
     * - true: Dividing the output file of create statement for each object.
     */
    const bool metadiv_;
    /**
     * @brief
     * Regulates the output of comments added to tables and views.
     * - 0: Do not output.
     * - 1: Output if it is commented.
     * - 2: In addition to 1, comment definition statements with empty values are also output.
     */
    const int32_t cmntlvl_;
    /**
     * @brief
     * - N is 0 or more and less or equal INT32_MAX. 
     *  Number of days to display the audit trail of object privilege from the current day.
     */
    const int32_t numdays_audit_;
    /**
     * @brief
     * Regulates the policy of migrating values of LONG or LONG RAW type.
     * - 0: Does not migrate
     * - 1: Migrates to LOB
     * - 2: Migrates to BFILE
     */
    const int32_t longtransit_;
    /**
     * @brief
     * - mask is a form that applies when expressing
     *   a DATE type value as a string.
     * @note
     *   See also the detail of elements of the mask on the onlien manual.
     *   https://docs.oracle.com/database/121/SQLRF/sql_elements004.htm#CDEHIFJA
     */
    std::string date_mask_;
    /**
     * @brief
     * - mask is a form that applies when expressing
     *   a TIMESTAMP or TIMESTAMP WITH LOCAL TIMEZONE type value as a string.
     */
    std::string timestamp_mask_;
    /**
     * @brief
     * - mask is a form that applies when expressing
     *   a TIMESTAMP WITH TIMEZONE type value as a string.
     */
    std::string timestamp_tz_mask_;
    cStartValues();
    ~cStartValues();
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
