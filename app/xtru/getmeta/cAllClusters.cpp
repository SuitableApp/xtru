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

constexpr size_t CLUSTER_TYPE_LEN = 5+1;
constexpr size_t FUNCTION_NAME_LEN = 15+1;
constexpr size_t CACHE_TYPE_LEN = 1+1;
constexpr size_t SINGLETBL_TYPE_LEN = 10+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllClusters::tAttributes
 * See to study about [ALL_CLUSTERS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-A006481A-A195-48A1-A276-8AEE570FFDD8.htm#REFRN20039)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllClusters::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the cluster
	char szClusterName[OBJECT_NAME_LEN];   ///< Name of the cluster
	ub4 iPctFree;                          ///< Minimum percentage of free space in a block
	ub4 iPctUsed;                          ///< Minimum percentage of used space in a block
	ub4 iIniTrans;                         ///< Initial number of transactions
	ub4 iMaxTrans;                         ///< Maximum number of transactions
	ub4 iKeySize;                          ///< Estimated size of cluster key plus associated rows
	ps::lib::sql::ind_t nKeySizeInd;       ///< VAL_IS_NULL for partitioned tables
    /// - INDEX - B*-Tree index
    /// - HASH - Hash
	char szClusterType[CLUSTER_TYPE_LEN];  ///< Type of the cluster:
	char szFunction[FUNCTION_NAME_LEN];    ///< If the cluster is a hash cluster, the hash function
    ps::lib::sql::ind_t nFunctionInd;      ///< VAL_IS_NULL for partitioned tables
	ub4 iHashKeys;                         ///< If the cluster is a hash cluster, the number of hash keys (hash buckets)
	char szDegree[DEGREE_LEN];             ///< Number of threads per instance for scanning the cluster, or DEFAULT
	char szInstances[INSTANCES_LEN];       ///< Number of instances across which the cluster is to be scanned , or DEFAULT
	char szCache[CACHE_TYPE_LEN];          ///< Indicates whether the cluster is to be cached in the buffer cache (Y) or not (N)
	char szSingleTable[SINGLETBL_TYPE_LEN];///< Indicates whether this is a single-table cluster (Y) or not (N)
	ub4 iPctIncrease;                      ///< Percentage increase in extent size
    ps::lib::sql::ind_t nPctIncreaseInd;   ///< VAL_IS_NULL for partitioned tables
	ub4 iFreelists;                        ///< Number of process freelists allocated to this segment
    ps::lib::sql::ind_t nFreelistsInd;     ///< VAL_IS_NULL for partitioned tables
	ub4 iFreelistGroups;                   ///< Number of freelist groups allocated to this segment
    ps::lib::sql::ind_t nFreelistGroupsInd;///< VAL_IS_NULL for partitioned tables
	char szBufferPool[BUFFER_POOL_LEN];    ///< Buffer pool to be used for cluster blocks:
    ps::lib::sql::ind_t nBufferPoolInd;    ///< VAL_IS_NULL for partitioned tables
	char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the cluster
	ps::lib::sql::ind_t nTablespaceNameInd;///< VAL_IS_NULL for partitioned tables
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iPctFree(0)
        , iPctUsed(0)
        , iIniTrans(0)
        , iMaxTrans(0)
        , iKeySize(0)
        , nKeySizeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nFunctionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHashKeys(0)
        , iPctIncrease(0)
        , nPctIncreaseInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelists(0)
        , nFreelistsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelistGroups(0)
        , nFreelistGroupsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nBufferPoolInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szClusterName, 0, sizeof(szClusterName));
        ::memset(szClusterType, 0, sizeof(szClusterType));
        ::memset(szFunction, 0, sizeof(szFunction));
        ::memset(szDegree, 0, sizeof(szDegree));
        ::memset(szInstances, 0, sizeof(szInstances));
        ::memset(szCache, 0, sizeof(szCache));
        ::memset(szSingleTable, 0, sizeof(szSingleTable));
        ::memset(szBufferPool, 0, sizeof(szBufferPool));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::strcpy(szOwner, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szClusterName, rhs.szClusterName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllClusters& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllClusters::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllClusters& oCtx) const
{
    using ps::lib::iCompBinary;
    std::string sClusterType;
    std::string sKeySize;

    if (iCompBinary(strcmp, szClusterType, "INDEX"))
    {
        sClusterType = "index ";
    }
    else
    {
        sClusterType = (boost::format("%shashkeys %d ")
            % (iCompBinary(strcmp, szSingleTable, "Y") ? "single table ": "")
            % iHashKeys
        ).str();
    }
    os << sPrintLabel("Clus", i + 1, szClusterName);
    os << boost::format("drop cluster %s including tables cascade constraints;")
        % ps::lib::sMakeEnclosedName(szClusterName)
        << std::endl
        << boost::format("create cluster %s") 
        % ps::lib::sMakeEnclosedName(szClusterName)
        << std::endl
        // Describing of the key of the clustering.
        << oPrintItemsFN(oCtx.oAllTabColumns_, &cAllTabColumns::vPrintAllTabColumns,  std::make_tuple(szOwner, szClusterName), true);
        ;
    if (nKeySizeInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        sKeySize = (boost::format("size %u") % iKeySize).str();
    }
    os << boost::format(") %s") % sClusterType;
    if (
        nFunctionInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
        && iCompBinary(strcmp, szFunction, "HASH EXPRESSION")
    ){
        os << oPrintItems(oCtx.oHashExpr_, std::make_tuple(szOwner, szClusterName), true);
    }
    os << sKeySize << std::endl << boost::format(
        "  pctfree %u pctused %u initrans %u maxtrans %u"
        " %s parallel (degree %s instances %s) "
        ) % iPctFree % iPctUsed % (iIniTrans > 1 ? iIniTrans : 2) % iMaxTrans
        % ((iCompBinary(strcmp, szCache,"Y")) ? "cache" : "nocache")
        % szDegree % szInstances
        << std::endl << std::string("  storage (")
    ;
    os << oPrintItems(oCtx.oUserSegments_, std::make_tuple(szOwner, szClusterName, "CLUSTER", ""), true);
    if (nPctIncreaseInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("pctincrease %u ") % iPctIncrease;
    }
    if (nFreelistsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("freelists %u ") % iFreelists;
    }
    if (nFreelistGroupsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("freelist groups %u ") % iFreelistGroups;
    }
    if (nBufferPoolInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("buffer_pool %s") % szBufferPool;
    }
    os << std::string(") ");
    if (nTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    os << std::endl << std::string(";") << std::endl;
    if (iCompBinary(strcmp, szClusterType, "INDEX"))
    {
        os << oPrintItems(
            oCtx.oUserIndexes_, std::make_tuple(szOwner, szClusterName, "")
            , true
            , false // false means not IOT
        );
    }
    os
        // Describing of the privileges of the object.
        << oPrintItems(oCtx.oAllTabPrivs_, std::make_tuple(szOwner, szClusterName), true)
        // Describing of the options of the object audit.
        << oPrintItems(oCtx.oUserObjAuditOpts_, std::make_tuple(szOwner, szClusterName), true)
        // Showing the trails of the audit.
        << oPrintItems(oCtx.oUserAuditObject_, std::make_tuple(szOwner, szClusterName), true)
    ;
}

std::string cAllClusters::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szClusterName='%s', iPctFree=%u, iPctUsed=%u, iIniTrans=%u, iMaxTrans=%u)"
        R"(, iKeySize=%u(%d), szClusterType='%s', szFunction='%s'(%d), iHashKeys=%u, szDegree='%s')"
        R"(, szInstances='%s', szCache='%s', szSingleTable='%s')"
        R"(, iPctIncrease=%u(%d), iFreelists=%u(%d), iFreelistGroups=%u(%d))"
        R"(, szBufferPool='%s'(%d), szTablespaceName='%s'(%d))"
    ) % szOwner % szClusterName % iPctFree % iPctUsed % iIniTrans % iMaxTrans
        % iKeySize % nKeySizeInd % szClusterType % szFunction % nFunctionInd % iHashKeys % szDegree
        % szInstances % szCache % szSingleTable
        % iPctIncrease % nPctIncreaseInd % iFreelists % nFreelistsInd % iFreelistGroups % nFreelistGroupsInd
        % szBufferPool % nBufferPoolInd % szTablespaceName % nTablespaceNameInd
    ;
    return oss.str();
}

/**
 * @struct cAllClusters::cRetriever
 */
class cAllClusters::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllClusters::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    /// number of elements in oList_.
    size_t& iNumRows_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , size_t& iNumRows
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllClusters::cRetriever::iBulkSize_ = 100;

cAllClusters::tAttributes cAllClusters::cRetriever::rTable_[cAllClusters::cRetriever::iBulkSize_];

cAllClusters::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllClusters
        , "Reading attributes of cluster", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
{
    const auto later_9iR11 = oSvc.iCompDbVersion("0900",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_9iR11 ? cAllClusters::szSingleTableReady : cAllClusters::szSingleTableNotReady
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szClusterName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPctFree, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPctUsed, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iIniTrans, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxTrans, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iKeySize, SQLT_UIN, &rTable_->nKeySizeInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szClusterType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szFunction, SQLT_STR, &rTable_->nFunctionInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iHashKeys, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDegree, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szInstances, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szCache, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szSingleTable, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPctIncrease, SQLT_UIN, &rTable_->nPctIncreaseInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelists, SQLT_UIN, &rTable_->nFreelistsInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelistGroups, SQLT_UIN, &rTable_->nFreelistGroupsInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szBufferPool, SQLT_STR, &rTable_->nBufferPoolInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, nullptr, nullptr, iSkip_);
}

void cAllClusters::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllClusters::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllClusters::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllClusters::szAllClusters[] = {
"select owner "
", cluster_name "
", pct_free "
", pct_used "
", ini_trans "
", max_trans "
", key_size "
", cluster_type "
", function "
", hashkeys "
", ltrim(degree) as degree "
", ltrim(instances) as instances "
", ltrim(cache) as cache "
", %s as single_table "
", pct_increase "
", freelists "
", freelist_groups "
", buffer_pool "
", tablespace_name "
"from all_clusters "
"where owner in %s "
};

const char cAllClusters::szSingleTableNotReady[] = {"'N' "};
const char cAllClusters::szSingleTableReady[] = {"ltrim(single_table) "};

cAllClusters::cAllClusters(
    cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
    , cUserSegments& oUserSegments
    , cAllTabColumns& oAllTabColumns
    , cHashExpr& oHashExpr
    , cUserIndexes& oUserIndexes
)
    : iNumRows_(0)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
    , oUserSegments_(oUserSegments)
    , oAllTabColumns_(oAllTabColumns)
    , oHashExpr_(oHashExpr)
    , oUserIndexes_(oUserIndexes)
{
    // nothing to do
}

cAllClusters::~cAllClusters()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllClusters::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , iNumRows_
    );
}

void cAllClusters::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllClusters::tAttributes::iKeyMatched
        , &cAllClusters::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
