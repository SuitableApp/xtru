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
#include <getmeta/cDescriber.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

cDescriber::cDescriber(
    ps::lib::sql::occi::cSvc& oSvc
    , const uint32_t& iBulkSize
    , const std::string& szInStmt
    , const std::string& tag
    , ps::lib::sql::occi::cBind* oIBind
    , const bool& iSkipExecute
)
    : cStmt(oSvc, iBulkSize, szInStmt, tag, oIBind)
    , stat_(ps::lib::cStat::get_mutable_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , tag_(tag)
    , iTotalBytes_(0LL)
    , iTotalRows_(0UL)
    , iSkipExecute_(iSkipExecute)
{}

cDescriber::cDescriber(
    ps::lib::sql::occi::cSvc& oSvc
    , const uint32_t& iBulkSize
    , const std::string& szInStmt
    , const std::string& tag
    , ps::lib::sql::occi::cBind* oIBind
)
    : cDescriber(oSvc, iBulkSize, szInStmt, tag, oIBind, false)
{}

cDescriber::~cDescriber()
{}

void cDescriber::vConvPlaceHolder(
    const ps::lib::str_vct& opts
){
    ps::lib::sql::occi::cStmt::vConvPlaceHolder(opts);
}

void cDescriber::vExecuteAndFetch()
{
    if (iSkipExecute_) return;
    std::exception_ptr ep = nullptr;

    ps::lib::sql::occi::cStmt::vExecute();
    vPreRepeatAction();
    ps::lib::sql::occi::cStmt::iFetch(*this, ep);
    if (iTotalRows_)
    {
        vPostRepeatAction();
    }
    else
    {
        vNotFoundAction();
    }
    vFinalizeAction();
    if (ep)
    {
        std::rethrow_exception(ep);
    }
}

void cDescriber::vAddOutputBytes(const int64_t& iOutputBytes)
{
    stat_.vAddOutputBytes(iOutputBytes);
    iTotalBytes_ += iOutputBytes;
}

void cDescriber::vAddOutputRows(const int32_t& iOutputRows)
{
    iTotalRows_ += iOutputRows;
}

void cDescriber::vShowStatValue() const
{
    const auto iMiSec = stat_.iDurationMilliSeconds();
    const auto iOutputBytes = stat_.iGetOutputBytes();
    const auto iDataRate = iMiSec ? iOutputBytes * 1000/ iMiSec : 0;
    const auto sContinuety = iGetFetchHasDone() ? (rtn_.iCotinue() ? "Done." : "CAN.") : "...";
    // Outline of statistics for console.
    cout_ << boost::format(ps::lib::sql::sFmtStatCout)
        % tag_
        % ps::lib::sIntToa(iTotalRows_)
        % ps::lib::sBinIntToIntStr(iOutputBytes)
        % (iMiSec/1000.0)
        % ps::lib::sBinIntToIntStr(iDataRate)
        % sContinuety
    << std::endl;
    // Detail of statistics for tracefile.
    trc_ << boost::format(ps::lib::sql::sFmtStatTrace)
        % tag_
        % ps::lib::sIntToa(iTotalRows_)
        % ps::lib::sIntToa(iTotalBytes_)
        % ps::lib::sBinIntToIntStr(iOutputBytes)
        % ps::lib::sIntToa(iOutputBytes)
        % (iMiSec/1000.0)
        % ps::lib::sBinIntToIntStr(iDataRate)
        % sContinuety
    << std::endl;
}

ps::lib::sql::occi::cDefine& cDescriber::oGetDefine()
{
    return ps::lib::sql::occi::cStmt::oGetDefine();
}

void cDescriber::vPreRepeatAction()
{
    // nothing to do.
}

void cDescriber::vNotFoundAction()
{
    // nothing to do.
}

void cDescriber::vFeedbackAction()
{
    vShowStatValue();
}

void cDescriber::vFinalizeAction()
{
    vShowStatValue();
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
