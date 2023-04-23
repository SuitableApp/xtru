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
#include "cAppConf.h"

namespace ps
{

namespace demo
{

namespace po = boost::program_options;

cAppConf::cAppConf(ps::lib::cConfigures& conf)
    : conf_(conf), parallelism_(0), bulk_size_(0)
{
    conf_.vSetAuxConf(this);
    conf_.oGetOptionsDesc().add_options()
    ("parallelism"
        , po::value<int32_t>(&parallelism_)
            ->default_value(conf.iGetNumPhisicalCores())
                ->value_name("N")
         , "N is a positive integer. Set a concurrency number.")
    ("bulk_size"
        , po::value<int32_t>(&bulk_size_)
            ->default_value(1000)
                ->value_name("N")
         , "N is a positive integer. Set a number of array size.")
    ("userid,u"
        , po::value<std::string>(&userid)
            ->default_value("SYSTEM/MANAGER")
                ->value_name("<user>/<pass>[@<alias>]")
        , "Set the connect string.")
    ;
}

int32_t cAppConf::iValidate(
    std::ostream& os
    , const boost::program_options::variables_map& vm
) const
{
    BOOST_ASSERT(vm.size() > 0);
    int32_t iErrors = 0;
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "parallelism_", parallelism_ > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "userid", userid.size() > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "bulk_size_", bulk_size_ > 0);
    return iErrors;
}

} // ps::demo

} // ps

