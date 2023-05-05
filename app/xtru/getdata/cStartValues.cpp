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
#ifdef __sun
#endif
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getdata
{

cStartValues::cStartValues()
    : input_(ps::lib::sHasParentOrPrefixedPath(conf_.as<std::string>("input"), conf_.sGetPsHome()))
    , queryfix_(ps::lib::sHasParentOrPrefixedPath(conf_.as<std::string>("queryfix"), conf_.sGetPsHome()))
    , queryvar_(ps::lib::sHasParentOrPrefixedPath(conf_.as<std::string>("queryvar"), conf_.sGetPsHome()))
    , dfile_alt_dirs_(conf_.as<std::string>("dfile_alt_dirs"))
    , exec_load_(conf_.as<std::string>("exec_load"))
    , exec_plus_(conf_.as<std::string>("exec_plus"))
    , extnamesql_(conf_.as<std::string>("extnamesql"))
    , sFindstrCmd_(conf_.as<std::string>("findstrcmd"))
    // input_
    , filebind_(input_ / conf_.as<std::string>("filebind"))
    , fileexcpt_(input_ / conf_.as<std::string>("fileexcpt"))
    , filefixed_(input_ / conf_.as<std::string>("filefixed"))
    , filetable_(input_ / conf_.as<std::string>("filetable"))
    , pre_rep_exec_pls_(input_ / conf_.as<std::string>("pre_rep_exec_pls"))
    , post_rep_exec_pls_(input_ / conf_.as<std::string>("post_rep_exec_pls"))
    // output_
    , dataext_(conf_.as<std::string>("dataext"))
    , extnameclob_(conf_.as<std::string>("extnameclob"))
    , extnameblob_(conf_.as<std::string>("extnameblob"))
    , queryfilename_(conf_.as<std::string>("queryfilename"))
    , stream_locator_(conf_.as<std::string>("stream_locator"))  
    , suppress_ctrlf_(conf_.as<bool>("suppress_ctrlf"))
    , partitioning_(conf_.as<int>("partitioning"))
    , sMngScrOut_(conf_.as<std::string>("mngscrout"))
    , iMngScrOut_(0)
    // scripts for the reloading.
    , mode_(
        boost::filesystem::fstream::ios_base::out
        | boost::filesystem::fstream::ios_base::trunc
    )
    , make_sh_(conf_.as<std::string>("scr_make_sh"))
    , par0_txt_(conf_.as<std::string>("par_par0_txt"))
    , par1_txt_(conf_.as<std::string>("par_par1_txt"))
    , conn_sql_(conf_.as<std::string>("scr_conn_sql"))
    , wait_here_(conf_.as<std::string>("scr_wait_here_sh"))
    , disable_deps_(conf_.as<std::string>("scr_disable_deps_sql"))
    , enable_deps_(conf_.as<std::string>("scr_enable_deps_sql")) // Query only use
    , sSuffixDeps_("_DEPS")
    , sSuffixRefs_("_REFS")
{
    mos_ << boost::format("input directory is %s") % input_ << std::endl;
    mos_ << boost::format("queryfix directory is %s") % queryfix_ << std::endl;
    mos_ << boost::format("queryvar directory is %s") % queryvar_ << std::endl;
    const auto s_stdout = conf_.as<std::string>("stdout");
    const auto li_stdout = std::strtol(s_stdout.c_str(),NULL,0);
    // Scanning a value for the selection whether redirecting stream or not.
    ASSERT_OR_RAISE_FNC(errno != ERANGE, std::runtime_error
        , [=](void)
        {
            return (boost::format("Raised %s. Specified '%s' to the parameter stdout.") %
                (li_stdout == LONG_MIN ? "underflow" : "overflow") % s_stdout).str();
        }
    );
    if ((stdout_ = static_cast<int32_t>(li_stdout)) != 0)
    {
        mos_ << boost::format("Non-default value is assigned %s into parameter stdout.")
            % s_stdout << std::endl;
    }
    // Initializing STREM LOCATOR.
    ps::lib::nsStreamLocator::vInitialize(
        // destination for output.
        // Scheme name is placed front of "://" and defines kind of redirector.
        stream_locator_
        , stdout_   ///
        , output_   /// to be assigned for {O} macro.
        // Types of target file generated when table is unloaded.
        // These values are able to refer as:
        // ps::lib::nsStreamLocator::cStreamLocator::
        , {{dataext_, "ctl", extnameclob_, extnameblob_}}
        , suppress_ctrlf_ // True means suppressing the controlfile outputting.
    );
    ASSERT_OR_RAISE(4 >= partitioning_ && 0 <= partitioning_
        , std::runtime_error, boost::format("FAILED: Out of range. Acutually specified %d.") % partitioning_);
    /*
     * Converts a string representing a hexadecimal number to an integer. 
     * The assumption that hexadecimal numbers are prefixed with "0x".
     */
    std::stringstream ss(sMngScrOut_);
    ss >> std::hex >> iMngScrOut_;
}

cStartValues::~cStartValues()
{}

/**
 * @details
 */
const boost::filesystem::path& cStartValues::sGetParfName(const bool& iCondition)
{
    return iCondition ? par1_txt_ : par0_txt_;
}

/**
 * @details
 */
ps::lib::tPtrFstream cStartValues::oGetStreamToMakeSh(
    const boost::filesystem::path output
    , const bool iIsNullDevice
){
    boost::filesystem::path str = iIsNullDevice ? "/dev/null" : output / make_sh_;
    return ps::lib::open_with_check(str, mode_);
}

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps
