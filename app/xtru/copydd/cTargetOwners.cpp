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

namespace copydd
{

void cTargetOwners::vInitTable(ps::lib::sql::lite3::cSqliteDb& oDb)
{
    ps::lib::cTracer& trc(ps::lib::cTracer::get_mutable_instance());
    if (ps::lib::sql::lite3::nsHelper::iDoesTableNotExist("TARGET_OWNERS", oDb))
    {
        ASSERT_OR_RAISE_FNC(
            ! oDb.iExecSql(
                "CREATE TABLE TARGET_OWNERS\n"
                "( OWNER                          TEXT NOT NULL\n"
                ", CONSTRAINT PK_TARGET_OWNERS PRIMARY KEY(OWNER)\n"
                ")"
            ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb)
        );
        trc << std::string("TARGET_OWNERS was created.") << std::endl;
    }
    else
    {
        ASSERT_OR_RAISE_FNC(
            ! oDb.iExecSql(
                "DELETE FROM TARGET_OWNERS"
            ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb)
        );
        trc << std::string("TARGET_OWNERS was deleted.") << std::endl;
    }
}

cTargetOwners::cTargetOwners(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : oDb_(oDb)
    , iNumIter_(oOwners.size())
    , iSkip_(sizeof(tAttributes))
    , rTable_(0)
    , iInd_(0)
{
    vInitTable(oDb);
    rTable_ = new tAttributes[iNumIter_];
    iInd_ = new ps::lib::sql::ind_t[iNumIter_];
    ::memset(rTable_, 0, iNumIter_ * sizeof(tAttributes));
    for (size_t i = 0; i < iNumIter_; ++i)
    {
        strncpy(rTable_[i].szOwner, oOwners[i].c_str(), OBJECT_NAME_LEN);
        iInd_[i] = ps::lib::sql::ind_t::VAL_IS_NOTNULL;
    }
}

cTargetOwners::~cTargetOwners()
{
    if (rTable_) delete [] rTable_;
    if (iInd_) delete [] iInd_;
}

void cTargetOwners::vRun()
{
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "INSERT INTO TARGET_OWNERS (OWNER) "
        "SELECT USERNAME "
        "FROM ALL_USERS "
        "WHERE USERNAME LIKE ?"
    );
    ASSERT_OR_RAISE_FNC(
        oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    using ps::lib::sql::lite3::cAttr;
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(rTable_->szOwner, cAttr::STR, iInd_, iSkip_, iSkip_);
    BOOST_ASSERT(iNumIter_);
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter_) == SQLITE_DONE
            , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

ps::lib::str_vct cTargetOwners::oGetOwners()
{
    tAttributes rRowBuf;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "SELECT OWNER FROM TARGET_OWNERS"
    );
    ASSERT_OR_RAISE_FNC(
        oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwner, cAttr::STR, iInd_, iSkip_, iSkip_);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&]
        {
            oOwners_.push_back(rRowBuf.szOwner);
        }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
            , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    return oOwners_;
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

