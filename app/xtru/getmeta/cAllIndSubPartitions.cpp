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

namespace getmeta
{

/**
 * @struct cAllIndSubPartitions::tAttributes
 * See to study about [ALL_IND_SUBPARTITIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-29DC1F28-E943-4A72-A0EE-E1E6C6A254CF.htm#REFRN20087)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllIndSubPartitions::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Owner of the index
    char szIndexName[OBJECT_NAME_LEN];     ///< Name of the index
    char szPartitionName[OBJECT_NAME_LEN]; ///< Name of the partition
    /**
     * subpartition_name :
     * - Anonymous subpartition naming policy in "STORE IN" clause:
     *   Given SYS_SUBP<n> (n is integer)
     * - Given subpartition template = "<pn>"_"<gn>" (@b pn is a partition name,
     *   @b gn a name of the subpartition given in the template definition)
     */
    char szSubPartitionName[OBJECT_NAME_LEN];///< Name of the subpartition
    ub4 iSubPartitionPosition;             ///< Position of the subpartition within the partition
    char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the subpartition
    char szGenerated[GENERATED_LEN];       ///< assosiated with all_objects.generated
    char szCompression[COMPRES_LEN];       ///< Indicates whether this subpartition is compressed (ENABLED) or not (DISABLED)
    char *szHighValue;                     ///< Subpartition bound value expression
    ps::lib::sql::ind_t nHighValueInd;     ///< ind_t::VAL_IS_NULL: value is NULL
    ub4 iHighValue;                        ///< length of the comment body.
    /**
     * - Enter tAttributes::szHighValue and lead cFunctionalReplace::sConvertStringFormat to derive.
     */
    std::string sHighValue;                ///< It is not a variable bound to SQL.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iSubPartitionPosition(0)
        , szHighValue(nullptr)
        , nHighValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHighValue(0)
        , sHighValue("")
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szIndexName, std::get<1>(oKey));
        ::strcpy(szPartitionName, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szHighValue which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iSubPartitionPosition(rhs.iSubPartitionPosition)
        , szHighValue(new char[rhs.iHighValue + 1])
        , nHighValueInd(rhs.nHighValueInd)
        , iHighValue(rhs.iHighValue)
        , sHighValue(rhs.sHighValue)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szIndexName, rhs.szIndexName);
        ::strcpy(szPartitionName, rhs.szPartitionName);
        ::strcpy(szSubPartitionName, rhs.szSubPartitionName);
        ::strcpy(szTablespaceName, rhs.szTablespaceName);
        ::strcpy(szGenerated, rhs.szGenerated);
        ::strcpy(szCompression, rhs.szCompression);
        ::strcpy(szHighValue, rhs.szHighValue);
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
            , szPartitionName, rhs.szPartitionName
            , iSubPartitionPosition, rhs.iSubPartitionPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iFullPartitionName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szIndexName, rhs.szIndexName
            , szPartitionName, rhs.szPartitionName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCompIndPartByQty(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szIndexName, rhs.szIndexName
        )
        // Is the prefix of szSubPartitionName matched with "SYS_SUBP" ?
        && ps::lib::iCompTernary(
            strncmp, szPartitionName, "SYS_SUBP", 8
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllIndSubPartitions& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepeatIndPartByQty(std::ostream& os, const size_t& i, cAllIndSubPartitions& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllIndSubPartitions::tAttributes::vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllIndSubPartitions& oCtx) const
{
    BOOST_ASSERT(oCtx.szSubpartitioningType_);
    BOOST_ASSERT(oCtx.szDefTablespaceName_);
    using ps::lib::iCompBinary;
    const bool iIsNamed = iCompBinary(strcmp, szGenerated, "N");
    os << boost::format("  %s%ssubpartition %s%s%s ")
        % (i == 0 ? "( " : ", ")
        % sColumnIdCooment(iSubPartitionPosition)
        % (iIsNamed ? "\"" : "/* ")
        % szSubPartitionName
        % (iIsNamed ? "\"" : " */")
    ;
    if (nHighValueInd != ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        os << sGetSubpertitionElement(oCtx.szSubpartitioningType_, 35, 45, 4, 2, sHighValue);
    }
    else
    {
        // Sub partition is organized HASH partitioning,
        // if nHighValueInd is ps::lib::sql::ind_t::VAL_IS_NULL.
        // There is no additional grammar about the boundary value.
    }
    if (! iCompBinary(strcmp, szTablespaceName, oCtx.szDefTablespaceName_))
    {
        os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    os << std::endl;
}

void cAllIndSubPartitions::tAttributes::vOnRepeatIndPartByQty(std::ostream& os, const size_t& i, cAllIndSubPartitions& oCtx) const
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

std::string cAllIndSubPartitions::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szIndexName='%s', szPartitionName='%s', szSubPartitionName='%s'  "
        R"(, iSubPartitionPosition=%u, szTablespaceName='%s', szGenerated='%s')"
        R"(, szCompression='%s', iHighValue=%u, szHighValue='%s', sHighValue='%s')"
    ) % szOwner % szIndexName % szPartitionName % szSubPartitionName
        % iSubPartitionPosition % szTablespaceName % szGenerated
        % szCompression % iHighValue % szHighValue % sHighValue
    ;
    return oss.str();
}

/**
 * @struct cAllIndSubPartitions::cRetriever
 */
class cAllIndSubPartitions::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllIndSubPartitions::oList_.
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

const uint32_t cAllIndSubPartitions::cRetriever::iBulkSize_ = 100;

cAllIndSubPartitions::tAttributes cAllIndSubPartitions::cRetriever::rTable_[cAllIndSubPartitions::cRetriever::iBulkSize_];

cAllIndSubPartitions::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
)
    : cDescriber(oSvc, iBulkSize_
        , szAllIndSubPartitions
        , "Reading index sub partition", &oBind_
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
        , later_9iR2 ? szHvReady : szHvNotReady
        , sGetSqlInList(oOwners)
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSubPartitionPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompression, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllIndSubPartitions::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllIndSubPartitions::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
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

void cAllIndSubPartitions::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllIndSubPartitions::szAllIndSubPartitions[] = {
"select ip.index_owner "
", ip.index_name "
", ip.partition_name "
", ip.subpartition_name "
", ip.subpartition_position "
", ip.tablespace_name "
", ao.generated "
", %s "
", %s "
"from all_ind_subpartitions ip "
", all_objects ao "
"where ip.index_owner in %s "
"and (ip.index_owner, ip.index_name) in ( "
	/*
	 * Subpartition indexes that contain at least one subpartition
	 * whose name or tablespace has changed.
	 */
	"select /*+ NO_MERGE NO_UNNEST */ t1.owner, t1.index_name "
	"from all_part_indexes t1 "
	", all_ind_subpartitions t2 "
	"where t1.owner in %s "
	"and t1.subpartitioning_type != 'NONE' "
	"and t2.index_owner = t1.owner "
	"and t2.index_name = t1.index_name "
	"and not exists ( "
		"select 1 "
		"from all_tab_subpartitions "
		"where table_owner = t1.owner "
		"and table_name = t1.table_name "
		"and partition_name = t2.partition_name "
		"and subpartition_name = t2.subpartition_name "
		"and tablespace_name = t2.tablespace_name "
	") "
") "
"and ao.owner = ip.index_owner "
"and ao.object_name = ip.index_name "
"and ao.subobject_name = ip.subpartition_name "
"and ao.object_type = 'INDEX SUBPARTITION' "
};

const char cAllIndSubPartitions::szComprsReady[] = {"nvl(ip.compression,'DISABLED') as compression"};
const char cAllIndSubPartitions::szComprsNotReady[] = {"'DISABLED' as compression"};
const char cAllIndSubPartitions::szHvReady[]         = {"ip.high_value"};
const char cAllIndSubPartitions::szHvNotReady[]      = {"ip.global_stats as high_value"};
/*
 * In Oracle R8.1.6.3.0, error ORA-00255 will occure.
 * To avoid this failure, use VARCHAR2 column of any of the all_tab_subpartitions table instead of null.
 * In R8.1.7.4.0, It can use null.
 */

cAllIndSubPartitions::cAllIndSubPartitions()
    : szSubpartitioningType_(nullptr)
    , szDefTablespaceName_(nullptr)
{
    // nothing to do
}

cAllIndSubPartitions::~cAllIndSubPartitions()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllIndSubPartitions::oSubmit(
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

void cAllIndSubPartitions::vRptAllIndSubPartitions(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const char *const szSubpartitioningType
    , const char *const szDefTablespaceName
){
    BOOST_ASSERT(szSubpartitioningType);
    BOOST_ASSERT(::strlen(szSubpartitioningType) > 0);
    BOOST_ASSERT(szDefTablespaceName);
    szSubpartitioningType_ = szSubpartitioningType;
    szDefTablespaceName_ = szDefTablespaceName;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllIndSubPartitions::tAttributes::iFullPartitionName
        , &cAllIndSubPartitions::tAttributes::vOnRepFullPartitionName
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllIndSubPartitions::vPostRepFullPartitionName
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

void cAllIndSubPartitions::vRptIndSubPartByQuantity(
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
        , &cAllIndSubPartitions::tAttributes::iCompIndPartByQty
        , &cAllIndSubPartitions::tAttributes::vOnRepeatIndPartByQty
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllIndSubPartitions::vPostRepeatIndPartByQty
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

void cAllIndSubPartitions::vPostRepFullPartitionName(std::ostream& os, const tAttributes&) const
{
    os << std::string("  )") << std::endl;
}

void cAllIndSubPartitions::vPostRepeatIndPartByQty(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
