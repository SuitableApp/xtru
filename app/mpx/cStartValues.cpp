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
#include "mpx.h"

namespace ps
{

namespace app
{

namespace mpx
{

cStartValues::cStartValues()
    : env_(ps::lib::cEnv::get_const_instance())
    , rtn_(ps::lib::cRtn::get_mutable_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , stat_(ps::lib::cStat::get_mutable_instance())
    , iParallelism_(conf_.as<int32_t>("parallelism"))
    , iNice_(conf_.as<int32_t>("nice"))
    , rule_("", " \t,", "\"")
    , sStartup_(conf_.as<boost::filesystem::path>("startup"))
    , sCsi_(conf_.as<std::string>("csi"))
    , sCso_(conf_.as<std::string>("cso"))
    , sCse_(conf_.as<std::string>("cse"))
{
    // Program names to be executed in parallel are packed in an array.
    boost::tokenizer< ps::lib::tSep > tokens_(conf_.as<std::string>("mpxlist"), rule_);
    oMaxList_.insert(tokens_.begin(), tokens_.end());
    // Even with abnormal end, program names to be ignored are packed in an array.
    tokens_.assign(conf_.as<std::string>("ignlist"), rule_);
    oIgnList_.insert(tokens_.begin(), tokens_.end());

    tokens_.assign(conf_.as<std::string>("scr_make_sh"), rule_);
    sParamValue_.assign(tokens_.begin(), tokens_.end());
    sCmdFile_ = sParamValue_[0]; // The first token is used as a script file name.
    sParamValue_.erase(sParamValue_.begin()); // The remaining tokens are given as arguments.

    for (size_t i = 0; i < sParamValue_.size(); ++i)
    {
        oTableOfReplacement_.emplace(
            (boost::format("$%d") % i).str()   // key: Prefix '$' is for identifying a placeholder
            , sParamValue_[i]                  // and value
        );
    }

    for (const auto& val: env_)
    {
        oTableOfReplacement_.emplace(
            (boost::format("$%s") % val.first).str()  // key: Prefix '$' is for environmental variable
            , val.second                              // andvalue
        );
    }

    mos_
        << boost::format("Parallel degree is %d.")
            % iParallelism_ << std::endl
        << boost::format("Nice value is %d.")
            % iNice_ << std::endl
        << boost::format("%s will be executed in parallel.")
            % oMaxList_ << std::endl
        << boost::format("Returned value from %s will be ignored.")
            % oIgnList_ << std::endl
        << boost::format(R"(Startup directory is %s.)")
            % sStartup_ << std::endl;
        if (! sCsi_.empty())
        {
            mos_ << boost::format(
                    sCsi_[0] == '@'
                    ? R"(Decrypted string will be read from "%s".)"
                    : R"(The string "%s" will send to each child STDIN.)")
                % sCsi_ << std::endl;
        }
        if (! sCso_.empty())
        {
            mos_ << boost::format(R"(STDOUT from child processes will be stored to "%s".)")
                % sCso_ << std::endl;
        }
        if (! sCse_.empty())
        {
            mos_ << boost::format(R"(STDERR from child processes will be stored to "%s".)")
                % sCse_ << std::endl;
        }
        if (sParamValue_.empty())
        {
            mos_ << boost::format(R"(Reading %s.)")
                % sCmdFile_ << std::endl;
        }
        else
        {
            mos_ << boost::format(R"(Reading %s: with args %s.)")
                % sCmdFile_ % sParamValue_ << std::endl;
        }
    ;
}

cStartValues::~cStartValues()
{}

} // ps::app::mpx

} // ps::app

} // ps
