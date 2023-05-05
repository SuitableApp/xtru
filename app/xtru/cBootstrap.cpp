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
#include <sys/processor.h>
#include <thread.h>
#endif
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

std::string cBootstrap::sGetAffinityMask(void)
{
    const auto ans = conf_.as<std::string>("prc_affinity_mask");
    ASSERT_OR_RAISE(0 < ans.size(), std::runtime_error, "FAILED: prc_affinity_mask is empty.");
    return ans;
}

cBootstrap::cBootstrap()
    : rtn_(ps::lib::cRtn::get_mutable_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , iSkipScr_(conf_.as<bool>("skipscr"))
    , iBulkSize_(conf_.as<int32_t>("bulk_size"))
    , iConcurrency_ (conf_.as<int32_t>("parallelism"))
    , prc_affinity_mask_(sGetAffinityMask())
    , core_ids_(ps::lib::oPrintPagesParser(prc_affinity_mask_, 0U, conf_.iGetNumLogicalCores() - 1))
    , output_(ps::lib::sHasParentOrPrefixedPath(conf_.as<std::string>("output"), conf_.sGetPsHome()))
    , src_user_(conf_.as<std::string>("src_user"))
{
    {
        mos_ << boost::format("Bulk size is %d") % iBulkSize_ << std::endl
             << boost::format("Requested concurrency is %d") % iConcurrency_ << std::endl
             << boost::format(R"(Requested prc_affinity_mask is "%s", Actually )") % prc_affinity_mask_
             << core_ids_ << std::string(" to be used.") << std::endl;
    }
    // Setting proces affinity mask.
#ifdef __sun
    // Querying and printing affinities for the current LWP
    auto i = 0;
    procset_t ps;
    uint_t nids = core_ids_.size();
    std::unique_ptr<id_t[]> ids(new id_t[nids]);
    std::for_each(core_ids_.cbegin(), core_ids_.cend()
    , [&](const uint32_t& cpuid)
    {
        ids[i++] = cpuid;
    });
    setprocset(&ps, POP_AND, P_PID, P_MYID, P_LWPID, thr_self());
    uint32_t flags = PA_TYPE_CPU | PA_AFF_STRONG;
    ASSERT_OR_RAISE(
        processor_affinity(&ps, &nids, ids.get(), &flags) == 0
        , std::runtime_error, "FAILED: setting affinity."
    );
#ifndef NDEBUG
    flags = PA_QUERY;
    nids = 0;
    ASSERT_OR_RAISE(
        processor_affinity(&ps, &nids, NULL, &flags) == 0
        , std::runtime_error, "FAILED: Querying number of ids."
    );
    mos_ << boost::format("LWP %d has affinity for %d CPUs.") % thr_self() % nids << std::endl;
    if (nids)
    {
        flags = PA_QUERY;
        ids.reset(new id_t[nids]);
        ASSERT_OR_RAISE(
            processor_affinity(&ps, &nids, ids.get(), &flags) == 0
            , std::runtime_error, "FAILED: Querying ids."
        );
        if (nids == 0)
        {
            mos_ << std::string("Current LWP has no affinity set.") << std::endl;
        }
        else
        {
            ps::lib::cVector<id_t> vct;
            std::copy(&ids[0], &ids[nids], std::back_inserter(vct));
            mos_ << std::string("Current LWP has affinity for the following CPU(s): ") << vct << std::endl;
        }
    }
#endif
#else
    cpu_set_t mask;
    CPU_ZERO(&mask);
    std::for_each(core_ids_.cbegin(), core_ids_.cend()
    , [&](const uint32_t& cpuid)
    {
        CPU_SET(cpuid, &mask);
    });
    auto rtn = sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    ASSERT_OR_RAISE(0 == rtn, std::runtime_error, "FAILED: sched_setaffinity.");
#endif
    // Connecting to Oracle database.
    std::string username, password, db_alias;
    ps::lib::vSplitUserid(conf_.as<std::string>("userid"), username, password, db_alias);
    BOOST_ASSERT(username.size() > 0);
    BOOST_ASSERT(password.size() > 0);
    oSvc_.reset(ps::lib::sql::occi::cSvc::oMakeSvc(
        ps::lib::sql::occi::cSvc::tTypeOfPool::iDedicated
        , username, password, db_alias, iConcurrency_, 0
    ));
    oSvc_->vShowServerVersion();
    // Shows Sqlite 3's banner.
    mos_ << oDb_.sShowVersion() << std::endl;
    /*
     * Read the value from target schema names list ("src_user").
     * Then split it by the comma, tab, or whitespace.
     * finally, Put tokens into "oWildOwners_" as ps::lib::str_vct.
     */
    static const boost::regex ex(R"([\w$_]+)");
    auto it1 = boost::make_regex_iterator(src_user_, ex);
    boost::sregex_iterator it2; // end of matching.
    ASSERT_OR_RAISE(it1 != it2
        , std::runtime_error
        , boost::format(R"(Parse error in src_user_. "%s" does not match for regex:"%s".)")
          % src_user_ % ex.str()
    );
    std::for_each(
        it1, it2, [this](boost::sregex_iterator::reference m)
        { oWildOwners_.push_back(m.str()); }
    );
    mos_ << std::string("A value of the src_user includes ") << oWildOwners_ << std::endl;
    boost::filesystem::exists(output_, ec);
    ASSERT_OR_RAISE(
        !ec
        , std::runtime_error
        , boost::format("Specified %s is not exists. Check startup parameter 'output'.")
          % output_
    );
    mos_ << boost::format("output directory is %s") % output_ << std::endl;
    /*
     * cCopyDd::vAssociate() registers cSvc instance and Sqlite3 instance.
     */
    oCopyDd_.vAssociate(oSvc_.get(), &oDb_);
    /*
     * cCopyDd::vCreateRepo() makes repository tables  on the Sqlite3 instance.
     */
    oCopyDd_.vCreateRepo();
    /*
     * Extract only the name of the actual schema in the DB.
     */
    oOwners_ = oCopyDd_.oConvEffectiveOwnerName(oWildOwners_);
    ASSERT_OR_RAISE(
        oOwners_.size() > 0
        , std::runtime_error
        , boost::format("Actual user was not found. Searched for: %s ") % oWildOwners_
    );
}

} // ps::app::xtru

} // ps::app

} // ps
