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

class cPartitionedBySchemeImpl
{
private:
    const static char szQuery[];
    ps::lib::cTracer& trc_;
    const ps::lib::cConfigures& conf_;         /// This will activate but not in used.
    const ps::lib::cDelimiter oDelim_;
    ps::lib::sql::lite3::cSqliteDb& oDb_;
    ps::lib::sql::occi::cSvc::tPtr& oSvc_;
    const uint32_t iBulkSize_;
    const size_t iSkip_;
    const boost::filesystem::path exec_load_;
    const int32_t iRows_;  ///< A Number of rows at a time of loading.
    ps::lib::tPtrFstream& st_make_sh_;
    struct tAttributes
    {
        char szOwnerName[OBJECT_NAME_LEN];
        char szTableName[OBJECT_NAME_LEN];
        char szObjectType[OBJECT_TYPE_LEN];    // either NULLSTR, "PARTITION", "SUBPARTITION" 
        ps::lib::sql::ind_t nObjectTypeInd;         // indicates OCI_IND_NOTNULL if the table is partitioned.
        char szPartitionName[OBJECT_NAME_LEN]; // Partition or Subpartition name
        ps::lib::sql::ind_t nPartitionNameInd;
        int iAvgRowLength;                     // 
        double fNumBytes;                      // Usage of segment in bytes.
        tAttributes(
            const char* szOwnerName_ =0
            , const char* szTableName_ =0
            , const char* szObjectType_ =0
            , const char* szPartitionName_ =0
        ){
            ::memset(this, 0, sizeof(tAttributes));
            if (szOwnerName_) strcpy(szOwnerName, szOwnerName_);
            if (szTableName_) strcpy(szTableName, szTableName_);
            if (szObjectType_) strcpy(szObjectType, szObjectType_);
            if (szPartitionName_) strcpy(szPartitionName, szPartitionName_);
        }
        tAttributes(const tAttributes& rhs)
        {
            ::memset(this, 0, sizeof(tAttributes));
            strcpy(szOwnerName, rhs.szOwnerName);
            strcpy(szTableName, rhs.szTableName);
            strcpy(szObjectType, rhs.szObjectType);
            nObjectTypeInd = rhs.nObjectTypeInd;
            strcpy(szPartitionName, rhs.szPartitionName);
            nPartitionNameInd = rhs.nPartitionNameInd;
            fNumBytes = rhs.fNumBytes;
        }
        bool operator<(const tAttributes& rhs) const
        {
            return ps::lib::composite_less(
                szOwnerName, rhs.szOwnerName
                , szTableName, rhs.szTableName
                , rhs.fNumBytes, fNumBytes  // sort by DECENDING fNumBytes.
            );
        }
        bool iEqual(const ps::app::xtru::cTableList::value_type& tbl) const
        {
            return std::string(szOwnerName) == tbl.sOwner
                && std::string(szTableName) == tbl.sTable
            ;
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
    cPartitionedBySchemeImpl(
        ps::lib::sql::lite3::cSqliteDb& oDb
        , ps::lib::sql::occi::cSvc::tPtr& oSvc
        , const uint32_t& iBulkSize
        , const int32_t& iRows
        , ps::lib::tPtrFstream& st_make_sh
    );
    ~cPartitionedBySchemeImpl();
    bool iFind(const ps::app::xtru::cTableList::value_type& tbl) const ;
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

const char cPartitionedBySchemeImpl::szQuery[] = {"SELECT * FROM %s %s(\"%s\")"};

cPartitionedBySchemeImpl::cPartitionedBySchemeImpl(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , ps::lib::sql::occi::cSvc::tPtr& oSvc
    , const uint32_t& iBulkSize
    , const int32_t& iRows
    , ps::lib::tPtrFstream& st_make_sh
)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , conf_(ps::lib::cConfigures::get_const_instance())
    , oDelim_(ps::lib::oMakeVarDelimiter())
    , oDb_(oDb)
    , oSvc_(oSvc)
    , iBulkSize_(iBulkSize)
    , iSkip_(sizeof(tAttributes))
    , exec_load_(conf_.as<std::string>("exec_load"))
    , iRows_(iRows)
    , st_make_sh_(st_make_sh)
{
    static const char sStmt[] = {
    "SELECT T1.TABLE_OWNER "
    ", T1.TABLE_NAME "
    ", REPLACE(T4.OBJECT_TYPE,'TABLE ','') AS OBJECT_TYPE "
    ", T4.SUBOBJECT_NAME AS PARTITION_NAME "
    ", ifnull(T1.NUM_ROWS * T1.AVG_ROW_LEN, 0) AS NUM_BYTS "
    "FROM TARGET_TABLES T0 "
    ", ALL_TAB_PARTITIONS T1"
    ", ALL_PART_TABLES T3 "
    ", ALL_OBJECTS T4 "
    "WHERE T1.TABLE_OWNER = T0.OWNER "
    "AND T1.TABLE_NAME = T0.TABLE_NAME "
    "AND T3.OWNER = T1.TABLE_OWNER "
    "AND T3.TABLE_NAME = T1.TABLE_NAME "
    "AND T4.OWNER = T3.OWNER "
    "AND T4.OBJECT_NAME = T3.TABLE_NAME "
    "AND T4.SUBOBJECT_NAME = T1.PARTITION_NAME "
    "AND ( "
        "( "
            "T3.SUBPARTITIONING_TYPE IS NULL " /* Non-composit partitioning */
        ") OR ( "
            "T3.SUBPARTITIONING_TYPE IS NOT NULL " /* Composit partitioning */
            "AND T4.OBJECT_TYPE = 'TABLE PARTITION' "
        ") "
    ") "
    };
    tAttributes rRowBuf;
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb_, sStmt);
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwnerName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szTableName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szObjectType, cAttr::STR, &rRowBuf.nObjectTypeInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szPartitionName, cAttr::STR, &rRowBuf.nPartitionNameInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.fNumBytes, cAttr::DOUBLE, NULL, iSkip_, iSkip_);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&] { oList_.push_back(tAttributes(rRowBuf)); }
        , [&] { trc_ << std::string("Start to read the information of the partitions of partitioned table.") << std::endl; }
        , [&] { trc_ << boost::format("Finished to read and returned %d rows.") % oList_.size() << std::endl; }
        , [&] { trc_ << std::string("Not found the partitioning information.") << std::endl; }
        , [&] { oList_.sort(); }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb_));
}

cPartitionedBySchemeImpl::~cPartitionedBySchemeImpl()
{}

bool cPartitionedBySchemeImpl::iFind(const ps::app::xtru::cTableList::value_type& tbl) const 
{
    return std::find_if( oList_.cbegin(), oList_.cend(), tbl) != oList_.cend();
}

std::string cPartitionedBySchemeImpl::sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl)
{
    ps::lib::cList<tAttributes> oChosen;
    oSelectMatchedAndRemove(tbl, oChosen);
    std::ostringstream oss;
    for (const auto& rRowBuf: oChosen)
    {
        oss << boost::format(szQuery)
            % tbl.sGetConcatenatedName("\"") 
            % rRowBuf.szObjectType
            % rRowBuf.szPartitionName
            << ';';
    }
    return oss.str();
}

void cPartitionedBySchemeImpl::vInsertUnloadTasks(
    const ps::app::xtru::tTabName& tbl
    , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
    , const boost::filesystem::path& param_f
){
    ps::lib::cList<tAttributes> oChosen;
    oSelectMatchedAndRemove(tbl, oChosen);
    std::ostringstream oss;
    for (const auto& rRowBuf: oChosen)
    {
        oss << boost::format(szQuery)
            % tbl.sGetConcatenatedName("\"") 
            % rRowBuf.szObjectType
            % rRowBuf.szPartitionName
            << ';';
        const auto table_n(tbl.sGetConcatenatedName()); // Non-enclosing name will be return.
        const auto file_n = ps::lib::sConvertDollar2Sharp(table_n);
        auto ptr = new ps::lib::sql::occi::cUnloader(
            *oSvc_
            , new ps::lib::nsStreamLocator::cStreamLocator(tbl.sOwner, file_n, rRowBuf.szPartitionName)
            , iBulkSize_, oss.str(), table_n, tbl.iNumLongs
        );
        unldrs.push_back(ptr);
        oss.str("");
        const auto fname = ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName()) + "_" + rRowBuf.szPartitionName;
        *st_make_sh_
            << (
              ("IOT" == tbl.sIotType || tbl.iNumLobs)
              ? boost::format("%s parfile=%s control=%s.ctl")
                  % exec_load_.string()
                  % param_f.string()
                  % fname
              : boost::format("%s parfile=%s rows=%d control=%s.ctl")
                  % exec_load_.string()
                  % param_f.string()
                  % iRows_
                  % fname
            )
            << std::endl
        ;
    }
}

cPartitionedByScheme::cPartitionedByScheme(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , ps::lib::sql::occi::cSvc::tPtr& oSvc
    , const uint32_t& iBulkSize
    , const int32_t& iRows
    , ps::lib::tPtrFstream& st_make_sh
)
    : oImpl_(new cPartitionedBySchemeImpl(oDb, oSvc, iBulkSize, iRows, st_make_sh))
{}

cPartitionedByScheme::~cPartitionedByScheme()
{}

bool cPartitionedByScheme::iFind(const ps::app::xtru::cTableList::value_type& tbl) const 
{
    return oImpl_->iFind(tbl);
}

std::string cPartitionedByScheme::sGenerateSql(const ps::app::xtru::cTableList::value_type& tbl)
{
    return oImpl_->sGenerateSql(tbl);
}

void cPartitionedByScheme::vInsertUnloadTasks(
    const ps::app::xtru::tTabName& tbl
    , ps::lib::tSequence<ps::lib::sql::cFetchable>& unldrs
    , const boost::filesystem::path& param_f
){
    oImpl_->vInsertUnloadTasks(tbl, unldrs, param_f);
}

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps

