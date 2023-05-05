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
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @details
 */
cScriptMaker::cScriptMaker(const boost::filesystem::path output)
    : output_(output)
{}

/**
 * @details
 */
cScriptMaker::~cScriptMaker()
{}

/**
 * @details
 */
void cScriptMaker::vOutputScriptsRegular(
    const boost::filesystem::fstream::ios_base::openmode& mode
    , const boost::filesystem::path& make_sh
    , const boost::filesystem::path& par0_txt
    , const boost::filesystem::path& par1_txt
    , const boost::filesystem::path& conn_sql
    , const boost::filesystem::path& wait_here
){
    static const int32_t iReadSize = (10 << 20); // Read buffere size (unit: bytes)
    static const int32_t iBindSize = (10 << 20); // Bind array size (unit: bytes) 
    /// To refer to parameters at startup.
    const ps::lib::cConfigures& conf_ = ps::lib::cConfigures::get_const_instance();
    /// refers to the value of parameter 'dst_info' at startup.
    const std::string dst_info_ = conf_.as<std::string>("dst_info");
    std::string sUserid, sConnect;
    std::ostringstream oss;
    if (dst_info_[0] == '@')
    {
        sUserid = "";

        oss
            << std::string("accept credential hide prompt ")
            << std::string("'Imput the connect string. [name/pwd@dbalias]: '")
            << std::endl
            << std::string("connect &credential")
            << std::endl
            ;
        sConnect = oss.str();
        oss.str("");
    }
    else
    {
        oss
            << boost::format("userid=%s") % dst_info_ << std::endl;
        sUserid = oss.str();
        oss.str("");

        oss
            << boost::format("connect %s") % dst_info_ << std::endl;
        sConnect = oss.str();
        oss.str("");
    }
    {
        auto st_par0_txt = ps::lib::open_with_check(output_ / par0_txt, mode);
        *st_par0_txt
            << sUserid
            << "silent=(header)" << std::endl
            << "errors=1" << std::endl
            << "direct=true" << std::endl
            << "readsize=" << iReadSize << std::endl
            ;
    }
    {
        auto st_par1_txt = ps::lib::open_with_check(output_ / par1_txt, mode);
        *st_par1_txt
            << sUserid
            << "silent=(header)" << std::endl
            << "errors=1" << std::endl
            << "direct=false" << std::endl
            << "bindsize=" << iBindSize << std::endl
            << "readsize=" << iReadSize << std::endl
            ;
    }
    {
        auto st_conn_sql = ps::lib::open_with_check(output_ / conn_sql, mode);
        *st_conn_sql
            << "set linesize 250 pagesize 200 verify off feedback off"
               << " trimspool on tab off echo on sqlprompt '' termout on" << std::endl
            << "whenever oserror exit failure" << std::endl
            << "whenever sqlerror exit sql.sqlcode" << std::endl
            << sConnect
            << "whenever sqlerror continue" << std::endl
            ;
    }
    {
        auto st_wait_here = ps::lib::open_with_check(output_ / wait_here, mode);
        *st_wait_here
            << "#!/bin/bash" << std::endl
            << "# " << wait_here  << " exists only in order to cause"
            << " the synchronization of parallel operation." << std::endl
            << std::string(
                "# Since referential integrity depends on"
                " the enforcement of unique key constraints."
            ) << std::endl
            ;
    }
    {
        cChmod obj(
            output_
            , boost::filesystem::add_perms
            | boost::filesystem::owner_exe
            | boost::filesystem::group_write
            | boost::filesystem::group_exe
            );
        obj(make_sh)(wait_here);
    }
    {
        cChmod obj(
            output_
            , boost::filesystem::add_perms
            | boost::filesystem::group_write 
            );
        obj(par0_txt)(par1_txt)(conn_sql);
    }
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
