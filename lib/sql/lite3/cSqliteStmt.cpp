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

namespace ps
{

namespace lib
{

namespace sql
{

namespace lite3
{

cSqliteStmt::cSqliteStmt(
    cSqliteDb& oDb
    , const std::string& sql
)
    : cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
    , oDb_(&oDb)
    , ite3_stmt_(nullptr)
    , sql_(sql)
    , iNumBindParam_(0)
    , iNumColumns_(0)
    , oBind_(*this)
    , oDefine_(*this)
    , szTail_(nullptr)
{}

cSqliteStmt::cSqliteStmt(
    cSqliteStmt& rhs
)
    : cout_(rhs.cout_)
    , trc_(rhs.trc_)
    , mos_(rhs.mos_)
    , oDb_(rhs.oDb_)
    , ite3_stmt_(std::move(rhs.ite3_stmt_))
    , sql_(rhs.sql_)
    , iNumBindParam_(rhs.iNumBindParam_)
    , iNumColumns_(rhs.iNumColumns_)
    , oBind_(rhs.oBind_)
    , oDefine_(rhs.oDefine_)
    , szTail_(rhs.szTail_)
{}

cSqliteStmt::~cSqliteStmt()
{}

sqlite3_stmt* cSqliteStmt::oGetHndl()
{
    return ite3_stmt_.get();
}

cSqliteDb& cSqliteStmt::oGetDb()
{
    return *oDb_;
}

cBind& cSqliteStmt::oGetBind()
{
    return oBind_;
}

cDefine& cSqliteStmt::oGetDefine()
{
    return oDefine_;
}

void cSqliteStmt::vConvPlaceHolder(
    const ps::lib::str_vct& opts
){
    BOOST_ASSERT_MSG(sql_.size() != 0, "Call CTOR function first and initialize the member sql_.");
    ps::lib::vConvPlaceHolder(opts, sql_, trc_);
}

int32_t cSqliteStmt::iParse(void)
{
    sqlite3_stmt *ptr(nullptr);
    const int32_t rc = sqlite3_prepare_v2(oDb_->oGetHndl(), sql_.c_str(), sql_.size(), &ptr, &szTail_);
    ite3_stmt_.reset(ptr);
    iNumBindParam_ = sqlite3_bind_parameter_count(ptr);
    iNumColumns_ = sqlite3_column_count(ptr);
    return rc;
}

int32_t cSqliteStmt::iExecute(
    const size_t& iNumIter
){
    BOOST_ASSERT(ite3_stmt_);
    BOOST_ASSERT(iNumBindParam_ == oBind_.iGetSize());
    BOOST_ASSERT(iNumIter > 0);
    int32_t rc = SQLITE_DONE;
    for (size_t i = 0; i < iNumIter; ++i)
    {
        oBind_.vAttachTo(i);
        rc = sqlite3_step(ite3_stmt_.get());
        if (rc != SQLITE_DONE)
        {
            break;
        }
        sqlite3_reset(ite3_stmt_.get());
    }
    return rc;
}

int32_t cSqliteStmt::iFetch(
    ps::lib::sql::cFetchable& fetchable
){
    BOOST_ASSERT(ite3_stmt_);
    BOOST_ASSERT(iNumBindParam_ == oBind_.iGetSize());
    BOOST_ASSERT(iNumColumns_ == oDefine_.iGetSize());
    int32_t rc;
    size_t iTotalRows = 0;
    oBind_.vAttachTo(ps::lib::iZeroItem);
    while (SQLITE_ROW == (rc = sqlite3_step(ite3_stmt_.get())))
    {
        if (0 == iTotalRows) fetchable.vPreRepeatAction();
        oDefine_.vAcquire(0);
        fetchable.vPostBulkAction(ps::lib::iOneItem);
        ++iTotalRows;
    }
    if (rc == SQLITE_DONE)
    {
        if (iTotalRows)
        {
            fetchable.vPostRepeatAction();
        }
        else
        {
            fetchable.vNotFoundAction();
        }
    }
    fetchable.vFinalizeAction();
    return rc;
}

} // ps::lib::sql::lite3

} // ps::lib::sql

} // ps::lib

} // ps

