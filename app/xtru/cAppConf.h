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

#pragma once

namespace ps
{

namespace app
{

namespace xtru
{

class cAppConf
    : public ps::lib::cConfigures::cOptionExtender
{
private:
    ps::lib::cConfigures& conf_;
    std::string input_;
    std::string output_;
    int32_t parallelism_;
    std::string userid_;
    std::string src_user_;
    int32_t bulk_size_;
    std::string dfile_alt_dirs_;
    std::string queryfix_;
    std::string queryvar_;
    std::string queryfilename_;
    std::string filebind_;
    std::string fileexcpt_;
    std::string filefixed_;
    std::string filefkrb_;
    std::string filemeta_;
    std::string fileplsn_;
    std::string fileplsp_;
    std::string filetable_;
    std::string filetbln_;
    std::string filetblp_;
    std::string exec_load_;
    std::string exec_plus_;
    std::string extnameblob_;
    std::string extnameclob_;
    std::string extnamesql_;
    std::string findstrcmd_;
    std::string pre_rep_exec_pls_;
    std::string post_rep_exec_pls_;
    int32_t cmntlvl_;
    int32_t numdays_audit_;
    int32_t longtransit_;
    int32_t rowid_split_num_parts;
    int32_t reclength_;
    std::string sStatement_;
public:
    cAppConf(ps::lib::cConfigures& conf);
    virtual int32_t iValidate(
        std::ostream& os
        , const boost::program_options::variables_map& vm
    ) const;
};

} // ps::app::xtru

} // ps::app

} // ps


