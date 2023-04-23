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

namespace ps 
{

namespace lib
{

namespace sql
{

cTransporter::cTransporter(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const uint32_t& iBulkSize
    , const std::string& szInStmt
    , const std::string& tag
    , ps::lib::sql::occi::cBind* oIBind
    , const std::string& szOutStmt
)
    : cStmt(oSvc, iBulkSize, szInStmt, tag, oIBind)
    , cSqliteStmt(oDb, szOutStmt)
    , stat_(ps::lib::cStat::get_mutable_instance())
    , tag_(tag)
    , iTotalBytes_(0LL)
    , iTotalRows_(0UL)
    , oOBind_(this->oGetBind())
{
    ASSERT_OR_RAISE_FNC(
        this->iParse() == SQLITE_OK
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb)
    );
}

cTransporter::~cTransporter()
{}

void cTransporter::vExecuteAndFetch()
{
    ps::lib::sql::occi::cStmt::vExecute();
    vPreRepeatAction();
    ps::lib::sql::occi::cStmt::iFetch(*this);
    if (iTotalRows_)
    {
        vPostRepeatAction();
    }
    else
    {
        vNotFoundAction();
    }
    vFinalizeAction();
}

void cTransporter::vAddOutputBytes(const int64_t& iOutputBytes)
{
    stat_.vAddOutputBytes(iOutputBytes);
    iTotalBytes_ += iOutputBytes;
}

void cTransporter::vAddOutputRows(const int32_t& iOutputRows)
{
    iTotalRows_ += iOutputRows;
}

void cTransporter::vShowStatValue() const
{
    auto& cout = ps::lib::cConsole::get_mutable_instance();
    auto& trc = ps::lib::cTracer::get_mutable_instance();
    const auto iMiSec = stat_.iDurationMilliSeconds();
    const auto iOutputBytes = stat_.iGetOutputBytes();
    const auto iDataRate = iMiSec ? iOutputBytes * 1000/ iMiSec : 0;
    const auto sContinuety = iGetFetchHasDone() ? (rtn_.iCotinue() ? "Done." : "CAN.") : "...";
    // Outline of statistics for console.
    cout << boost::format(ps::lib::sql::sFmtStatCout)
        % tag_
        % ps::lib::sIntToa(iTotalRows_)
        % ps::lib::sBinIntToIntStr(iOutputBytes)
        % (iMiSec/1000.0)
        % ps::lib::sBinIntToIntStr(iDataRate)
        % sContinuety
    << std::endl;
    // Detail of statistics for tracefile.
    trc << boost::format(ps::lib::sql::sFmtStatTrace)
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

void cTransporter::vFeedbackAction()
{
    vShowStatValue();
}

void cTransporter::vFinalizeAction()
{
    vShowStatValue();
}

} // ps::lib::sql

} // ps::lib

} // ps

