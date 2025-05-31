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

constexpr size_t COMPOSITE_TYP_LEN = 3+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllIndPartitions::tAttributes
 * See to study about [ALL_IND_PARTITIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9E62A85C-CEE9-4DDE-9FDF-C3FA47F51EE5.htm#REFRN20086)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllIndPartitions::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Owner of the index
    char szIndexName[OBJECT_NAME_LEN];     ///< Name of the index
    /// - to a local index on a composite-partitioned table (YES) or not (NO)
    char szComposite[COMPOSITE_TYP_LEN];   ///< Indicates whether the partition belongs
    char szPartitionName[OBJECT_NAME_LEN]; ///< Name of the partition
    /// - If a local index on a composite-partitioned table
    ub4 iSubpartitionCount;                ///< the number of subpartitions in the partition
    ub4 iPartitionPosition;                ///< Position of the partition within the index
    char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the partition
    ps::lib::sql::ind_t nTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    char szGenerated[GENERATED_LEN];       ///< assosiated with all_objects.generated
    /**
     * Indicates whether index compression is enabled or disabled for a partitioned index
     *  NULL for a nonpartitioned index:
     * - NONE: Not specified
     * - ENABLED: The setting for compression is enabled.
     * - DISABLED: The setting for compression is disabled.
     */
    char szCompression[COMPRES_LEN];       ///< Indicates the actual compression property
    char *szHighValue;                     ///< Subpartition bound value expression
    ps::lib::sql::ind_t nHighValueInd;     ///< ind_t::VAL_IS_NULL: value is NULL
    ub4 iHighValue;                        ///< length of the comment body.
    /**
     * - Enter tAttributes::szHighValue and lead cFunctionalReplace::sConvertStringFormat to derive.
     */
    std::string sHighValue;                ///< It is not a variable bound to SQL.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iSubpartitionCount(0)
        , iPartitionPosition(0)
        , nTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , szHighValue(nullptr)
        , nHighValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHighValue(0)
        , sHighValue("")
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szComposite, 0, sizeof(szComposite));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szIndexName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szHighValue which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iSubpartitionCount(rhs.iSubpartitionCount)
        , iPartitionPosition(rhs.iPartitionPosition)
        , nTablespaceNameInd(rhs.nTablespaceNameInd)
        , szHighValue(new char[rhs.iHighValue + 1])
        , nHighValueInd(rhs.nHighValueInd)
        , iHighValue(rhs.iHighValue)
        , sHighValue(rhs.sHighValue)
    {
        std::memset(szOwner, 0, sizeof(szOwner));
        std::memset(szIndexName, 0, sizeof(szIndexName));
        std::memset(szComposite, 0, sizeof(szComposite));
        std::memset(szPartitionName, 0, sizeof(szPartitionName));
        std::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        std::memset(szGenerated, 0, sizeof(szGenerated));
        std::memset(szCompression, 0, sizeof(szCompression));
        std::strncpy(szOwner, rhs.szOwner, sizeof(szOwner) - 1);
        szOwner[sizeof(szOwner) - 1] = '\0';
        std::strncpy(szIndexName, rhs.szIndexName, sizeof(szIndexName) - 1);
        szIndexName[sizeof(szIndexName) - 1] = '\0';
        std::strncpy(szComposite, rhs.szComposite, sizeof(szComposite) - 1);
        szComposite[sizeof(szComposite) - 1] = '\0';
        std::strncpy(szPartitionName, rhs.szPartitionName, sizeof(szPartitionName) - 1);
        szPartitionName[sizeof(szPartitionName) - 1] = '\0';
        std::strncpy(szTablespaceName, rhs.szTablespaceName, sizeof(szTablespaceName) - 1);
        szTablespaceName[sizeof(szTablespaceName) - 1] = '\0';
        std::strncpy(szGenerated, rhs.szGenerated, sizeof(szGenerated) - 1);
        szGenerated[sizeof(szGenerated) - 1] = '\0';
        std::strncpy(szCompression, rhs.szCompression, sizeof(szCompression) - 1);
        szCompression[sizeof(szCompression) - 1] = '\0';
        std::strcpy(szHighValue, rhs.szHighValue);
    }
    ~tAttributes()
    {
        delete [] szHighValue;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szIndexName, rhs.szIndexName
            , iPartitionPosition, rhs.iPartitionPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iFullPartitionName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCompIndPartByQty(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szIndexName, rhs.szIndexName
        )
        // Is the prefix of szPartitionName matched with "SYS_P" ?
        && ps::lib::iCompTernary(
            strncmp, szPartitionName, "SYS_P", 5
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllIndPartitions& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepeatIndPartByQty(std::ostream& os, const size_t& i, cAllIndPartitions& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllIndPartitions::tAttributes::vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllIndPartitions& oCtx) const
{
    BOOST_ASSERT(oCtx.szPartitioningType_);
    BOOST_ASSERT(oCtx.szSubpartitioningType_);
    BOOST_ASSERT(oCtx.szDefTablespaceName_);
    using ps::lib::iCompBinary;
    const bool iIsNamed = iCompBinary(strcmp, szGenerated, "N");
    os << boost::format("%s%spartition %s%s%s ")
        % (i == 0 ? "( " : ", ")
        % sColumnIdCooment(iPartitionPosition)
        % (iIsNamed ? "\"" : "/* ")
        % szPartitionName
        % (iIsNamed ? "\"" : " */")
    ;
    if (
        nHighValueInd != ps::lib::sql::ind_t::VAL_IS_NULL
        && ! iCompBinary(strcmp, oCtx.szLocality_, "LOCAL")
    ){
        os << sGetSubpertitionElement(oCtx.szPartitioningType_, 35, 45, 4, 2, sHighValue);
    }
    else
    {
        // Sub partition is organized HASH partitioning,
        // if nHighValueInd is ps::lib::sql::ind_t::VAL_IS_NULL.
        // There is no additional grammar about the boundary value.
    }
    if (! iCompBinary(strcmp, szTablespaceName, oCtx.szDefTablespaceName_))
    {
        os << boost::format("tablespace %s") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    os << std::endl;
    if (iCompBinary(strcmp, szComposite, "YES"))
    {
        oCtx.oAllIndSubPartitions_.vRptAllIndSubPartitions(
            os
            , std::make_tuple(szOwner, szIndexName, szPartitionName), true
            , oCtx.szSubpartitioningType_, szTablespaceName
        );
    }
}

void cAllIndPartitions::tAttributes::vOnRepeatIndPartByQty(std::ostream& os, const size_t& i, cAllIndPartitions& oCtx) const
{
    auto res = oCtx.oTbsNames_.insert(szTablespaceName);
    if (res.second)
    {
        // oCtx.oTbsNames_ did not have szTablespaceName (so insertion got success).
        const auto sEnclosed = ps::lib::sMakeEnclosedName(szTablespaceName);
        if (i == 0)
        {
            os << boost::format("store in (%s")
                % sEnclosed
            ;
        }
        else
        {
            os << boost::format(", %s")
                % sEnclosed
            ;
        }
    }
}

std::string cAllIndPartitions::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szIndexName='%s', szComposite='%s', szPartitionName='%s'  "
        R"(, iSubpartitionCount=%u,, iPartitionPosition=%u, szTablespaceName='%s'(%s))"
        R"(, szGenerated='%s', szCompression='%s')"
        R"(, iHighValue=%u, szHighValue='%s', sHighValue='%s')"
    ) % szOwner % szIndexName % szComposite % szPartitionName
        % iSubpartitionCount % iPartitionPosition % szTablespaceName % nTablespaceNameInd
        % szGenerated % szCompression
        % iHighValue % szHighValue % sHighValue
    ;
    return oss.str();
}

/**
 * @struct cAllIndPartitions::cRetriever
 */
class cAllIndPartitions::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllIndPartitions::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    ps::lib::sql::occi::cPieceVct pv_;
    const cFunctionalReplace& oFunctionalReplace_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const cFunctionalReplace& oFunctionalReplace
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllIndPartitions::cRetriever::iBulkSize_ = 100;

cAllIndPartitions::tAttributes cAllIndPartitions::cRetriever::rTable_[cAllIndPartitions::cRetriever::iBulkSize_];

cAllIndPartitions::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
)
    : cDescriber(oSvc, iBulkSize_
        , szAllIndPartitions
        , "Reading table partition", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szHighValue, &rTable_->nHighValueInd, &rTable_->iHighValue)
    , oFunctionalReplace_(oFunctionalReplace)
{
    const auto later_9iR2 = oSvc.iCompDbVersion("0902",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_9iR2 ? szComprsReady : szComprsNotReady
        , sGetSqlInList(oOwners)
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szComposite, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iSubpartitionCount, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitionPosition, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szCompression, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
}

void cAllIndPartitions::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllIndPartitions::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    for (uint32_t i = 0; i < iNumIter; ++i)
    {
        if (rTable_[i].nHighValueInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            // To abstract long-lasting expressions.
            rTable_[i].sHighValue = oFunctionalReplace_.sConvertStringFormat(
                rTable_[i].szHighValue, rTable_[i].iHighValue
            );
        }
    }
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllIndPartitions::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllIndPartitions::szAllIndPartitions[] = {
"select ip.index_owner "
", ip.index_name "
", ip.composite "
", ip.partition_name "
", ip.subpartition_count "
", ip.partition_position "
", ip.tablespace_name "
", ao.generated "
", %s "
", ip.high_value "
"from all_ind_partitions ip "
", all_objects ao "
"where ip.index_owner in %s "
"and (ip.index_owner, ip.index_name) in ( "
	/*
     * For global indexes or local indexes, for partition indexes
     * that contain even a single partition whose name or tablespace
     * has changed
	 */
	"select /*+ NO_MERGE NO_UNNEST */ t1.owner, t1.index_name "
	"from all_part_indexes t1 "
	", all_ind_partitions t2 "
	"where t1.owner in %s "
	"and t1.subpartitioning_type = 'NONE' "
	"and t2.index_owner = t1.owner "
	"and t2.index_name = t1.index_name "
	"and not ( "
		"t1.locality = 'LOCAL' "
		"and exists ( "
			"select 1 "
			"from all_tab_partitions "
			"where table_owner = t1.owner "
			"and table_name = t1.table_name "
			"and partition_name = t2.partition_name "
			"and tablespace_name = t2.tablespace_name "
		") "
	") "
") "
"and ao.owner = ip.index_owner "
"and ao.object_name = ip.index_name "
"and ao.subobject_name = ip.partition_name "
"and ao.object_type = 'INDEX PARTITION' "
};

const char cAllIndPartitions::szComprsReady[] = {"nvl(ip.compression,'DISABLED') as compression"};
const char cAllIndPartitions::szComprsNotReady[] = {"'DISABLED' as compression"};

/*
 * In Oracle R8.1.6.3.0, error ORA-00255 will occure.
 * To avoid this failure, use VARCHAR2 column of any of the all_tab_subpartitions table instead of null.
 * In R8.1.7.4.0, It can use null.
 */

cAllIndPartitions::cAllIndPartitions(
    cAllIndSubPartitions& oAllIndSubPartitions
)
    : oAllIndSubPartitions_(oAllIndSubPartitions)
    , szPartitioningType_(nullptr)
    , szSubpartitioningType_(nullptr)
    , szDefTablespaceName_(nullptr)
{
    // nothing to do
}

cAllIndPartitions::~cAllIndPartitions()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllIndPartitions::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , oFunctionalReplace
    );
}

void cAllIndPartitions::vRptAllIndPartitions(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const char *const szPartitioningType
    , const char *const szSubpartitioningType
    , const char *const szDefTablespaceName
	, const char *const szLocality
){
    BOOST_ASSERT(szPartitioningType);
    BOOST_ASSERT(::strlen(szPartitioningType) > 0);
    BOOST_ASSERT(szSubpartitioningType);
    BOOST_ASSERT(::strlen(szSubpartitioningType) > 0);
    BOOST_ASSERT(szDefTablespaceName);
    BOOST_ASSERT(szLocality);
    szPartitioningType_ = szPartitioningType;
    szSubpartitioningType_ = szSubpartitioningType;
    szDefTablespaceName_ = szDefTablespaceName;
    szLocality_ = szLocality;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllIndPartitions::tAttributes::iFullPartitionName
        , &cAllIndPartitions::tAttributes::vOnRepFullPartitionName
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllIndPartitions::vPostRepFullPartitionName
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

void cAllIndPartitions::vRptIndPartByQuantity(
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
        , &cAllIndPartitions::tAttributes::iCompIndPartByQty
        , &cAllIndPartitions::tAttributes::vOnRepeatIndPartByQty
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllIndPartitions::vPostRepeatIndPartByQty
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
    oTbsNames_.clear();
}

void cAllIndPartitions::vPostRepFullPartitionName(std::ostream& os, const tAttributes&) const
{
    os << std::string(") ");
}

void cAllIndPartitions::vPostRepeatIndPartByQty(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
