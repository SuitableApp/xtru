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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

std::ostream& operator<<(std::ostream& os, const cNumObjs& rhs)
{
    return rhs.vRptBanner(os);
}

void cNumObjs::vSetValues(
    const int32_t& iDlns
    , const int32_t& iSeqs
    , const int32_t& iType
    , const int32_t& iPckg
    , const int32_t& iFunc
    , const int32_t& iProc
    , const int32_t& iClus
    , const int32_t& iTabs
    , const int32_t& iChds
    , const int32_t& iSnps
    , const int32_t& iRefs
    , const int32_t& iView
    , const int32_t& iTypb
    , const int32_t& iPkgb
    , const int32_t& iSyns
    , const int32_t& iJobs
){
    iDlns_ = iDlns;
    iSeqs_ = iSeqs;
    iType_ = iType;
    iPckg_ = iPckg;
    iFunc_ = iFunc;
    iProc_ = iProc;
    iClus_ = iClus;
    iTabs_ = iTabs;
    iChds_ = iChds;
    iSnps_ = iSnps;
    iRefs_ = iRefs;
    iView_ = iView;
    iTypb_ = iTypb;
    iPkgb_ = iPkgb;
    iSyns_ = iSyns;
    iJobs_ = iJobs;
}

std::ostream& cNumObjs::vRptBanner(std::ostream& os) const
{
    const int64_t iLastPos = os.tellp();
    // register the top of the header in preparation for a later call.
    const static int64_t iPosHeader_ = iLastPos;
    // seek to the top of the header.
    os.seekp(iPosHeader_, std::ios::beg);
    // output header.
    os
        << boost::format(" * Number of db links       (Dlns) = %13s") % iDlns_ << std::endl
        << boost::format(" * Number of sequences      (Seqs) = %13s") % iSeqs_ << std::endl
        << boost::format(" * Number of types          (Type) = %13s") % iType_ << std::endl
        << boost::format(" * Number of packages       (Pckg) = %13s") % iPckg_ << std::endl
        << boost::format(" * Number of functions      (Func) = %13s") % iFunc_ << std::endl
        << boost::format(" * Number of procedures     (Proc) = %13s") % iProc_ << std::endl
        << boost::format(" * Number of clusters       (Clus) = %13s") % iClus_ << std::endl
        << boost::format(" * Number of all tables     (Tabs) = %13s") % iTabs_ << std::endl
        << boost::format(" * Number of snapshots      (Snps) = %13s") % iSnps_ << std::endl
        << boost::format(" * Number of child tables   (Chds) = %13s") % iChds_ << std::endl
        << boost::format(" * Number of refresh groups (Refs) = %13s") % iRefs_ << std::endl
        << boost::format(" * Number of views          (View) = %13s") % iView_ << std::endl
        << boost::format(" * Number of type bodies    (Typb) = %13s") % iTypb_ << std::endl
        << boost::format(" * Number of package bodies (Pkgb) = %13s") % iPkgb_ << std::endl
        << boost::format(" * Number of synonyms       (Syns) = %13s") % iSyns_ << std::endl
        << boost::format(" * Number of job queues     (Jobs) = %13s") % iJobs_ << std::endl
    ;
    // resume up.
    if (iPosHeader_  < iLastPos)
    {
        os.seekp(iLastPos, std::ios::beg);
    }
    return os;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

