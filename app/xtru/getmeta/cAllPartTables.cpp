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

#define UNLIMITED_EXTENTS  (INT_MAX-3)
#define PARTNING_TYPE_LEN        (9+1)
#define LOGGING_TYPE_LEN         (7+1)
#define INTERVAL_LEN          (1000+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllPartTables::tAttributes
 * See to study about [ALL_PART_TABLES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-F6EC7236-9A38-44FA-9A17-F118D1E4F64D.htm#REFRN20159)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllPartTables::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the partitioned table
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the partitioned table
    /// - specified one in RANGE, HASH, SYSTEM, LIST, REFERENCE.
	char szPartitioningType[PARTNING_TYPE_LEN];  ///< Type of the partitioning method:
    /// - specified one in RANGE, HASH, SYSTEM, LIST, REFERENCE.
	char szSubpartitioningType[PARTNING_TYPE_LEN];///< Type of the composite partitioning method:
    /// - For interval partitioned tables, the value of this column is always 1048575.
	ub4 iPartitionCount;                   ///< Number of partitions in the table.
    /// - the default number of subpartitions, if specified
	ub4 iDefSubpartitionCount;             ///< For a composite-partitioned table,
	ub4 iPartitioningKeyCount;             ///< Number of columns in the partitioning key
    /// - the number of columns in the subpartitioning key
	ub4 iSubpartitioningKeyCount;          ///< For a composite-partitioned table,
	char szDefTablespaceName[OBJECT_NAME_LEN];///< Default tablespace to be used when adding a partition
	ps::lib::sql::ind_t nDefTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iPctFree;                          ///< Default value of PCTFREE to be used when adding a partition
	ub4 iPctUsed;                          ///< Default value of PCTUSED to be used when adding a partition
    /// - ASSM = Automatic segment space management
    ps::lib::sql::ind_t nPctUsedInd;       ///< equal ind_t::VAL_IS_NULL in ASSM
	ub4 iIniTrans;                         ///< Default value of INITRANS to be used when adding a partition
	ub4 iMaxTrans;                         ///< Default value of MAXTRANS to be used when adding a partition
    /// - YES or NO
	char szLogging[LOGGING_TYPE_LEN];      ///< Default LOGGING attribute to be used when adding a partition:
    ps::lib::sql::ind_t nLoggingInd;       ///< ind_t::VAL_IS_NULL: value is NULL
    /// - or 0 if no INITIAL value was specified
	ub4 iDefInitialExtent;             ///< Default value of INITIAL (in Oracle blocks) to be used when adding a partition,
    /// - or 0 if no NEXT value was specified
	ub4 iDefNextExtent;                ///< Default value of NEXT (in Oracle blocks) to be used when adding a partition,
    /// -  or 0 if no MINEXTENTS value was specified
	ub4 iDefMinExtents;                ///< Default value of MINEXTENTS to be used when adding a partition,
    /// - or 0 if no MAXEXTENTS value was specified
	ub4 iDefMaxExtents;                ///< Default value of MAXEXTENTS to be used when adding a partition,
    /// - or DEFAULT if no PCTINCREASE value was specified
	ub4 iDefPctIncrease;               ///< Default value of PCTINCREASE to be used when adding a partition,
	ub4 iDefFreelists;                 ///< Default value of FREELISTS to be used when adding a partition
    ps::lib::sql::ind_t nDefFreelists; ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iDefFreelistGroups;            ///< Default value of FREELIST GROUPS to be used when adding a partition
    ps::lib::sql::ind_t nDefFreelistGroups;///< ind_t::VAL_IS_NULL: value is NULL
    /// - "ENABLED","DISABLED" or "NONE"
	char szDefCompression[COMPRES_LEN];    ///< Default compression to be used when adding a partition:
    /// - BASIC, ADVANCED, QUERY LOW, QUERY HIGH, ARCHIVE LOW, ARCHIVE HIGH, UNKNOWN
	char szDefCompressFor[COMPFOR_LEN];    ///< Default compression for what kind of operations to be used when adding a partition:
    ps::lib::sql::ind_t nDefCompressForInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szDefBufferPool[BUFFER_POOL_LEN]; ///< Default buffer pool to be used when adding a partition:
	char szRefPtnConstraintName[OBJECT_NAME_LEN];///< Name of the partitioning referential constraint for reference-partitioned tables
    ps::lib::sql::ind_t nRefPtnConstraintNameInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szInterval[INTERVAL_LEN];         ///< String of the interval value
    ps::lib::sql::ind_t nIntervalInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	char long_str[18];                     ///< Function dependence on tAttributes::iDefMaxExtents
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iPartitionCount(0)
        , iDefSubpartitionCount(0)
        , iPartitioningKeyCount(0)
        , iSubpartitioningKeyCount(0)
        , nDefTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctFree(0)
        , iPctUsed(0)
        , nPctUsedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iIniTrans(0)
        , iMaxTrans(0)
        , nLoggingInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iDefInitialExtent(0)
        , iDefNextExtent(0)
        , iDefMinExtents(0)
        , iDefMaxExtents(0)
        , iDefPctIncrease(0)
        , iDefFreelists(0)
        , nDefFreelists(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iDefFreelistGroups(0)
        , nDefFreelistGroups(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nDefCompressForInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nRefPtnConstraintNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIntervalInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szPartitioningType, 0, sizeof(szPartitioningType));
        ::memset(szSubpartitioningType, 0, sizeof(szSubpartitioningType));
        ::memset(szDefTablespaceName, 0, sizeof(szDefTablespaceName));
        ::memset(szLogging, 0, sizeof(szLogging));
        ::memset(szDefCompression, 0, sizeof(szDefCompression));
        ::memset(szDefCompressFor, 0, sizeof(szDefCompressFor));
        ::memset(szDefBufferPool, 0, sizeof(szDefBufferPool));
        ::memset(szRefPtnConstraintName, 0, sizeof(szRefPtnConstraintName));
        ::memset(szInterval, 0, sizeof(szInterval));
        ::memset(long_str, 0, sizeof(long_str));
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
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vFullPrinter(std::ostream& os, const size_t& i, cAllPartTables& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vStoragePrinter(std::ostream& os, const size_t& i, cAllPartTables& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllPartTables::tAttributes::vFullPrinter(std::ostream& os, const size_t&, cAllPartTables& oCtx) const
{
    using ps::lib::sBinIntToIntStr;
    using ps::lib::iCompBinary;
    const uint32_t iDbBlockSize = ps::lib::sql::occi::cSvc::iGetDbBlockSize();
    os << std::string("  storage (");
    if (iDefInitialExtent)
    {
        os << boost::format("initial %s ")
            % sBinIntToIntStr(iDefInitialExtent * iDbBlockSize, "");
    }
    if (iDefNextExtent)
    {
        os << boost::format("next %s ")
            % sBinIntToIntStr(iDefNextExtent * iDbBlockSize, "");
    }
    if (iDefMinExtents)
    {
        os << boost::format("minextents %u ") % iDefMinExtents;
    }
    if (iDefMaxExtents)
    {
        os << boost::format("maxextents %s ") % long_str
            << std::endl << std::string(2, ' ');
    }
    if (iDefPctIncrease)
    {
        os << boost::format("pctincrease %u ") % iDefPctIncrease;
    }
    if (iDefFreelists)
    {
        os << boost::format("freelists %u ") % iDefFreelists;
    }
    if (iDefFreelistGroups)
    {
        os << boost::format("freelist groups %u ") % iDefFreelistGroups;
    }
    os << boost::format("buffer_pool %s) ") % szDefBufferPool;

    if (nDefTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szDefTablespaceName);
    }
    // Compress option.
    // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129627
    cCompressObject oCompressObject(szDefCompression, szDefCompressFor, nDefCompressForInd);
    os << oCompressObject.sGetClause();
    os << std::endl;
    os << boost::format("partition by %s ") % szPartitioningType;
    if (iCompBinary(strcmp, szPartitioningType, "REFERENCE"))
    {
        os << boost::format("(%s)") % ps::lib::sMakeEnclosedName(szRefPtnConstraintName)
            << std::endl;
    }
    else
    {
        // prints partitioning key list.
        os << oPrintItems(oCtx.oAllPartKeyColumns_, std::make_tuple(szOwner, szTableName), true);
    }
    {
        char szQuantityPrefix[80];
        // interval partition
        if (nIntervalInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
           ::sprintf(szQuantityPrefix, "interval (%s)", szInterval);
           oCtx.oAllTabPartitions_.vRptTabPartByQuantity(
               os, std::make_tuple(szOwner, szTableName), true, szQuantityPrefix
           );
        }
        // Representation by hash_partitions_by_quantity
        if (iCompBinary(strcmp, szPartitioningType, "HASH"))
        {
           ::sprintf(szQuantityPrefix, "partitions %u", iPartitionCount);
           oCtx.oAllTabPartitions_.vRptTabPartByQuantity(
               os, std::make_tuple(szOwner, szTableName), true, szQuantityPrefix
           );
        }
    }
    if ( ! iCompBinary(strcmp, szSubpartitioningType, "NONE"))
    {
        // composite partitioning
        os << boost::format("subpartition by %s ") % szSubpartitioningType
            // subpartitioning key.
            << oPrintItems(
                oCtx.oAllSubPartKeyColumns_, std::make_tuple(szOwner, szTableName), true
            );
        // Representation by subpartition_template
        if (oCtx.oAllSubPartitionTemplates_) // this is a reference to the smart-pointer.
        {
            os << oPrintItems(
                *oCtx.oAllSubPartitionTemplates_, std::make_tuple(szOwner, szTableName), true
                , std::string(szSubpartitioningType)  // RANGE or LIST and HASH
            );
        }
        // Representation by hash_subpartitions_by_quantity
        if (iCompBinary(strcmp, szSubpartitioningType, "HASH"))
        {
            oCtx.oAllTabSubPartitions_.vRptTabSubPartByQuantity(
                os, std::make_tuple(
                    szOwner, szTableName
                    , "this will not be used", "this will not be used"
                ), true
                , iDefSubpartitionCount
            );
        }
    }
    // Description of the partition
    oCtx.oAllTabPartitions_.vRptAllTabPartitions(
        os
        , std::make_tuple(szOwner, szTableName), true
        , szPartitioningType, szSubpartitioningType, szDefTablespaceName
        , &oCompressObject
    );
}

void cAllPartTables::tAttributes::vStoragePrinter(std::ostream& os, const size_t&, cAllPartTables&) const
{
    if (iPctFree)
    {
        os << boost::format("pctfree %d ") % iPctFree;
    }
    if (nPctUsedInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("pctused %d ") % iPctUsed;
    }
    if (iIniTrans)
    {
        os << boost::format("initrans %d ") % iIniTrans;
    }
    if (iMaxTrans)
    {
        os << boost::format("maxtrans %d ") % iMaxTrans;
    }
    os << std::string(
        ps::lib::iCompBinary(strcmp, szLogging, "NO")
        ? "nologging " : "" 
    );
}

std::string cAllPartTables::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szPartitioningType='%s', szSubpartitioningType='%s')"
        R"(, iPartitionCount=%u, iDefSubpartitionCount=%u, iPartitioningKeyCount=%u)"
        R"(, iSubpartitioningKeyCount=%u, szDefTablespaceName='%s'(%d))"
        R"(, iPctFree=%u, iPctUsed=%u(%d), iIniTrans=%u, iMaxTrans=%u, szLogging='%s'(%d))"
        R"(, iDefInitialExtent=%u, iDefNextExtent=%u, iDefMinExtents=%u, iDefMaxExtents=%u,iDefPctIncrease=%u)"
        R"(, iDefFreelists=%u(%d), iDefFreelistGroups=%u(%d))"
        R"(, szDefCompression='%s', szDefCompressFor='%s'(%d), szDefBufferPool='%s')"
        R"(, szRefPtnConstraintName='%s'(%d), szInterval='%s'(%d))"
    ) % szOwner % szTableName % szPartitioningType % szSubpartitioningType
        % iPartitionCount % iDefSubpartitionCount % iPartitioningKeyCount
        % iSubpartitioningKeyCount % szDefTablespaceName % nDefTablespaceNameInd
        % iPctFree % iPctUsed % nPctUsedInd % iIniTrans % iMaxTrans % szLogging % nLoggingInd
        % iDefInitialExtent % iDefNextExtent % iDefMinExtents % iDefMaxExtents % iDefPctIncrease
        % iDefFreelists % nDefFreelists % iDefFreelistGroups % nDefFreelistGroups
        % szDefCompression %szDefCompressFor % nDefCompressForInd % szDefBufferPool
        % szRefPtnConstraintName % nRefPtnConstraintNameInd % szInterval % nIntervalInd
    ;
    return oss.str();
}

/**
 * @struct cAllPartTables::cRetriever
 */
class cAllPartTables::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllPartTables::oList_.
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

const uint32_t cAllPartTables::cRetriever::iBulkSize_ = 100;

cAllPartTables::tAttributes cAllPartTables::cRetriever::rTable_[cAllPartTables::cRetriever::iBulkSize_];

cAllPartTables::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllPartTables
        , "Reading attribute of partition tables", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    const auto later_9ir2 = oSvc.iCompDbVersion("0902",4) >= 0; // true: equal or later.
    const auto later_11g1 = oSvc.iCompDbVersion("1101",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_9ir2 ? cAllPartTables::szComprsReady : cAllPartTables::szComprsNotReady
        , later_11g1 ? cAllPartTables::szCompForReady : cAllPartTables::szCompForNotReady
        , later_11g1 ? cAllPartTables::szMoreThan_11g : cAllPartTables::szMoreThan_8i
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitioningType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubpartitioningType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitionCount, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefSubpartitionCount, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitioningKeyCount, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSubpartitioningKeyCount, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefTablespaceName, SQLT_STR, &rTable_->nDefTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctFree, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctUsed, SQLT_UIN, &rTable_->nPctUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iIniTrans, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxTrans, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogging, SQLT_STR, &rTable_->nLoggingInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefInitialExtent, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefNextExtent, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefMinExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefMaxExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefPctIncrease, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefFreelists, SQLT_UIN, &rTable_->nDefFreelists, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefFreelistGroups, SQLT_UIN, &rTable_->nDefFreelistGroups, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefCompression, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefCompressFor, SQLT_STR, &rTable_->nDefCompressForInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefBufferPool, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRefPtnConstraintName, SQLT_STR, &rTable_->nRefPtnConstraintNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInterval, SQLT_STR, &rTable_->nIntervalInd, NULL, NULL, iSkip_);
}

void cAllPartTables::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllPartTables::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
	for (uint32_t i = 0; i < iNumIter; i++){
		if (rTable_[i].iDefMaxExtents > UNLIMITED_EXTENTS)
        {
			::strcpy(rTable_[i].long_str, "unlimited");
		}
        else
        {
			::sprintf(rTable_[i].long_str, "%d", rTable_[i].iDefMaxExtents);
		}
	}
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllPartTables::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllPartTables::szAllPartTables[] = {
"select t1.owner "
", t1.table_name "
", t1.partitioning_type "
", t1.subpartitioning_type "
", t1.partition_count "
", t1.def_subpartition_count "
", t1.partitioning_key_count "
", t1.subpartitioning_key_count "
", t1.def_tablespace_name "
", t1.def_pct_free "
", t1.def_pct_used "
", t1.def_ini_trans "
", t1.def_max_trans "
", t1.def_logging "
", decode(t1.def_initial_extent, 'DEFAULT', 0, to_number(t1.def_initial_extent)) as def_initial_extent "
", decode(t1.def_next_extent, 'DEFAULT', 0, to_number(t1.def_next_extent)) as def_next_extent "
", decode(t1.def_min_extents, 'DEFAULT', 0, to_number(t1.def_min_extents)) as def_min_extents "
", decode(t1.def_max_extents, 'DEFAULT', 0, to_number(t1.def_max_extents)) as def_max_extents "
", decode(t1.def_pct_increase, 'DEFAULT', 0, to_number(t1.def_pct_increase)) as def_pct_increase "
", t1.def_freelists "
", t1.def_freelist_groups "
", %s " // def_compression
", %s " // def_compress_for
", t1.def_buffer_pool "
", %s " // ref_ptn_constraint_name, interval
"from all_part_tables t1 "
"where t1.owner in %s "
"order by t1.table_name "
};

const char cAllPartTables::szComprsReady[] = { "t1.def_compression" };
const char cAllPartTables::szComprsNotReady[] = { "'NONE' as def_compression" };
const char cAllPartTables::szCompForReady[] = { "t1.def_compress_for" };
const char cAllPartTables::szCompForNotReady[] = { "null as def_compress_for" };
const char cAllPartTables::szMoreThan_8i[] = {
"null as ref_ptn_constraint_name"
", null as interval"
};
const char cAllPartTables::szMoreThan_11g[] = {
"ref_ptn_constraint_name"
", interval"
};

cAllPartTables::cAllPartTables(
    cAllTabSubPartitions& oAllTabSubPartitions
    , cAllTabPartitions& oAllTabPartitions
    , cAllPartKeyColumns& oAllPartKeyColumns
    , cAllSubPartKeyColumns& oAllSubPartKeyColumns
    , std::shared_ptr<cAllSubPartitionTemplates>& oAllSubPartitionTemplates
)
    : oAllTabSubPartitions_(oAllTabSubPartitions)
    , oAllTabPartitions_(oAllTabPartitions)
    , oAllPartKeyColumns_(oAllPartKeyColumns)
    , oAllSubPartKeyColumns_(oAllSubPartKeyColumns)
    , oAllSubPartitionTemplates_(oAllSubPartitionTemplates)
{
    // nothing to do
}

cAllPartTables::~cAllPartTables()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllPartTables::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllPartTables::vPrintFullItems(
    std::ostream& os
    , const tKeyTuple& oKey
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllPartTables::tAttributes::iKeyMatched
        , &cAllPartTables::tAttributes::vFullPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
    boost::for_each(
        oArr, [this](tIter it){oList_.erase(it);}
    );
}

void cAllPartTables::vPrintStorageItems(
    std::ostream& os
    , const tKeyTuple& oKey
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllPartTables::tAttributes::iKeyMatched
        , &cAllPartTables::tAttributes::vStoragePrinter
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
