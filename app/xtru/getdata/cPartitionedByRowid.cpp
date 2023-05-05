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

namespace getdata
{
class cPartitionedByRowidImpl
{
private:
    const static char szQuery[];
    ps::lib::cTracer& trc_;
    const ps::lib::cConfigures& conf_;
    ps::lib::sql::lite3::cSqliteDb& oDb_;
    ps::lib::sql::occi::cSvc::tPtr& oSvc_;
    const uint32_t iBulkSize_;
    const size_t iSkip_;
    const boost::filesystem::path exec_load_;
    const int32_t iRows_;  ///< A Number of rows at a time of loading.
    ps::lib::tPtrFstream& st_make_sh_;
    struct tAttributes
    {
        char szOwner[OBJECT_NAME_LEN];             // PK1 NOT NULL VARCHAR2(30)
        char szTableName[OBJECT_NAME_LEN];         // PK2 NOT NULL VARCHAR2(30)
        int32_t iRangeNo;                          // PK3 n = 1,2,3, ...
        char szRowidBgn[ROWID_STR_LEN];            // NOT NULL VARCHAR2(19)
        char szRowidEnd[ROWID_STR_LEN];            // NOT NULL VARCHAR2(19)
        tAttributes(
            const char* szOwner_ =0
            , const char* szTableName_ =0
            , const char* szRowidBgn_ =0
            , const char* szRowidEnd_ =0
        ){
            ::memset(this, 0, sizeof(tAttributes));
            if (szOwner_) strcpy(szOwner, szOwner_);
            if (szTableName_) strcpy(szTableName, szTableName_);
            if (szRowidBgn_) strcpy(szRowidBgn, szRowidBgn_);
            if (szRowidEnd_) strcpy(szRowidEnd, szRowidEnd_);
        }
        tAttributes(const tAttributes& rhs)
        {
            ::memset(this, 0, sizeof(tAttributes));
            strcpy(szOwner, rhs.szOwner);
            strcpy(szTableName, rhs.szTableName);
            iRangeNo = rhs.iRangeNo;
            strcpy(szRowidBgn, rhs.szRowidBgn);
            strcpy(szRowidEnd, rhs.szRowidEnd);
        }
        bool operator<(const tAttributes& rhs) const
        {
            return ps::lib::composite_less(
                szOwner, rhs.szOwner
                , szTableName, rhs.szTableName
                , iRangeNo, rhs.iRangeNo // sort by ACENDING iRangeNo.
            );
        }
        bool iEqual(const ps::app::xtru::cTableList::value_type& tbl) const
        {
            return std::string(szOwner) == tbl.sOwner
                && std::string(szTableName) == tbl.sTable
            ;
        }
        std::string sGetRangeNo(void) const
        {
            return (boost::format("%02d") % iRangeNo).str();
        }
    };
    ps::lib::cList<tAttributes> oList_;
    void oSelectMatchedAndRemove(const ps::app::xtru::cTableList::value_type& tbl, ps::lib::cList<tAttributes>& oChosen)
    {
        std::copy_if(oList_.cbegin(), oList_.cend(), std::back_inserter(oChosen), tbl);
        /// Removing elements that matched for argument tbl.
        const auto new_end = std::remove_if(oList_.begin(), oList_.end(), tbl);
        oList_.erase(new_end, oList_.end());
    }
public:
    cPartitionedByRowidImpl(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , ps::lib::sql::occi::cSvc::tPtr& oSvc
        , const uint32_t& iBulkSize
        , const int32_t& iRows
        , ps::lib::tPtrFstream& st_make_sh
    );
    ~cPartitionedByRowidImpl();
    bool iFind(const ps::app::xtru::cTableList::value_type& tbl) const;
    std::string sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl);
    /**
     * @brief
     * @param [in] tbl
     * @param [out] unldrs
     * @param [in] param_f
     */
    void vInsertUnloadTasks(
        const ps::app::xtru::tTabName& tbl
        , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
        , const boost::filesystem::path& param_f
    );
};

const char cPartitionedByRowidImpl::szQuery[]  = {"SELECT * FROM %s WHERE ROWID BETWEEN '%s' AND '%s'"};

cPartitionedByRowidImpl::cPartitionedByRowidImpl(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , ps::lib::sql::occi::cSvc::tPtr& oSvc
    , const uint32_t& iBulkSize
    , const int32_t& iRows
    , ps::lib::tPtrFstream& st_make_sh
)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , oDb_(oDb)
    , oSvc_(oSvc)
    , iBulkSize_(iBulkSize)
    , iSkip_(sizeof(tAttributes))
    , exec_load_(conf_.as<std::string>("exec_load"))
    , iRows_(iRows)
    , st_make_sh_(st_make_sh)
{
    static const char sStmt[] = {
    "SELECT T2.OWNER "
    ", T2.SEGMENT_NAME "
    ", T2.RN AS RN "
    ", T2.PREDB AS PREDB "
    ", T2.PREDE AS PREDE "
    "FROM TARGET_TABLES T0"
    ", ALL_TABLES T1"
    ", ROWID_RANGES T2 "
    "WHERE T1.OWNER = T0.OWNER "
    "AND T1.TABLE_NAME = T0.TABLE_NAME "
    "AND T2.OWNER = T1.OWNER "
    "AND T2.SEGMENT_NAME = T1.TABLE_NAME "
    "AND T1.PARTITIONED = 'NO' "
    "AND T1.NUM_MBYTES >= ? "
    };
    tAttributes rRowBuf;
    auto iNumBytes = ps::lib::iIntStrToBinInt<int64_t>(conf_.as<std::string>("rowid_split_min_size"));
    auto iNumMBytes = static_cast<int32_t>(iNumBytes >> 20);  // Convert from bytes -> Mbytes
    auto iNumMBytesInd = ps::lib::sql::ind_t::VAL_IS_NOTNULL;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_, sStmt);
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwner, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szTableName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.iRangeNo, cAttr::INT32, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szRowidBgn, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szRowidEnd, cAttr::STR, NULL, iSkip_, iSkip_);
    ps::lib::sql::lite3::cBind& oBind(oStmt.oGetBind());
    oBind.vAddItem(iNumMBytes, cAttr::INT32, &iNumMBytesInd, 0, 0);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&] { oList_.push_back(tAttributes(rRowBuf)); }
        , [&] { trc_ << std::string("Start to read the information of the pieces of splitting by rowid range.") << std::endl; }
        , [&] { trc_ << boost::format("Finished to read and returned %d rows.") % oList_.size() <<  std::endl; }
        , [&] { trc_ << std::string("Not found the partitioning information.") << std::endl; }
        , [&] { oList_.sort(); }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

cPartitionedByRowidImpl::~cPartitionedByRowidImpl()
{}

bool cPartitionedByRowidImpl::iFind(const ps::app::xtru::cTableList::value_type& tbl) const 
{
    return std::find_if( oList_.cbegin(), oList_.cend(), tbl) != oList_.cend();
}

std::string cPartitionedByRowidImpl::sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl)
{
    ps::lib::cList<tAttributes> oChosen;
    oSelectMatchedAndRemove(tbl, oChosen);
    std::ostringstream oss;
    for (const auto& rRowBuf: oChosen)
    {
        oss << boost::format(szQuery)
            % tbl.sGetConcatenatedName("\"") 
            % rRowBuf.szRowidBgn
            % rRowBuf.szRowidEnd
            << ';';
    }
    return oss.str();
}

void cPartitionedByRowidImpl::vInsertUnloadTasks(
    const ps::app::xtru::tTabName& tbl
    , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
    , const boost::filesystem::path& param_f
){
    ps::lib::cList<tAttributes> oChosen;
    oSelectMatchedAndRemove(tbl, oChosen);
    std::ostringstream oss;
    for (const auto& rRowBuf: oChosen)
    {
        // SQL for tables that are not split and extracted.
        oss << boost::format(szQuery)
            % tbl.sGetConcatenatedName("\"") 
            % rRowBuf.szRowidBgn
            % rRowBuf.szRowidEnd
            ;
        const auto table_n(tbl.sGetConcatenatedName()); // Non-enclosing name will be return.
        const auto file_n = ps::lib::sConvertDollar2Sharp(table_n);
        auto ptr = new ps::lib::sql::occi::cUnloader(
            *oSvc_
            , new ps::lib::nsStreamLocator::cStreamLocator(tbl.sOwner, file_n, rRowBuf.sGetRangeNo())
            , iBulkSize_, oss.str(), table_n, tbl.iNumLongs
        );
        unldrs.push_back(ptr);
        oss.str("");
        const auto fname = ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName()) + "_" + rRowBuf.sGetRangeNo();
        *st_make_sh_
            << (
              ("IOT" == tbl.sIotType || tbl.iNumLobs)
              ? boost::format("%s parfile=%s control=%s.ctl")
                  % exec_load_
                  % param_f
                  % fname
              : boost::format("%s parfile=%s rows=%d control=%s.ctl")
                  % exec_load_
                  % param_f
                  % iRows_
                  % fname
            )
            << std::endl
        ;
    }
}

cPartitionedByRowid::cPartitionedByRowid(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , ps::lib::sql::occi::cSvc::tPtr& oSvc
    , const uint32_t& iBulkSize
    , const int32_t& iRows
    , ps::lib::tPtrFstream& st_make_sh
)
    : oImpl_(new cPartitionedByRowidImpl(oDb, oSvc, iBulkSize, iRows, st_make_sh))
{}

cPartitionedByRowid::~cPartitionedByRowid()
{}

bool cPartitionedByRowid::iFind(const ps::app::xtru::cTableList::value_type& tbl) const 
{
    return oImpl_->iFind(tbl);
}

std::string cPartitionedByRowid::sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl)
{
    return oImpl_->sGenerateSql(tbl);
}

void cPartitionedByRowid::vInsertUnloadTasks(
    const ps::app::xtru::tTabName& tbl
    , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
    , const boost::filesystem::path& param_f
){
    oImpl_->vInsertUnloadTasks(tbl, unldrs, param_f);
}

} // ps::app::xtru::getdata

} // ps::app::xtru

} /* namespace app */

} /* namespace ps */
