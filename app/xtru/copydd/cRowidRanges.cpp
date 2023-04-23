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
#include <xtru.h>

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

struct cRowidRanges::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];             // NOT NULL VARCHAR2(30)
    char szTableName[OBJECT_NAME_LEN];         // NOT NULL VARCHAR2(30)
    int32_t iRangeNo;                              // n = 1,2,3, ...
    char szRowidBgn[ROWID_STR_LEN];             // NOT NULL VARCHAR2(19)
    char szRowidEnd[ROWID_STR_LEN];             // NOT NULL VARCHAR2(19)
};

const char cRowidRanges::szCreStmt[] = {
"CREATE TABLE ROWID_RANGES\n"
"( OWNER                          TEXT NOT NULL\n"
", SEGMENT_NAME                   TEXT NOT NULL\n"
", RN                             INT NOT NULL\n"
", PREDB                          TEXT NOT NULL\n"
", PREDE                          TEXT NOT NULL\n"
", CONSTRAINT PK_ROWID_RANGES PRIMARY KEY\n"
    "( OWNER\n"
    ", SEGMENT_NAME\n"
    ", RN\n"
    ")\n"
")"
};

const char cRowidRanges::szDelStmt[] = {
"DELETE FROM ROWID_RANGES"
};

const char cRowidRanges::szInStmt_[] = {
"select c.owner, c.segment_name "
", c.rn + 1 as rn "
", sys.dbms_rowid.rowid_create(1, d.oid, c.fid1, c.bid1, 0)    predb "
", sys.dbms_rowid.rowid_create(1, d.oid, c.fid2, c.bid2, 9999) prede "
"from ( "
    "select distinct a.owner, a.segment_name, b.rn "
    ", first_value(a.fid) over ( "
        "partition by a.owner, a.segment_name, b.rn "
        "order by a.fid, a.bid rows between unbounded preceding "
                                       "and unbounded following) fid1 "
    ", last_value(a.fid) over ( "
        "partition by a.owner, a.segment_name, b.rn "
        "order by a.fid, a.bid rows between unbounded preceding "
                                       "and unbounded following) fid2 "
    ", first_value( "
        "decode( "
            "sign(range2 - range1) "
            ", 1 "
            ", a.bid + ((b.rn - a.range1) * a.chunks1) "
            ", a.bid "
        ") "
    ") over ( "
        "partition by a.owner, a.segment_name, b.rn "
        "order by a.fid, a.bid rows between unbounded preceding "
                                       "and unbounded following) bid1 "
    ", last_value( "
        "decode( "
            "sign(range2 - range1) "
            ", 1 "
            ", a.bid + ((b.rn - a.range1 + 1) * a.chunks1) - 1 "
            ", (a.bid + a.blocks - 1) "
        ") "
    ") over ( "
        "partition by a.owner, a.segment_name, b.rn "
        "order by a.fid, a.bid rows between unbounded preceding "
                                       "and unbounded following) bid2 "
    "from ( "
        "select owner, segment_name "
        ", fid "
        ", bid "
        ", blocks "
        ", chunks1 "
        ", trunc((sum2 - blocks + 1 - 0.1) / chunks1) range1 "
        ", trunc((sum2 - 0.1) / chunks1) range2 "
        "from ( "
            "select /*+ rule */ owner, segment_name "
            ", relative_fno fid "
            ", block_id bid "
            ", blocks "
            ", sum(blocks) over () sum1 "
            ", trunc((sum(blocks) over ( "
                "partition by owner, segment_name)) / :NUM_RANGES) chunks1 "
            ", sum(blocks) over ( "
                "partition by owner, segment_name "
                "order by relative_fno, block_id) sum2 "
            "from dba_extents "
            "where owner in %s "
            "and partition_name is null "
        ") "
        "where sum1 > :NUM_RANGES "
        "and chunks1 > 0 "
    ") a "
    ", ( "
        "select rownum - 1 rn from dual "
        "connect by level <= :NUM_RANGES "
    ") b "
    "where b.rn between a.range1 and a.range2 "
") c "
", ( "
    "select owner, object_name, max(data_object_id) oid "
    "from dba_objects "
    "where owner in %s "
    "and data_object_id is not null "
    "and object_type = 'TABLE' "
    "group by owner, object_name "
") d "
"where c.owner = d.owner "
"and c.segment_name = d.object_name "
};

const char cRowidRanges::szOutStmt_[] = {
"INSERT INTO ROWID_RANGES "
"( OWNER "
", SEGMENT_NAME "
", RN "
", PREDB "
", PREDE "
") VALUES (?,?,?,?,?)"
};

const uint32_t cRowidRanges::iBulkSize_ = 1000;

cRowidRanges::cRowidRanges(
    ps::lib::sql::occi::cSvc& oSvc
    , ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
)
    : cTransporter(oSvc, oDb, iBulkSize_, szInStmt_, "ROWID_RANGES", &oBind_, szOutStmt_)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , rTable_(new tAttributes[iBulkSize_])
    , conf_(ps::lib::cConfigures::get_const_instance())
    , iNumRangeParts_(conf_.as<int32_t>("rowid_split_num_parts"))
    , iAvailable_(false)
{
    auto iNumBytes = ps::lib::iIntStrToBinInt<int64_t>(conf_.as<std::string>("rowid_split_min_size"));
    /// Checking availabity of the Data Dictionary.
    ps::lib::sql::occi::cMetaData oMetaData(oSvc);
    oMetaData.vAddNamesAndTypes({
        { "SYS.DBA_OBJECTS", oracle::occi::MetaData::PTYPE_UNK }
        , { "SYS.DBA_EXTENTS", oracle::occi::MetaData::PTYPE_UNK }
    });
    iAvailable_ = oMetaData.iGetAvailability() && (iNumBytes > 0LL);
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners), sGetSqlInList(oOwners) });
    // input data
    oBind_.vAddItem(":NUM_RANGES", &iNumRangeParts_);
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iRangeNo, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRowidBgn, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRowidEnd, SQLT_STR, NULL, NULL, NULL, iSkip_);
    // Adding column attributes to SQLite3.
    oOBind_.vAddItem(rTable_->szOwner, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szTableName, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->iRangeNo, tLite3Type::INT32, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szRowidBgn, tLite3Type::STR, NULL, iSkip_, iSkip_);
    oOBind_.vAddItem(rTable_->szRowidEnd, tLite3Type::STR, NULL, iSkip_, iSkip_);
}

cRowidRanges::~cRowidRanges()
{
    delete [] rTable_;
}

void cRowidRanges::vExecuteAndFetch()
{
    if (iAvailable_)
    {
        cTransporter::vExecuteAndFetch();
    }
}

void cRowidRanges::vPreBulkAction(const uint32_t& iBulkSize)
{
    ::memset(rTable_, 0, iSkip_ * iBulkSize);
}

void cRowidRanges::vPostBulkAction(const uint32_t& iNumIter)
{
    ASSERT_OR_RAISE_FNC(iExecute(iNumIter) == SQLITE_DONE, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oGetDb()));
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps


