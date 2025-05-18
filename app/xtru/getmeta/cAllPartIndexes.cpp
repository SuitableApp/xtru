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

constexpr int UNLIMITED_EXTENTS = INT_MAX-3;
constexpr size_t PARTNING_TYPE_LEN = 9+1;
constexpr size_t LOGGING_TYPE_LEN = 7+1;
constexpr size_t ALIGNMENT_LEN = 12+1;
constexpr size_t INTERVAL_LEN = 1000+1;
constexpr size_t LOCALITY_LEN = 6+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllPartIndexes::tAttributes
 * See to study about [ALL_PART_INDEXES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-7077F50B-F021-44C9-AC6D-EB3B6FCF5F15.htm#REFRN20156)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllPartIndexes::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the partitioned index
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the partitioned table
	char szIndexName[OBJECT_NAME_LEN];     ///< Name of the partitioned index
    /// - specified one in RANGE, HASH, SYSTEM, LIST, REFERENCE.
	char szPartitioningType[PARTNING_TYPE_LEN];  ///< Type of the partitioning method:
    /// - specified one in RANGE, HASH, SYSTEM, LIST, REFERENCE.
	char szSubpartitioningType[PARTNING_TYPE_LEN];///< Type of the composite partitioning method:
    /// - For interval partitioned tables, the value of this column is always 1048575.
	ub4 iPartitionCount;                   ///< Number of partitions in the index.
    /// - the default number of subpartitions, if specified
	ub4 iDefSubpartitionCount;             ///< For a composite-partitioned index,
	ub4 iPartitioningKeyCount;             ///< Number of columns in the partitioning key
    /// - the number of columns in the subpartitioning key
	ub4 iSubpartitioningKeyCount;          ///< For a composite-partitioned index,
	char szLocality[LOCALITY_LEN];         ///< Indicates whether the partitioned index is local (LOCAL) or global (GLOBAL)
	char szAlignment[ALIGNMENT_LEN];       ///< Indicates whether the partitioned index is prefixed (PREFIXED) or non-prefixed (NON_PREFIXED)
	char szDefTablespaceName[OBJECT_NAME_LEN];///< For a local index, the default tablespace to be used when adding or splitting a table partition
	ps::lib::sql::ind_t nDefTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    /// - to be used when adding a table partition, or DEFAULT if no INITIAL value was specified
	ub4 iDefInitialExtent;             ///< For a local index, the default INITIAL value (in Oracle blocks)
    /// - to be used when adding a table partition, or DEFAULT if no NEXT value was specified
	ub4 iDefNextExtent;                ///< For a local index, the default NEXT value (in Oracle blocks)
    /// - to be used when adding a table partition, or DEFAULT if no MINEXTENTS value was specified
	ub4 iDefMinExtents;                ///< For a local index, the default MINEXTENTS value
    /// - to be used when adding a table partition, or DEFAULT if no MAXEXTENTS value was specified
	ub4 iDefMaxExtents;                ///< For a local index, the default MAXEXTENTS value
    /// - to be used when adding a table partition, or DEFAULT if no PCTINCREASE value was specified
	ub4 iDefPctIncrease;               ///< For a local index, the default PCTINCREASE value
	ub4 iDefFreelists;                 ///< For a local index, the default FREELISTS value to be used when adding a table partition
	ub4 iDefFreelistGroups;            ///< For a local index, the default FREELIST GROUPS value to be used when adding a table partition
    /// - DEFAULT, KEEP, RECYCLE or NULL
	char szDefBufferPool[BUFFER_POOL_LEN]; ///< For a local index, the default buffer pool to be used when adding a table partition:
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
        , iDefInitialExtent(0)
        , iDefNextExtent(0)
        , iDefMinExtents(0)
        , iDefMaxExtents(0)
        , iDefPctIncrease(0)
        , iDefFreelists(0)
        , iDefFreelistGroups(0)
        , nIntervalInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szPartitioningType, 0, sizeof(szPartitioningType));
        ::memset(szSubpartitioningType, 0, sizeof(szSubpartitioningType));
        ::memset(szLocality, 0, sizeof(szLocality));
        ::memset(szAlignment, 0, sizeof(szAlignment));
        ::memset(szDefTablespaceName, 0, sizeof(szDefTablespaceName));
        ::memset(szDefBufferPool, 0, sizeof(szDefBufferPool));
        ::memset(szInterval, 0, sizeof(szInterval));
        ::memset(long_str, 0, sizeof(long_str));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szIndexName, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllPartIndexes& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllPartIndexes::tAttributes::vPrinter(std::ostream& os, const size_t&, cAllPartIndexes& oCtx) const
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
    os << std::endl;
    if (iCompBinary(strcmp, szLocality, "LOCAL"))
    {
        os << std::string("local") << std::endl;
        if (iCompBinary(strcmp, szSubpartitioningType, "NONE"))
        {
            if (iCompBinary(strcmp, szPartitioningType, "HASH"))
            {
               // Representation by hash_partitions_by_quantity
               // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2125987
               oCtx.oAllIndPartitions_.vRptIndPartByQuantity(
                   os, std::make_tuple(szOwner, szIndexName), true
               );
            }
        }
        else
        {
            // Representation by hash_subparts_by_quantity
            // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#BABIDCDG
            oCtx.oAllIndSubPartitions_.vRptIndSubPartByQuantity(
                os, std::make_tuple(
                    szOwner, szIndexName, "this will not be used"
                ), true
            );
        }
    }
    else
    {
        // Representation by table_partitioning_clauses
        // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129707
        os << boost::format("global partition by %s ") % szPartitioningType
            // prints partitioning key list.
           << oPrintItems(
               oCtx.oAllPartKeyColumns_, std::make_tuple(szOwner, szIndexName), true
           );
        if (! iCompBinary(strcmp, szSubpartitioningType, "NONE"))
        {
            // composite partitioning
            os << boost::format("subpartition by %s ") % szSubpartitioningType
                // subpartitioning key.
                << oPrintItems(oCtx.oAllSubPartKeyColumns_, std::make_tuple(szOwner, szIndexName), true)
                ;
        }
    }
    // Description of the partition
    oCtx.oAllIndPartitions_.vRptAllIndPartitions(
        os
        , std::make_tuple(szOwner, szIndexName), true
        , szPartitioningType, szSubpartitioningType, szDefTablespaceName
        , szLocality
    );
}

std::string cAllPartIndexes::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szIndexName='%s', szPartitioningType='%s', szSubpartitioningType='%s')"
        R"(, iPartitionCount=%u, iDefSubpartitionCount=%u, iPartitioningKeyCount=%u)"
        R"(, iSubpartitioningKeyCount=%u, szLocality='%s', szAlignment='%s', szDefTablespaceName='%s'(%d))"
        R"(, iDefInitialExtent=%u, iDefNextExtent=%u, iDefMinExtents=%u, iDefMaxExtents=%u,iDefPctIncrease=%u)"
        R"(, iDefFreelists=%u, iDefFreelistGroups=%u, szDefBufferPool='%s', szInterval='%s'(%d))"
    ) % szOwner % szTableName % szIndexName % szPartitioningType % szSubpartitioningType
        % iPartitionCount % iDefSubpartitionCount % iPartitioningKeyCount
        % iSubpartitioningKeyCount % szLocality % szAlignment % szDefTablespaceName % nDefTablespaceNameInd
        % iDefInitialExtent % iDefNextExtent % iDefMinExtents % iDefMaxExtents % iDefPctIncrease
        % iDefFreelists % iDefFreelistGroups % szDefBufferPool % szInterval % nIntervalInd
    ;
    return oss.str();
}

/**
 * @struct cAllPartIndexes::cRetriever
 */
class cAllPartIndexes::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllPartIndexes::oList_.
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

const uint32_t cAllPartIndexes::cRetriever::iBulkSize_ = 100;

cAllPartIndexes::tAttributes cAllPartIndexes::cRetriever::rTable_[cAllPartIndexes::cRetriever::iBulkSize_];

cAllPartIndexes::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllPartIndexes
        , "Reading attribute of partition indexes", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    const auto later_11g1 = oSvc.iCompDbVersion("1101",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_11g1 ? cAllPartIndexes::szMoreThan_11g : cAllPartIndexes::szMoreThan_8i
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitioningType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szSubpartitioningType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitionCount, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iDefSubpartitionCount, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitioningKeyCount, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iSubpartitioningKeyCount, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szLocality, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szAlignment, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDefTablespaceName, SQLT_STR, &rTable_->nDefTablespaceNameInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iDefInitialExtent, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefNextExtent, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefMinExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefMaxExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefPctIncrease, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefFreelists, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iDefFreelistGroups, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefBufferPool, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInterval, SQLT_STR, &rTable_->nIntervalInd, NULL, NULL, iSkip_);
}

void cAllPartIndexes::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllPartIndexes::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
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

void cAllPartIndexes::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllPartIndexes::szAllPartIndexes[] = {
"select owner "
", table_name "
", index_name "
", partitioning_type "
", subpartitioning_type "
", partition_count "
", def_subpartition_count "
", partitioning_key_count "
", subpartitioning_key_count "
", locality "
", alignment "
", def_tablespace_name "
", decode(def_initial_extent, 'DEFAULT', 0, to_number(def_initial_extent)) as def_initial_extent "
", decode(def_next_extent, 'DEFAULT', 0, to_number(def_next_extent)) as def_next_extent "
", decode(def_min_extents, 'DEFAULT', 0, to_number(def_min_extents)) as def_min_extents "
", decode(def_max_extents, 'DEFAULT', 0, to_number(def_max_extents)) as def_max_extents "
", decode(def_pct_increase, 'DEFAULT', 0, to_number(def_pct_increase)) as def_pct_increase "
", def_freelists "
", def_freelist_groups "
", def_buffer_pool "
", %s "
"from all_part_indexes "
"where owner in %s "
};

const char cAllPartIndexes::szMoreThan_8i[] = {
"null as interval"
};
const char cAllPartIndexes::szMoreThan_11g[] = {
"interval"
};

cAllPartIndexes::cAllPartIndexes(
    cAllIndSubPartitions& oAllIndSubPartitions
    , cAllIndPartitions& oAllIndPartitions
    , cAllPartKeyColumns& oAllPartKeyColumns
    , cAllSubPartKeyColumns& oAllSubPartKeyColumns
)
    : oAllIndSubPartitions_(oAllIndSubPartitions)
    , oAllIndPartitions_(oAllIndPartitions)
    , oAllPartKeyColumns_(oAllPartKeyColumns)
    , oAllSubPartKeyColumns_(oAllSubPartKeyColumns)
{
    // nothing to do
}

cAllPartIndexes::~cAllPartIndexes()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllPartIndexes::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllPartIndexes::vPrintAllItems(
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
        , &cAllPartIndexes::tAttributes::iKeyMatched
        , &cAllPartIndexes::tAttributes::vPrinter
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
