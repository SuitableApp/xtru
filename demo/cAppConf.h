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

namespace demo
{

class cAppConf
    : public ps::lib::cConfigures::cOptionExtender
{
private:
    ps::lib::cConfigures& conf_;
    int32_t parallelism_;
    std::string userid;
    int32_t bulk_size_;
public:
    cAppConf(ps::lib::cConfigures& conf);
    virtual int32_t iValidate(
        std::ostream& os
        , const boost::program_options::variables_map& vm
    ) const;
};

} // ps::demo

} // ps



