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
#include "nsApp.h"
#include "cAppConf.h"

namespace ps
{

namespace app
{

namespace mpx
{

namespace po = boost::program_options;

cAppConf::cAppConf(ps::lib::cConfigures& conf)
    : conf_(conf)
{
    conf_.vSetAuxConf(this);
    conf_.oGetOptionsDesc().add_options()
    // Options from conf file or command line arguments.
    ("parallelism,p"
        , po::value<int32_t>(&iParallelism_)
            ->default_value(conf.iGetNumPhisicalCores())
                ->value_name("nteger_as_parallel_degree")
         , "integer_as_parallel_degree is a positive integer. "
           "Set a concurrency number. An integer from 1 to 64 is valid.")
    ("nice,n"
        , po::value<int32_t>(&iNice_)
            ->default_value(0)
                ->value_name("integer_as_iNice_degree")
         , "integer_as_iNice_degree is a integer. "
           "Set an integer between -20 and +19.")
    ("mpxlist,m"
        , po::value<std::string>(&sMpxlist_)
            ->default_value("sqlldr,sqlplus")
                ->value_name("m_exe1[,m_exe2]...")
        , "Set one or more command name to be started in parallel. "
          "If there are two or more, separate them with commas.")
    ("ignlist,x"
         , po::value<std::string>(&sIgnlist_)
            ->default_value("grep")
                ->value_name("i_exe1[,i_exe2]...")
        , "Set one or more command names, to ignore even if an error occurs. "
          "If there are two or more, separate them with commas.")
    ("startup,d"
         , po::value<boost::filesystem::path>()
            ->default_value(boost::filesystem::initial_path())
                ->value_name("sStartup_dir")
         , "Set the start directory to execute the script file. "
           "If omitted, the current directory is specified. "
           "The argument of this switch is the search place "
           "when only the file name is specified as the argument "
           "of each switch of --csi, --cso, --cse and --cmd.")
    ("csi"
         , po::value<std::string>(&sCsi_)
            ->default_value("")
                ->value_name("string_for_send_to_child_proc_stdin")
         , "string_for_send_to_child_proc_stdin can be given a string to enter "
           "when starting a process with an input specification that accepts data "
           "from standard input. If the first byte of the character string specified "
           "in string_for_send_to_child_proc_stdin is '@', MPX operates with the "
           "following character string as the path to the file containing "
           "the credential information.")
    ("cso"
         , po::value<std::string>(&sCso_)
            ->default_value(cStdOut::file_)
                ->value_name("filename_for_recieve_from_child_proc_stdout")
         , "filename_for_recieve_from_child_proc_stdout can specify the name "
           "of the log file that stores the message when starting a process "
           "(console application) that displays messages to standard output. "
           "When only filename is specified for "
           "filename_for_recieve_from_child_proc_stdout, "
           "the search place is the place specified for --startup.")
    ("cse"
         , po::value<std::string>(&sCse_)
            ->default_value(cStdErr::file_)
                ->value_name("filename_for_recieve_from_child_proc_stderr")
         , "filename_for_recieve_from_child_proc_stderr can specify the name "
           "of the log file that stores the message when starting a process "
           "(console application) that displays messages to standard error. "
           "When only filename is specified for "
           "filename_for_recieve_from_child_proc_stderr, "
           "the search place is the place specified for --startup.")
    ("scr_make_sh,s"
         , po::value<std::string>(&sMakeSh_)
            ->default_value("_make.sh")
                ->value_name(R"("cmd_file_name [arg]...")")
         , "For cmd_file_name, specify the name of the script file to be executed. "
           "To include a relative directory, specify the relative position "
           "from the current directory when MPX started, regardless of the setting "
           "of --startup. Absolute path can also be specified. The search place "
           "when specifying only the file name to cmd_file_name is the same "
           "as the argument of --startup.")
    ;
    // Preparation for reading nameless options from command arguments.
    conf_.oGetPositionalDesc()
        .add("cmd", 1)
    ;
}

int32_t cAppConf::iValidate(
    std::ostream& os
    , const boost::program_options::variables_map& vm
) const
{
    BOOST_ASSERT(vm.size() > 0);
    int32_t iErrors = 0;
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "parallelism", iMaxParalellism >= iParallelism_ && iParallelism_ > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "nice", iMaxNice >= iNice_ && iNice_ >= iMinNice);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "scr_make_sh", !sMakeSh_.empty());
    return iErrors;
}

} // ps::app::mpx

} // ps::app

} // ps

