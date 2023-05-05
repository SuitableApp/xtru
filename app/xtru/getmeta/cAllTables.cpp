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

#define MINIMUM_INI_TRANS           2
#define LOGGING_TYPE_LEN         (7+1)
#define CACHE_TYPE_LEN           (1+1)
#define ROW_MOVEMENT_LEN         (8+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllTables::tAttributes
 * See to study about [ALL_TABLES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-6823CD28-0681-468E-950B-966C6F71325D.htm#REFRN20286)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTables::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the table
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table
	char szPartitioned[PARTITIONED_LEN];   ///< Indicates whether the table is partitioned (YES) or not (NO)
	char szClusterName[OBJECT_NAME_LEN];   ///< Name of the cluster, if any, to which the table belongs
    ps::lib::sql::ind_t nClusterNameInd;   ///< 
    /// - if any, to which the overflow or mapping table entry belongs.
    /// - If the tAttributes::nIotTypeInd is not VAL_IS_NULL, then this column contains the base table name.
	char szIotName[OBJECT_NAME_LEN];       ///< Name of the index-organized table (IOT),
    ps::lib::sql::ind_t nIotNameInd;       ///< 
	ub4 iPctFree;                          ///< Minimum percentage of free space in a block
    ps::lib::sql::ind_t nPctFreeInd;       ///< VAL_IS_NULL for partitioned tables
	ub4 iPctUsed;                          ///< Minimum percentage of used space in a block
    ps::lib::sql::ind_t nPctUsedInd;       ///< VAL_IS_NULL for partitioned tables
	ub4 iIniTrans;                         ///< Initial number of transactions
    ps::lib::sql::ind_t nIniTransInd;      ///< VAL_IS_NULL for partitioned tables
	ub4 iMaxTrans;                         ///< Maximum number of transactions
    ps::lib::sql::ind_t nMaxTransInd;      ///< VAL_IS_NULL for partitioned tables
    /// - YES or NO
	char szLogging[LOGGING_TYPE_LEN];      ///< Indicates whether or not changes to the table are logged
    ps::lib::sql::ind_t nLoggingInd;       ///< VAL_IS_NULL for partitioned tables:
	ub4 iPctIncrease;                      ///< Percentage increase in extent size
    ps::lib::sql::ind_t nPctIncreaseInd;   ///< VAL_IS_NULL for partitioned tables
	ub4 iFreelists;                        ///< Number of process freelists allocated to the segment
    ps::lib::sql::ind_t nFreelistsInd;     ///< VAL_IS_NULL for partitioned tables
	ub4 iFreelistGroups;                   ///< Number of freelist groups allocated to the segment
    ps::lib::sql::ind_t nFreelistGroupsInd;///< VAL_IS_NULL for partitioned tables
    /// - DEFAULT, KEEP or RECYCLE.
	char szBufferPool[BUFFER_POOL_LEN];    ///< Buffer pool for the table. Indicates one of:
    ps::lib::sql::ind_t nBufferPoolInd;    ///< NULL for partitioned tables:
	char szDegree[DEGREE_LEN];             ///< Number of threads per instance for scanning the table, or DEFAULT
	char szInstances[INSTANCES_LEN];       ///< Number of instances across which the table is to be scanned, or DEFAULT
	char szCache[CACHE_TYPE_LEN];          ///< Indicates whether the table is to be cached in the buffer cache (Y) or not (N)
    /// - IOT, IOT_OVERFLOW, or IOT_MAPPING.
	char szIotType[IOT_TYPE_LEN];          ///< If the table is an index-organized table, this indicates one of:
    ps::lib::sql::ind_t nIotTypeInd;       ///< NULL indicates the table is not an index-organized table.
	char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the table
	ps::lib::sql::ind_t nTablespaceNameInd;///< NULL for partitioned, temporary, and index-organized tables
	char szRowMovement[ROW_MOVEMENT_LEN];  ///< Indicates whether partitioned row movement is enabled (ENABLED) or disabled (DISABLED)
	ps::lib::sql::ind_t nRowMovementInd;   ///< NULL indicates that DB version is less than 11g
	char szCompression[COMPRES_LEN];       ///< Indicates whether table compression is enabled (ENABLED) or not (DISABLED)
	ps::lib::sql::ind_t nCompressionInd;   ///< NULL for partitioned tables
    /**
     * - See to study [Table Compression](https://docs.oracle.com/database/121/CNCPT/tablecls.htm#GUID-54EE5325-0894-4869-B3AD-8912D9B4A329)
     *   - BASIC
     *   - ADVANCED
     * - See to study [Hybrid Columnar Compression](https://docs.oracle.com/database/121/CNCPT/tablecls.htm#CNCPT89198)
     *   - QUERY LOW                      
     *   - QUERY HIGH                     
     *   - ARCHIVE LOW                    
     *   - ARCHIVE HIGH                   
     *   - QUERY LOW ROW LEVEL LOCKING    
     *   - QUERY HIGH ROW LEVEL LOCKING   
     *   - ARCHIVE LOW ROW LEVEL LOCKING  
     *   - ARCHIVE HIGH ROW LEVEL LOCKING 
     *   - NO ROW LEVEL LOCKING
     */
	char szCompressFor[COMPFOR_LEN];       ///< Default compression for what kind of operations:
	ps::lib::sql::ind_t nCompressForInd;   ///< 
	char szSnapName[OBJECT_NAME_LEN];      ///< 
	ps::lib::sql::ind_t nSnapNameInd;      ///< 
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nClusterNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIotNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctFree(0)
        , nPctFreeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctUsed(0)
        , nPctUsedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iIniTrans(0)
        , nIniTransInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iMaxTrans(0)
        , nMaxTransInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nLoggingInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctIncrease(0)
        , nPctIncreaseInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelists(0)
        , nFreelistsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelistGroups(0)
        , nFreelistGroupsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nBufferPoolInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIotTypeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nRowMovementInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nCompressionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szPartitioned, 0, sizeof(szPartitioned));
        ::memset(szClusterName, 0, sizeof(szClusterName));
        ::memset(szIotName, 0, sizeof(szIotName));
        ::memset(szLogging, 0, sizeof(szLogging));
        ::memset(szBufferPool, 0, sizeof(szBufferPool));
        ::memset(szDegree, 0, sizeof(szDegree));
        ::memset(szInstances, 0, sizeof(szInstances));
        ::memset(szCache, 0, sizeof(szCache));
        ::memset(szIotType, 0, sizeof(szIotType));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szRowMovement, 0, sizeof(szRowMovement));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::memset(szCompressFor, 0, sizeof(szCompressFor));
        ::memset(szSnapName, 0, sizeof(szSnapName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// Search for the overflow segment on the OIT.
    bool iIotOvfComp(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            // szIotName is not a mistake.
            // It is intended not to be szTableName.
            , szIotName , rhs.szTableName
        ) && ps::lib::iCompBinary(strcmp, szIotType, "IOT_OVERFLOW");
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vFullPrinter(std::ostream& os, const size_t& i, cAllTables& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vIotOvfPrinter(std::ostream& os, const size_t& i, cAllTables& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllTables::tAttributes::vFullPrinter(std::ostream& os, const size_t&, cAllTables& oCtx) const
{
    using ps::lib::iCompBinary;
    if (iCompBinary(strcmp, szPartitioned, "NO")) // Regular table
    {
        if (nIotTypeInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL) { return; }
        os << boost::format("pctfree %u ") % iPctFree;
        if (nPctUsedInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("pctused %u ") % iPctUsed;
        }
        os << boost::format("initrans %u maxtrans %u %s")
            % (nSnapNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL && iIniTrans < MINIMUM_INI_TRANS
			 ? MINIMUM_INI_TRANS : iIniTrans)
            % iMaxTrans
            % (iCompBinary(strcmp, szLogging, "NO") ? "nologging " : "")
        ;
    }
    else // Partitiond table
    {
        oCtx.oAllPartTables_.vPrintStorageItems(os, std::make_tuple(szOwner, szTableName));
    }
    if (iCompBinary(strcmp, szPartitioned, "NO")) // Regular table
    {
        os << boost::format("%s parallel (degree %s instances %s) %s")
            % (iCompBinary(strcmp, szCache, "Y") ? "cache" : "nocache")
            % szDegree
            % szInstances
            % (iCompBinary(strcmp, szRowMovement, "ENABLED") ? "enable row movement " : "")
            << std::endl
            << std::string("  storage (")
            << oPrintItems(oCtx.oUserSegments_, std::make_tuple(szOwner, szTableName, "TABLE", ""), true)
        ;
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
        // Compress option.
        // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129627
        cCompressObject oCompressObject(szCompression, szCompressFor, nCompressForInd);
        os << oCompressObject.sGetClause();
        os << std::endl;
    }
    else // Partitiond table
    {
		// descripts table_partitioning_clause.
        // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129707
        oCtx.oAllPartTables_.vPrintFullItems(
            os, std::make_tuple(szOwner, szTableName)
        );
    }
}

void cAllTables::tAttributes::vIotOvfPrinter(std::ostream& os, const size_t&, cAllTables& oCtx) const
{
    using ps::lib::iCompBinary;
    os << boost::format("  overflow /* segment_name = %s */") % szTableName;
    if (iCompBinary(strcmp, szPartitioned, "NO")) // Regular table
    {
        os << std::endl
            << boost::format("  pctfree %u pctused %u initrans %u maxtrans %u %s")
            % iPctFree
            % iPctUsed
            % iIniTrans
            % iMaxTrans
            % (iCompBinary(strcmp, szLogging, "NO") ? "nologging " : "")
            << std::endl
            << std::string("  storage (")
            << oPrintItems(oCtx.oUserSegments_, std::make_tuple(szOwner, szTableName, "TABLE", ""), true)
        ;
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
        // Compress option.
        // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129627
        cCompressObject oCompressObject(szCompression, szCompressFor, nCompressForInd);
        os << oCompressObject.sGetClause();
    }
}

std::string cAllTables::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szPartitioned='%s', szClusterName='%s'(%d), szIotName='%s'(%d))"
        R"(, iPctFree=%u(%d), iPctUsed=%u(%d), iIniTrans=%u(%d), iMaxTrans=%u(%d))"
        R"(, szLogging='%s'(%d), iPctIncrease=%u(%d), iFreelists=%u(%d))"
        R"(, iFreelistGroups=%u(%d), szBufferPool='%s'(%d), szDegree='%s')"
        R"(, szInstances='%s', szCache='%s', szIotType='%s'(%d), szTablespaceName='%s'(%d))"
        R"(, szRowMovement='%s'(%d), szCompression='%s'(%d))"
    ) % szOwner % szTableName % szPartitioned % szClusterName % nClusterNameInd % szIotName % nIotNameInd
        % iPctFree % nPctFreeInd % iPctUsed % nPctUsedInd % iIniTrans % nIniTransInd % iMaxTrans % nMaxTransInd
        % szLogging % nLoggingInd % iPctIncrease % nPctIncreaseInd % iFreelists % nFreelistsInd
        % iFreelistGroups % nFreelistGroupsInd % szBufferPool % nBufferPoolInd % szDegree
        % szInstances % szCache % szIotType % nIotTypeInd % szTablespaceName % nTablespaceNameInd
        % szRowMovement % nRowMovementInd % szCompression % nCompressionInd
    ;
    return oss.str();
}

/**
 * @struct cAllTables::cRetriever
 */
class cAllTables::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTables::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllTables::cRetriever::iBulkSize_ = 1000;

cAllTables::tAttributes cAllTables::cRetriever::rTable_[cAllTables::cRetriever::iBulkSize_];

cAllTables::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTables
        , "Reading attributes of tables", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    const auto later_10g1 = oSvc.iCompDbVersion("1000",4) >= 0; // true: equal or later.
    const auto later_11g1 = oSvc.iCompDbVersion("1101",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_10g1 ? cAllTables::szAllTables_00_r10g : "NULL"
        , later_10g1 ? cAllTables::szComprsReady : cAllTables::szComprsNotReady
        , later_11g1 ? cAllTables::szCompForReady : cAllTables::szCompForNotReady
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitioned, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szClusterName, SQLT_STR, &rTable_->nClusterNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIotName, SQLT_STR, &rTable_->nIotNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctFree, SQLT_UIN, &rTable_->nPctFreeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctUsed, SQLT_UIN, &rTable_->nPctUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iIniTrans, SQLT_UIN, &rTable_->nIniTransInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxTrans, SQLT_UIN, &rTable_->nMaxTransInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogging, SQLT_STR, &rTable_->nLoggingInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctIncrease, SQLT_UIN, &rTable_->nPctIncreaseInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelists, SQLT_UIN, &rTable_->nFreelistsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelistGroups, SQLT_UIN, &rTable_->nFreelistGroupsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBufferPool, SQLT_STR, &rTable_->nBufferPoolInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDegree, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInstances, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCache, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIotType, SQLT_STR, &rTable_->nIotTypeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRowMovement, SQLT_STR, &rTable_->nRowMovementInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompression, SQLT_STR, &rTable_->nCompressionInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompressFor, SQLT_STR, &rTable_->nCompressForInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSnapName, SQLT_STR, &rTable_->nSnapNameInd, NULL, NULL, iSkip_);

}

void cAllTables::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTables::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllTables::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTables::szAllTables[] = {
"select t1.owner "
", t1.table_name "
", t1.partitioned "
", t1.cluster_name "
", t1.iot_name "
", t1.pct_free "
", t1.pct_used "
", t1.ini_trans "
", t1.max_trans "
", t1.logging "
", t1.pct_increase "
", t1.freelists "
", t1.freelist_groups "
", t1.buffer_pool "
", ltrim(t1.degree) as degree "
", ltrim(t1.instances) as instance "
", ltrim(t1.cache)  as cache "
", t1.iot_type "
", t1.tablespace_name "
", %s as row_movement "
", %s " // compression
", %s " // compress_for
", t2.mview_name "
"from all_tables t1 "
", all_mviews t2 "
"where t1.owner in %s "
"and t2.owner (+) = t1.owner "
"and t2.mview_name (+) = t1.table_name "
};

const char cAllTables::szAllTables_00_r10g[] = { "row_movement" };
const char cAllTables::szComprsReady[] = {"decode(t1.compression,'DISABLED','NONE',t1.compression) as compression"};
const char cAllTables::szComprsNotReady[] = {"'NONE' as compression"};
const char cAllTables::szCompForReady[] = {"t1.compress_for"};
const char cAllTables::szCompForNotReady[] = {"null as compress_for"};

cAllTables::cAllTables(
    cUserSegments& oUserSegments
    , cAllPartTables& oAllPartTables
)
    : oUserSegments_(oUserSegments)
    , oAllPartTables_(oAllPartTables)
{
    // nothing to do
}

cAllTables::~cAllTables()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTables::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllTables::vPrintFullItems(
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
        , &cAllTables::tAttributes::iKeyMatched
        , &cAllTables::tAttributes::vFullPrinter
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

void cAllTables::vPrintIotOvfItems(
    std::ostream& os
    , const tKeyTuple& oKey
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllTables::tAttributes::iIotOvfComp
        , &cAllTables::tAttributes::vIotOvfPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
}

bool cAllTables::iCheckClusterTable(
    const tKeyTuple& oKey
    , char *const szClusterName
) const {
    BOOST_ASSERT(szClusterName);
    auto it = oList_.begin(), ite = oList_.end();
    const tAttributes rhs(oKey);
    while (it != ite)
    {
        if (it->iKeyMatched(rhs) && it->nClusterNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            ::strcpy(szClusterName, it->szClusterName);
            return true;
        }
        ++it;
    }
    return false;
}

bool cAllTables::iCheckIot(
    const tKeyTuple& oKey
) const {
    auto it = oList_.begin(), ite = oList_.end();
    const tAttributes rhs(oKey);
    while (it != ite)
    {
        if (it->iKeyMatched(rhs) && it->nIotTypeInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            return true;
        }
        ++it;
    }
    return false;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
