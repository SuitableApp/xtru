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

namespace app
{

namespace xtru
{

namespace getdata
{

class cStartValues
    : virtual protected cBootstrap
{
protected:
    enum { SCHEMA_BASED_PARTITIONING=0x1, ROWID_BASED_PARTITIONING=0x2 } tPartitioningMethod;
    /// A queue for the tasks of unloading (or executing query).
    ps::lib::tSequence<ps::lib::sql::cFetchable> unldrs_;
    const boost::filesystem::path
        input_, queryfix_, queryvar_
        , dfile_alt_dirs_
        , exec_load_  /// executable of SQL*Loader
        , exec_plus_  /// executable of SQL*Plus
        , extnamesql_, sFindstrCmd_
        , filebind_, fileexcpt_, filefixed_, filetable_
        , pre_rep_exec_pls_, post_rep_exec_pls_
    ;
    const std::string dataext_, extnameclob_, extnameblob_;
    const std::string queryfilename_;
    int32_t stdout_;  /// 1-bit for the data file, 2-bit for the control file.
                      /// 3-bit or upper are not in used.
    const std::string stream_locator_;
    const bool suppress_ctrlf_;
    const int partitioning_;
    const std::string sMngScrOut_;
    int32_t iMngScrOut_;
    const boost::filesystem::fstream::ios_base::openmode mode_;
    const boost::filesystem::path
        make_sh_        ///< content of _make.sh
        , par0_txt_     ///< content of _par0.txt
        , par1_txt_     ///< content of _par1.txt
        , conn_sql_     ///< content of _conn.sql
        , wait_here_    ///< content of _wait_here.sh
        , disable_deps_ ///< content of _disable_deps.sql
        , enable_deps_  ///< content of _enable_deps.sql (Query only)
    ;
    const std::string sSuffixDeps_;
    const std::string sSuffixRefs_;
    cStartValues();
    ~cStartValues();
    /**
     * @brief
     * @param[in] iCondition
     *  -# true: conventional path load
     *  -# false: direct path load
     * @return
     *  -# par0_txt_ when true is passed to iCondition.
     *  -# par1_txt_ when false is passed to iCondition.
     */
    const boost::filesystem::path& sGetParfName(const bool& iCondition);
    /**
     * @brief
     * @return
     *  -# stream for null device when true is passed to iIsNullDevice.
     *  -# stream for make_sh_ when false is passed to iIsNullDevice.
     */
    ps::lib::tPtrFstream oGetStreamToMakeSh(
        const boost::filesystem::path output
        , const bool iIsNullDevice
    );
};

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps

