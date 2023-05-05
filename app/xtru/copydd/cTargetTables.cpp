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

const char cTargetTables::szCreStmt[] = {
/* Old name is APLDDT01 */
"CREATE TABLE TARGET_TABLES\n"
"( OWNER        TEXT NOT NULL\n"
", TABLE_NAME   TEXT NOT NULL\n"
", IOT_TYPE     TEXT\n"
", DATA_FMT     INT DEFAULT 0 NOT NULL\n"
", NUM_LONGS    INT DEFAULT 0 NOT NULL\n" // >0: indicates to involve LONG or LONG RAW.
", NUM_LOBS     INT DEFAULT 0 NOT NULL\n" // >0: indicates to involve LOB.
", CONSTRAINT PK_TARGET_TABLES PRIMARY KEY\n"
    "( OWNER, TABLE_NAME\n"
    ")\n"
", CONSTRAINT CK_TARGET_TABLES_01 CHECK(DATA_FMT in (0, 1))\n"
")"
};

const char cTargetTables::szDelStmt[] = {
"DELETE FROM TARGET_TABLES"
};

int32_t cTargetTables::iAllocAndSet(const ps::lib::str_vct& oTables)
{
    const auto& iNumIter = oTables.size();
    if (rTable_) delete [] rTable_;
    if (iInd_) delete [] iInd_;
    rTable_ = new tAttributes[iNumIter];
    iInd_ = new ps::lib::sql::ind_t[iNumIter];
    ::memset(rTable_, 0, iNumIter * sizeof(tAttributes));
    for (auto i = 0LU; i < iNumIter; ++i)
    {
        strncpy(rTable_[i].szTable, oTables[i].c_str(), OBJECT_NAME_LEN);
        iInd_[i] = ps::lib::sql::ind_t::VAL_IS_NOTNULL;
    }
    return iNumIter;
}

void cTargetTables::vInsItems(const ps::lib::str_vct& oTables)
{
    const auto iNumIter = iAllocAndSet(oTables);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "INSERT INTO TARGET_TABLES "
        "(OWNER "
        ", TABLE_NAME"
        ", IOT_TYPE"
        ") SELECT t1.OWNER AS OWNER "
        ", t1.TABLE_NAME AS TABLE_NAME "
        ", t1.IOT_TYPE AS IOT_TYPE "
        "FROM TARGET_OWNERS t0 "
        ", ALL_TABLES t1 "
        "WHERE t1.OWNER = t0.OWNER "
        "AND t1.TABLE_NAME LIKE ? ESCAPE '\\' "
        "AND NOT EXISTS ( "
            "SELECT null FROM TARGET_TABLES t2 "
            "WHERE t2.OWNER = t1.OWNER "
            "AND t2.TABLE_NAME = t1.TABLE_NAME "
        ") "
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    using ps::lib::sql::lite3::cAttr;
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(rTable_->szTable, cAttr::STR, iInd_, iSkip_, iSkip_);
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    // Optional operation.
}

void cTargetTables::vChgItems(const ps::lib::str_vct& oTables)
{
    const auto iNumIter = iAllocAndSet(oTables);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "UPDATE TARGET_TABLES "
        "SET DATA_FMT = 1 "
        "WHERE TABLE_NAME LIKE ? ESCAPE '\\' "
        "AND DATA_FMT = 0 "
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    using ps::lib::sql::lite3::cAttr;
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(rTable_->szTable, cAttr::STR, iInd_, iSkip_, iSkip_);
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

void cTargetTables::vDelItems(const ps::lib::str_vct& oTables)
{
    const auto iNumIter = iAllocAndSet(oTables);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "DELETE FROM TARGET_TABLES "
        "WHERE TABLE_NAME LIKE ? ESCAPE '\\' "
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    using ps::lib::sql::lite3::cAttr;
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(rTable_->szTable, cAttr::STR, iInd_, iSkip_, iSkip_);
    ASSERT_OR_RAISE_FNC(oStmt.iExecute(iNumIter) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

void cTargetTables::vDelOptionals()
{
    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "DELETE FROM TARGET_TABLES "
            "WHERE EXISTS ( "
                "SELECT null FROM ALL_MVIEWS t1 "
                "WHERE OWNER = t1.OWNER "
                "AND TABLE_NAME = t1.MVIEW_NAME "
                "AND t1.UPDATABLE = 'N' "
            ")"
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Excluded read-only MVIEW from targets.") << std::endl;

    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "DELETE FROM TARGET_TABLES "
            "WHERE (TABLE_NAME LIKE 'SNAP$\%' "
            "OR TABLE_NAME LIKE 'RUPD$\%' "
            "OR TABLE_NAME LIKE 'USLOG$\%' "
            ")"
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Excluded MVIEW relatiions from targets.") << std::endl;

    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "DELETE FROM TARGET_TABLES "
            "WHERE EXISTS ( "
                "SELECT null FROM ALL_TABLES t1 "
                "WHERE TARGET_TABLES.OWNER = t1.OWNER "
                "AND TARGET_TABLES.TABLE_NAME = t1.TABLE_NAME "
                "AND t1.IOT_TYPE = 'IOT_OVERFLOW' "
            ")"
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Excluded IOT OVERFLOW SEGS from targets.") << std::endl;

    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "DELETE FROM TARGET_TABLES "
            "WHERE EXISTS ( "
                "SELECT null FROM ALL_EXTERNAL_TABLES t1 "
                "WHERE TARGET_TABLES.OWNER = t1.OWNER "
                "AND TARGET_TABLES.TABLE_NAME = t1.TABLE_NAME "
            ")"
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Excluded EXTERNAL TABLES from targets.") << std::endl;

    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "DELETE FROM TARGET_TABLES "
            "WHERE EXISTS ( "
                "SELECT null FROM ALL_TABLES t1 "
                "WHERE TARGET_TABLES.OWNER = t1.OWNER "
                "AND TARGET_TABLES.TABLE_NAME = t1.TABLE_NAME "
                "AND (t1.DROPPED = 'YES' OR t1.TEMPORARY = 'Y') "
            ")"
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Excluded ENTRIES in RECYCLE_BIN from targets.") << std::endl;
}

void cTargetTables::vCountSpecialColumn()
{
    ASSERT_OR_RAISE_FNC(
        ! oDb_.iExecSql(
            "UPDATE TARGET_TABLES "
            "SET NUM_LONGS = ("
                "SELECT COUNT(*) FROM ALL_TAB_COLUMNS "
                "WHERE TARGET_TABLES.OWNER = OWNER "
                "AND TARGET_TABLES.TABLE_NAME = TABLE_NAME "
                "AND DATA_TYPE IN ('LONG','LONG RAW') "
            ") "
            ",   NUM_LOBS = ("
                "SELECT COUNT(*) FROM ALL_TAB_COLUMNS "
                "WHERE TARGET_TABLES.OWNER = OWNER "
                "AND TARGET_TABLES.TABLE_NAME = TABLE_NAME "
                "AND DATA_TYPE IN ('CLOB','BLOB','NCLOB')"
            ") "
        ), std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_)
    );
    trc_ << std::string("Numerated special data type columns such as LONG or LOB.") << std::endl;
}

cTargetTables::cTargetTables(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oListTable
    , const ps::lib::str_vct& oListFixed
    , const ps::lib::str_vct& oListExcpt
) : trc_(ps::lib::cTracer::get_mutable_instance())
    , oDb_(oDb)
    , iSkip_(sizeof(tAttributes))
    , rTable_(0)
    , iInd_(0)
{
    if (!oListTable.empty()) vInsItems(oListTable);
    if (!oListFixed.empty()) vChgItems(oListFixed);
    if (!oListExcpt.empty()) vDelItems(oListExcpt);
    vDelOptionals();
    vCountSpecialColumn();
}

cTargetTables::~cTargetTables()
{
    delete [] rTable_;
    delete [] iInd_;
}

// excludes TABLE_NAME that does not include
// the number of rows or more iNumRows from TARGET_TABLES
ps::app::xtru::cTableList cTargetTables::oRmWhereNumRows(ps::lib::sql::occi::cSvc& oSvc, const int32_t iNumRows)
{
    static const std::string sPls =
        "declare "
            "i_num_rows binary_integer := :b_num_rows; "
            "o_num_rows binary_integer; "
            "ORA_06552 exception; " /* Compilation unit analysis terminated */
            "ORA_06550 exception; " /* SQL Statement ignored */
            "pragma exception_init(ORA_06552, -6552); "
            "pragma exception_init(ORA_06550, -6550); "
        "begin "
            "select /*+ first_rows */ count(*) into o_num_rows "
            "from \"%s\".\"%s\" where rownum <= i_num_rows; "
            ":b_deleted := case when o_num_rows < i_num_rows then 1 else 0 end;"
        "exception "
            "when ORA_06552 or ORA_06550 then null;"
        "end; "
    ;
    ps::app::xtru::cTableList oTableList;
    tAttributes rRowBuf;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_,
        "SELECT t1.OWNER "
        ", t1.TABLE_NAME "
        ", t1.IOT_TYPE "
        ", t1.DATA_FMT "
        ", t1.NUM_LONGS "
        ", t1.NUM_LOBS "
        "FROM TARGET_TABLES t1 "
        ", ALL_TABLES t2 "
        "WHERE t2.OWNER = t1.OWNER "
        "AND t2.TABLE_NAME = t1.TABLE_NAME "
        "ORDER BY t2.NUM_MBYTES DESC, t1.OWNER, t1.TABLE_NAME"
    );
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwner, cAttr::STR, iInd_, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szTable, cAttr::STR, iInd_, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szIotType, cAttr::STR, iInd_, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.iDataFmt, cAttr::INT32, iInd_, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.iNumLongs, cAttr::INT32, iInd_, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.iNumLobs, cAttr::INT32, iInd_, iSkip_, iSkip_);
    ps::lib::sql::lite3::cSqliteStmt oDel(oDb_,
        "DELETE FROM TARGET_TABLES "
        "WHERE OWNER = ? "
        "AND TABLE_NAME = ? "
    );
    ASSERT_OR_RAISE_FNC(oDel.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    using ps::lib::sql::lite3::cAttr;
    ps::lib::sql::lite3::cBind& oDBind(oDel.oGetBind());
    oDBind.vAddItem(rRowBuf.szOwner, cAttr::STR, iInd_, iSkip_, iSkip_);
    oDBind.vAddItem(rRowBuf.szTable, cAttr::STR, iInd_, iSkip_, iSkip_);
    // input data
    ps::lib::sql::occi::cBind oBind;
    auto deleted = 1;
    oBind.vAddItem(":b_num_rows", &iNumRows);
    oBind.vAddItem(":b_deleted", &deleted);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&]
        {
            std::ostringstream tag;
            tag << boost::format("\"%s\".\"%s\"") % rRowBuf.szOwner % rRowBuf.szTable;
            std::unique_ptr<ps::lib::sql::occi::cStmt> oOcci(
                new ps::lib::sql::occi::cStmt(oSvc, 1, sPls, tag.str(), &oBind)
            );
            oOcci->vConvPlaceHolder({rRowBuf.szOwner, rRowBuf.szTable});
            oOcci->vExecute();
            if (deleted)
            {
                ASSERT_OR_RAISE_FNC(oDel.iExecute(ps::lib::iOneItem) == SQLITE_DONE
                    , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
                oTables_.push_back({rRowBuf.szOwner, rRowBuf.szTable});
            }
            else
            {
                oTableList.push_back({
                    rRowBuf.szOwner, rRowBuf.szTable, rRowBuf.szIotType
                    , rRowBuf.iDataFmt, rRowBuf.iNumLongs, rRowBuf.iNumLobs
                });
            }
        }
        , [&]
        { trc_ << std::string("Now counting number of rows:") << std::endl; }
        , [&]
        {
            trc_ << std::string("*** Following tables were excluded from the target, because number of rows was less than "
                    ) << iNumRows << '.' << std::endl;
            for (auto item : oTables_)
            {
                trc_ << boost::format("Excluded %s.%s")
                    % item.szOwner % item.szTable << std::endl;
            }
        }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    return oTableList;
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps
