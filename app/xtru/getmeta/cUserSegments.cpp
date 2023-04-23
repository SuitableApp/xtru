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

#define UNLIMITED_EXTENTS  INT_MAX-3 /// Number of maximum extent.
#define SEGMENT_NAME_LEN   81+1      /// Size of segment name.
#define MAX_EXTENTS_LEN    12

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cUserSegments::tAttributes
 * See to study about [DBA_SEGMENTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-7BF4F8C7-1917-4532-B048-C044E17A1754.htm#REFRN23243 "DBA_SEGMENTS")
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cUserSegments::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];        ///< Name of segment owner.
	char szSegmentName[SEGMENT_NAME_LEN]; ///< Name of segment.
	char szPartitionName[OBJECT_NAME_LEN];///< Name of partition if object is partitioned.
	ps::lib::sql::ind_t nPartitionNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    /**  Type of segment. Variation of types are:
     * - NESTED TABLE
     * - TABLE
     * - TABLE PARTITION
     * - CLUSTER
     * - LOBINDEX
     * - INDEX
     * - INDEX PARTITION
     * - LOBSEGMENT
     * - TABLE SUBPARTITION
     * - INDEX SUBPARTITION
     * - LOB PARTITION
     * - LOB SUBPARTITION
     * - ROLLBACK
     * - TYPE2 UNDO
     * - DEFERRED ROLLBACK
     * - TEMPORARY
     * - CACHE
     * - SPACE HEADER
     * - UNDEFINED
     */
	char szSegmentType[OBJECT_TYPE_LEN];
	char szTablespaceName[OBJECT_NAME_LEN]; ///< Name of tablespace.
	uint32_t ikBytes;                     ///< Size of segment in kbytes.
	uint32_t ikInitialExtent;             ///< Size of initial extent in kbytes.
	uint32_t ikNextExtent;                ///< Size of next extent in kbytes.
	ps::lib::sql::ind_t nkNextExtentInd;  ///< ind_t::VAL_IS_NULL: value is NULL
	uint32_t iMinExtents;                 ///< Number of minimum extents.
	uint32_t iMaxExtents;                 ///< Number of maximum extents.
	uint32_t iPctIncrease;                ///< Percentage of increase.
	ps::lib::sql::ind_t nPctIncreaseInd;  ///< ind_t::VAL_IS_NULL: value is NULL
	uint32_t iFreelists;                  ///< Number of free lists.
	ps::lib::sql::ind_t nFreelistsInd;    ///< ind_t::VAL_IS_NULL: value is NULL
	uint32_t iFreelistGroups;             ///< Number of free list groups.
	ps::lib::sql::ind_t nFreelistGroupsInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szBufferPool[BUFFER_POOL_LEN];   ///< Name of belonging buffer pool.
	// Derived attributes.
    /// sIZE OF INITIAL EXtent in kbytes.
    /// if cUserSegments::iCompress_ is true, the total size of all the extents is stored.
	uint32_t i_k_initiaiBytes;
    /// Size of next extent in kbytes.
    /// if cUserSegments::iCompress_ is true, next extent size recalculated
    /// with tAttributes::iPctIncrease is stored
	uint32_t i_k_next_bytes;
    /// if cUserSegments::iCompress_ is true, one is storead.
	uint32_t i_min_extents;
    char szMaxExtents[MAX_EXTENTS_LEN];
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nPartitionNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , ikBytes(0)
        , ikInitialExtent(0)
        , ikNextExtent(0)
        , nkNextExtentInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iMinExtents(0)
        , iMaxExtents(0)
        , iPctIncrease(0)
        , nPctIncreaseInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelists(0)
        , nFreelistsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelistGroups(0)
        , nFreelistGroupsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , i_k_initiaiBytes(0)
        , i_k_next_bytes(0)
        , i_min_extents(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szSegmentName, 0, sizeof(szSegmentName));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szSegmentType, 0, sizeof(szSegmentType));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szBufferPool, 0, sizeof(szBufferPool));
        ::memset(szMaxExtents, 0, sizeof(szMaxExtents));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szSegmentName, std::get<1>(oKey));
        ::strcpy(szSegmentType, std::get<2>(oKey));
        ::strcpy(szPartitionName, std::get<3>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", "", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szSegmentName, rhs.szSegmentName
            , szSegmentType, rhs.szSegmentType
            , szPartitionName, rhs.szPartitionName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szSegmentName, rhs.szSegmentName
            , szSegmentType, rhs.szSegmentType
            , szPartitionName, rhs.szPartitionName
        );
    }
    /**
     * @brief
     * For the long type description of the strage attribute.
     * For the description of LOBSEGMENT.
     */
    void vPrintFull(std::ostream& os) const;
    /**
     * @brief
     * For the short type description of the strage attribute.
     */
    void vPrintShort(std::ostream& os) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserSegments& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cUserSegments::tAttributes::vPrintFull(std::ostream& os) const
{
    os << boost::format(R"(tablespace "%s")") % szTablespaceName;
    if (nPartitionNameInd == ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        os << std::endl << boost::format("  storage (initial %s ")
            % ps::lib::sBinIntToIntStr(i_k_initiaiBytes * 1024, "");
        if (nkNextExtentInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("next %s ")
                % ps::lib::sBinIntToIntStr(i_k_next_bytes * 1024, "");
        }
        os << boost::format("minextents %d maxextents %s ") % i_min_extents % szMaxExtents
            << std::endl << std::string("  ");
        if (nPctIncreaseInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("pctincrease %d ") % iPctIncrease;
        }
        if (nFreelistsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("freelists %d ") % iFreelists;
        }
        if (nFreelistGroupsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("freelist groups %d ") % iFreelistGroups;
        }
        os << boost::format("buffer_pool %s)") % szBufferPool;
    }
}

void cUserSegments::tAttributes::vPrintShort(std::ostream& os) const
{
    os << boost::format("initial %s ")
        % ps::lib::sBinIntToIntStr(i_k_initiaiBytes * 1024, "");
    if (nkNextExtentInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("next %s ")
            % ps::lib::sBinIntToIntStr(i_k_next_bytes * 1024, "");
    }
    os << boost::format("minextents %d maxextents %s ") % i_min_extents % szMaxExtents
        << std::endl << std::string("  ");
}

void cUserSegments::tAttributes::vPrinter(std::ostream& os, const size_t&, cUserSegments&) const
{
    /**
     * See to study
     * [storage_clause](https://docs.oracle.com/database/121/SQLRF/clauses009.htm#SQLRF30013)
     * 
     */
    if (ps::lib::iCompBinary(strcmp, szSegmentType, "LOBSEGMENT"))
        vPrintFull(os);
    else
        vPrintShort(os);
}

std::string cUserSegments::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szSegmentName='%s', szPartitionName='%s'(%d))"
            R"(, szSegmentType='%s', szTablespaceName='%s')"
            R"(, ikBytes=%u, ikInitialExtent=%u, ikNextExtent=%u(%d))"
            R"(, iMinExtents=%u, iMaxExtents=%u, iPctIncrease=%u(%d))"
            R"(, iFreelists=%u(%d), iFreelistGroups=%u(%d))"
            R"(, szBufferPool='%s')"
            R"(, i_k_initiaiBytes=%u, i_k_next_bytes=%u, i_min_extents=%u)"
            R"(, szMaxExtents='%s')"
        ) % szOwner % szSegmentName % szPartitionName % nPartitionNameInd
            % szSegmentType % szTablespaceName
            % ikBytes % ikInitialExtent % ikNextExtent % nkNextExtentInd
            % iMinExtents % iMaxExtents % iPctIncrease % nPctIncreaseInd
            % iFreelists % nFreelistsInd % iFreelistGroups % nFreelistGroupsInd
            % szBufferPool
            % i_k_initiaiBytes % i_k_next_bytes % i_min_extents
            % szMaxExtents
        ;
    return oss.str();
}

/**
 * @struct cUserSegments::cRetriever
 */
class cUserSegments::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserSegments::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    /**
     * @brief
     * - true: Make all extents unity.
     */
    const bool iCompress_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const bool& iCompress
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
    bool iAccesible() const
    {
        const auto& oPrivsGiven(ps::app::xtru::getmeta::cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_SEGMENTS", "SELECT"});
    }
};

const uint32_t cUserSegments::cRetriever::iBulkSize_ = 1000;

cUserSegments::tAttributes cUserSegments::cRetriever::rTable_[cUserSegments::cRetriever::iBulkSize_];

cUserSegments::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const bool& iCompress
)
    : cDescriber(oSvc, iBulkSize_
        , iAccesible()
            ? szDbaSegments : szUserSegments
        , "Reading attribute of segments", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iCompress_(iCompress)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSegmentName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, &rTable_->nPartitionNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSegmentType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->ikBytes, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->ikInitialExtent, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->ikNextExtent, SQLT_UIN, &rTable_->nkNextExtentInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMinExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxExtents, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctIncrease, SQLT_UIN, &rTable_->nPctIncreaseInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelists, SQLT_UIN, &rTable_->nFreelistsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelistGroups, SQLT_UIN, &rTable_->nFreelistGroupsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBufferPool, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cUserSegments::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserSegments::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    for (uint32_t i = 0; i < iNumIter; ++i)
    {
        rTable_[i].i_k_initiaiBytes = iCompress_ ? rTable_[i].ikBytes : rTable_[i].ikInitialExtent;
        rTable_[i].i_k_next_bytes = iCompress_ ? rTable_[i].ikNextExtent : rTable_[i].ikInitialExtent;
        rTable_[i].i_min_extents = iCompress_ ? 1 : rTable_[i].iMinExtents;
        if (rTable_[i].iMaxExtents > UNLIMITED_EXTENTS)
        {
            ::strcpy(rTable_[i].szMaxExtents, "unlimited");
        }
        else
        {
            ::sprintf(rTable_[i].szMaxExtents, "%d", rTable_[i].iMaxExtents);
        }
    }
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserSegments::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cUserSegments::szDbaSegments[] = {
"select owner "
", segment_name "
", partition_name "
", segment_type "
", tablespace_name "
", bytes / 1024 "  // If this column added, physical reads will double.
", initial_extent / 1024 "
", next_extent / 1024 "
", min_extents "
", max_extents "
", pct_increase "
", freelists "
", freelist_groups "
", buffer_pool "
"from dba_segments "
"where owner in %s "
"and segment_type not in  "
"('TABLE PARTITION','TABLE SUBPARTITION'"
",'INDEX PARTITION','INDEX SUBPARTITION'"
")"
};

const char cUserSegments::szUserSegments[] = {
"select USER "
", segment_name "
", partition_name "
", segment_type "
", tablespace_name "
", bytes / 1024 "
", initial_extent / 1024 "
", next_extent / 1024 "
", min_extents "
", max_extents "
", pct_increase "
", freelists "
", freelist_groups "
", buffer_pool "
"from user_segments "
"where user in %s "
"and segment_type not in  "
"('TABLE PARTITION','TABLE SUBPARTITION'"
",'INDEX PARTITION','INDEX SUBPARTITION'"
")"
};

cUserSegments::cUserSegments()
{
    // nothing to do
}

cUserSegments::~cUserSegments()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserSegments::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const bool& iCompress
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , iCompress
    );
}

void cUserSegments::vPrintAllItems(
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
        , &cUserSegments::tAttributes::iKeyMatched
        , &cUserSegments::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cUserSegments::vPrintPostRows
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

void cUserSegments::vPrintPostRows(std::ostream& os, const tAttributes& rKey) const
{
    const static ps::lib::cSet<std::string> oSet{
        "LOBSEGMENT"
//      "CLUSTER", "INDEX", "LOBSEGMENT", "TABLE"
    };
    if (oSet.find(rKey.szSegmentType) != oSet.end())
    {
        os << std::endl;
    }
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

