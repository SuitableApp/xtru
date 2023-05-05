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
#include "cAppConf.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace po = boost::program_options;

cAppConf::cAppConf(ps::lib::cConfigures& conf)
    : conf_(conf)
{
    conf_.vSetAuxConf(this);
    // Options from conf file or command line arguments.
    conf_.oGetOptionsDesc().add_options()
    ("input"
         , po::value<std::string>(&input_)
            ->default_value("input")
                ->value_name("dir")
         , "dir is a directory name for the input files."
           " Candidate path is either absolute or relative to the current directory."
        )
    ("output"
         , po::value<std::string>(&output_)
            ->default_value("output")
                ->value_name("dir")
         , "dir is a directory name for the output files."
           " Candidate path is either absolute or relative to the current directory."
        )
    ("feedback"
         , po::value<int32_t>()
            ->default_value(100)
                ->value_name("N")
         , "")
    ("parallelism"
        , po::value<int32_t>(&parallelism_)
            ->default_value(conf.iGetNumPhisicalCores())
                ->value_name("N")
         , "N is a positive integer. Set a concurrency number.")
    ("prc_affinity_mask"
         , po::value<std::string>()
            ->default_value("-")
                ->value_name("CSV")
         , "A CSV containing one or more integers as well as two "
           "integers tied with one negative sign (ie integer ranges).")
    ("bulk_size"
        , po::value<int32_t>(&bulk_size_)
            ->default_value(1000)
                ->value_name("N")
         , "N is a positive integer. Set a number of array size.")
    ("userid,u"
        , po::value<std::string>(&userid_)
            ->default_value("SYSTEM/MANAGER")
                ->value_name("<user>/<pass>[@<alias>]")
        , "Set the connect string.")
    ("feature"
         , po::value<std::string>()
            ->default_value("unload")
                ->value_name("name")
        , "Select one from 'unload', 'query' or 'getmeta'.")
    ("src_user"
         , po::value<std::string>(&src_user_)
            ->default_value("SCOTT")
                ->value_name("schema_name")
         , "Set the target schema name.")
    ("repos_fn"
         , po::value<std::string>()
            ->default_value(":memory:")
                ->value_name("database file name (SQLite3)")
         , "Set the filename.if ommited then using In-memory database.")
    ("dst_info"
         , po::value<std::string>()
            ->default_value("SCOTT/TIGER")
                ->value_name("<user>/<pass>[@<alias>]")
         , "Specify the connection string of re-load the destination database.")
    ("alter_dest"
         , po::value<std::string>()
         , "")
    ("listexcpt"
         , po::value<std::string>()
         , "")
    ("listfixed"
         , po::value<std::string>()
         , "")
    ("listtable"
         , po::value<std::string>()
         , "")
    ("min_bulk_size"
         , po::value<int32_t>()
         , "")
    ("minimum_free_size"
         , po::value<std::string>()
         , "")
    ("auxiliary_dest"
         , po::value<std::string>()
         , "")
    ("dfile_alt_dirs"
         , po::value<std::string>()
         , "")
    ("stdout"
         , po::value<std::string>()
         , "")
    ("stream_locator"
         , po::value<std::string>()
            ->default_value("file://{O}/{C}.{X}")
                ->value_name("locator")
         , "")
    ("queryfix"
         , po::value<std::string>(&queryfix_)
            ->default_value("qryfix")
                ->value_name("name")
         , "")
    ("queryvar"
         , po::value<std::string>(&queryvar_)
            ->default_value("qryvar")
                ->value_name("name")
         , "")
    ("queryfilename"
         , po::value<std::string>(&queryfilename_)
            ->default_value("*")
                ->value_name("name")
         , "")
    ("filebind"
         , po::value<std::string>(&filebind_)
            ->default_value("bind.dat")
                ->value_name("name")
         , "")
    ("fileexcpt"
         , po::value<std::string>(&fileexcpt_)
            ->default_value("excpt.dat")
                ->value_name("name")
         , "")
    ("filefixed"
         , po::value<std::string>(&filefixed_)
            ->default_value("fixed.dat")
                ->value_name("name")
         , "")
    ("filefkrb"
         , po::value<std::string>(&filefkrb_)
            ->default_value("fkrb.sql")
                ->value_name("name")
         , "")
    ("filemeta"
         , po::value<std::string>(&filemeta_)
            ->default_value("meta.sql")
                ->value_name("name")
         , "name refers to the filename when saving the output result of getmeta function. "
           "When the directory name is omitted, it is saved in output. "
           "Optionally a directory name can be prefixed to name.")
    ("fileplsn"
         , po::value<std::string>(&fileplsn_)
            ->default_value("plsn.txt")
                ->value_name("name")
         , "")
    ("fileplsp"
         , po::value<std::string>(&fileplsp_)
            ->default_value("plsp.txt")
                ->value_name("name")
         , "")
    ("filesize"
         , po::value<std::string>()
         , "")
    ("filetable"
         , po::value<std::string>(&filetable_)
            ->default_value("table.dat")
                ->value_name("name")
         , "")
    ("filetbln"
         , po::value<std::string>(&filetbln_)
            ->default_value("tbln.txt")
                ->value_name("name")
         , "")
    ("filetblp"
         , po::value<std::string>(&filetblp_)
            ->default_value("tblp.txt")
                ->value_name("name")
         , "")
    ("loadmethod"
         , po::value<std::string>()
         , "")
    ("dependent"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "[true|yes|on|1] Object definition order is based on object mutual dependency.")
    ("compress"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "[true|yes|on|1] In the representation of storage attributes of segments, "
           "extra extents are collectively defined as initial extents.")
    ("metadiv"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "[true|yes|on|1] The output divides for each object, if value is true.")
    ("cmntlvl"
         , po::value<int32_t>(&cmntlvl_)
            ->default_value(1)
                ->value_name("N")
         , "N is 0,1 or 2. Regulates the output of comments added to tables and views.")
    ("longtransit"
         , po::value<int32_t>(&longtransit_)
            ->default_value(0)
                ->value_name("N")
         , "N is 0,1 or 2. Regulates the policy of migrating values of LONG or LONG RAW type. ")
    ("numdays_audit"
         , po::value<int32_t>(&numdays_audit_)
            ->default_value(-1)
                ->value_name("N")
         , "N is -1 or more and less or equal INT32_MAX. Set a positive number of days to go back to "
           "check the audit trail. When -1 is given, it can suppress to be listed the trail.")
    ("skipscr"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "[true|yes|on|1] Ommit to output scripts.")
    ("rows"
         , po::value<std::string>()
         , "")
    ("num_rows"
         , po::value<int32_t>()
         , "")
    ("exec_load"
         , po::value<std::string>(&exec_load_)
            ->default_value("sqlldr")
                ->value_name("name")
         , "")
    ("exec_plus"
         , po::value<std::string>(&exec_plus_)
            ->default_value("sqlplus")
                ->value_name("name")
         , "")
    ("discrete"
         , po::value<std::string>()
         , "")
    ("sorting"
         , po::value<std::string>()
         , "")
    ("diralias"
         , po::value<std::string>()
         , "")
    ("consistent"
         , po::value<std::string>()
         , "")
    ("extnameblob"
         , po::value<std::string>(&extnameblob_)
            ->default_value("blo")
                ->value_name("name")
         , "")
    ("extnameclob"
         , po::value<std::string>(&extnameclob_)
            ->default_value("clo")
                ->value_name("name")
         , "")
    ("extnamesql"
         , po::value<std::string>(&extnamesql_)
            ->default_value("sql")
                ->value_name("name")
         , "")
    ("wintarget"
         , po::value<std::string>()
         , "")
    ("mngscrout"
         , po::value<std::string>()
         , "")
    ("sort_area_size"
         , po::value<int32_t>()
         , "")
    ("optimizer_mode"
         , po::value<std::string>()
         , "")
    ("nmencls"
         , po::value<std::string>()
         , "")
    ("findstrcmd"
         , po::value<std::string>(&findstrcmd_)
            ->default_value("grep")
                ->value_name("name")
         , "")
    ("sqlfmt"
         , po::value<std::string>()
         , "")
    ("rerunpoint"
         , po::value<std::string>()
         , "")
    ("getexttbl"
         , po::value<std::string>()
         , "")
    ("bind_size"
         , po::value<std::string>()
         , "")
    ("fbqscn"
         , po::value<int32_t>()
         , "")
    ("fbqtime"
         , po::value<std::string>()
         , "")
    ("printcolid"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "[true|yes|on|1] true prints column number commented out in front of column name")
    ("pre_rep_exec_pls"
         , po::value<std::string>(&pre_rep_exec_pls_)
            ->default_value("")
                ->value_name("name")
         , "")
    ("post_rep_exec_pls"
         , po::value<std::string>(&post_rep_exec_pls_)
            ->default_value("")
                ->value_name("name")
         , "")
    ("merge_lobs_into_sdf"
         , po::value<std::string>()
         , "")
    ("io_overlap_scale"
         , po::value<int32_t>()
         , "")
    ("overlap_buffer_length"
         , po::value<std::string>()
         , "")
    ("db_file_multiblock_read_count"
         , po::value<int32_t>()
         , "")
    ("partitioning"
         , po::value<int32_t>()
         , "")
    ("optimizer_hints"
         , po::value<std::string>()
         , "")
    ("rowid_split_num_parts"
        , po::value<int32_t>(&rowid_split_num_parts)
            ->default_value(4)
                ->value_name("N")
         , "N is a positive integer. Set a number of peice of splitting by rowid.")
    ("rowid_split_min_size"
         , po::value<std::string>()
            ->default_value("0M")
                ->value_name("[1-9][0-9]*[.kMGTP]{0,1}")
         , "Specify a minimum size of the segment that be splitted by rowid range. This will be disabled by zero.")
    ("file_mapping"
         , po::value<int32_t>()
         , "")
    ("priority_class"
         , po::value<std::string>()
         , "")
    ("thread_priority"
         , po::value<std::string>()
         , "")
    ("num_asynclobs"
         , po::value<int32_t>()
         , "")
    ("charsetid"
         , po::value<int32_t>()
         , "")
    ("nobyteordermark"
         , po::value<bool>()
            ->default_value(false)
                ->value_name("boolean")
         , "")
    ("number2bcd"
         , po::value<int32_t>()
         , "")
    ("charsetform"
         , po::value<int32_t>()
         , "")
    ("reversebyteorder"
         , po::value<std::string>()
         , "")
    ("client_id"
         , po::value<std::string>()
         , "")
    ("serveroutput"
         , po::value<std::string>()
         , "")
    ("wait_sync_interval"
         , po::value<int32_t>()
         , "")
    ("wait_sync_maxretry"
         , po::value<int32_t>()
         , "")
    ("minidump_type"
         , po::value<std::string>()
         , "")
    ("protect_from"
         , po::value<std::string>()
         , "")
    ("showstmt"
         , po::value<std::string>()
         , "")
    ("showcltdtm"
         , po::value<std::string>()
         , "")
    ("showstat"
         , po::value<std::string>()
         , "")
    ("showvers"
         , po::value<std::string>()
         , "")
    ("executable"
         , po::value<std::string>()
         , "")
    ("exec_mpx"
         , po::value<std::string>()
         , "")
    ("maxlongsize"
         , po::value<int32_t>()
            ->default_value(1000)
         , "")
    ("date_mask"
         , po::value<std::string>()
            ->default_value("YYYYMMDDHH24MISS")
                ->value_name("mask")
         , "mask is a form that applies when expressing a DATE type value as a string.")
    ("timestamp_mask"
         , po::value<std::string>()
            ->default_value("YYYYMMDDHH24MISSXFF")
                ->value_name("mask")
         , "mask is a form that applies when expressing a TIMESTAMP or TIMESTAMP WITH LOCAL "
           "TIMEZONE type value as a string.")
    ("timestamp_tz_mask"
         , po::value<std::string>()
            ->default_value("YYYYMMDDHH24MISSXFFTZHTZM")
                ->value_name("mask")
         , "mask is a form that applies when expressing a TIMESTAMP WITH TIMEZONE type "
           "value as a string.")
    ("events_10046"
         , po::value<std::string>()
         , "")
    ("embed_column_name"
         , po::value<std::string>()
         , "")
    ("terminator"
         , po::value<std::string>()
            ->default_value(",")
                ->value_name("string")
         , "String that is used to separate the column.")
    ("encloser"
         , po::value<std::string>()
            ->default_value("~~")
                ->value_name("string")
         , "String to be used to enclose begining for str-value.")
    ("encloser2"
         , po::value<std::string>()
            ->value_name("string")
         , "String to be used to enclose ending for str-value.")
    ("reclength"
        , po::value<int32_t>(&reclength_)
            ->default_value(10)
                ->value_name("N")
         , "Place a data length field of N digits in front of each records.")
    ("endterm"
         , po::value<bool>()
         , "")
    ("suppress_ctrlf"
         , po::value<bool>()
         , "")
    ("usualpath"
         , po::value<bool>()
         , "")
    ("recoverable"
         , po::value<bool>()
         , "")
    ("fpnumfmt"
         , po::value<std::string>()
         , "")
    ("taf_delay_time"
         , po::value<int32_t>()
            ->default_value(60)
                ->value_name("N")
         , "")
    ("rowseparator"
         , po::value<std::string>()
            ->default_value("\\n")
                ->value_name("string")
         , "String used to separate the record")
    ("dataext"
         , po::value<std::string>()
            ->default_value("dat")
                ->value_name("string")
         , "")
    ("scr_make_sh"
         , po::value<std::string>(&sStatement_)
            ->default_value("_make.sh")
                ->value_name("file_name")
         , "Specify the name of the script file to reload. ")
    ("par_par0_txt"
         , po::value<std::string>()
            ->default_value("_par0.txt")
                ->value_name("file_name")
         , "Specify the name of the parameter file for SQL*Loader.")
    ("par_par1_txt"
         , po::value<std::string>()
            ->default_value("_par1.txt")
                ->value_name("file_name")
         , "Specify the name of the parameter file for SQL*Loader.")
    ("scr_conn_sql"
         , po::value<std::string>()
            ->default_value("_conn.sql")
                ->value_name("file_name")
         , "Specify the name of the script file to connect database using SQL*Plus.")
    ("scr_wait_here_sh"
         , po::value<std::string>()
            ->default_value("_wait_here.sh")
                ->value_name("file_name")
         , "Specify the name of the script file to synchronize. ")
    ("scr_disable_deps_sql"
         , po::value<std::string>()
            ->default_value("_disable_deps.sql")
                ->value_name("file_name")
         , "Specify the name of the script file to disabled schema objects using SQL*Plus.")
    ("scr_enable_deps_sql"
         , po::value<std::string>()
            ->default_value("_enable_deps.sql")
                ->value_name("file_name")
         , "Specify the name of the script file to enabled schema objects using SQL*Plus.")
    ;
    // Preparation for reading nameless options from command arguments.
    conf_.oGetPositionalDesc()
        .add("feature", 1)
    ;
}

int32_t cAppConf::iValidate(
    std::ostream& os
    , const boost::program_options::variables_map& vm
) const
{
    BOOST_ASSERT(vm.size() > 0);
    int32_t iErrors = 0;
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "input", !input_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "output", !output_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "parallelism", parallelism_ > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "userid", !userid_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "bulk_size", bulk_size_ > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "queryfix", !queryfix_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "queryvar", !queryvar_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "queryfilename", !queryfilename_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "filebind", !filebind_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "filefkrb", !filefkrb_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "filemeta", !filemeta_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "fileplsn", !fileplsn_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "fileplsp", !fileplsp_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "filetbln", !filetbln_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "filetblp", !filetblp_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "exec_load", !exec_load_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "exec_plus", !exec_plus_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "extnameblob", !extnameblob_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "extnameclob", !extnameclob_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "extnamesql", !extnamesql_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "findstrcmd", !findstrcmd_.empty());
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "cmntlvl", cmntlvl_ >= 0 && cmntlvl_ <= 2);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "numdays_audit", numdays_audit_ >= -1);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "longtransit", longtransit_ >= 0 && longtransit_ <= 2);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "rowid_split_num_parts", rowid_split_num_parts > 0);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "reclength", reclength_ >= 0 && reclength_ <= 10);
    iErrors += PS_UTL_CCONFIGURES_VALIDATE(os, vm, "scr_make_sh", !sStatement_.empty());
    return iErrors;
}

} // ps::app::xtru

} // ps::app

} // ps

