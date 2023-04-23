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

#define COMPOSITE_TYP_LEN         3+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllTabPartitions::tAttributes
 * See to study about [ALL_TAB_PARTITIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-D6A09AE6-BE9F-43F2-874C-60D218F09AC1.htm#REFRN20281)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTabPartitions::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Owner of the table
    char szTableName[OBJECT_NAME_LEN];     ///< Name of the table
    char szComposite[COMPOSITE_TYP_LEN];   ///< Indicates whether the table is composite-partitioned (YES) or not (NO)
    char szPartitionName[OBJECT_NAME_LEN]; ///< Name of the partition
    /**
     * If this is a composite partitioned table. Otherwise 0 is shown.
     */
    ub4 iSubpartitionCount;                ///< the number of subpartitions in the partition
    ub4 iPartitionPosition;                ///< Position of the partition within the tablea. Originate by 1.
    char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the subpartition
    ps::lib::sql::ind_t nTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    char szGenerated[GENERATED_LEN];       ///< assosiated with all_objects.generated
    /**
     * for a partition of a simple partitioned table, or the default (if specified) for subpartitions
     * for composite partitioned tables, otherwise NONE.
     * - NONE: The partition is composite, and a default setting is not specified for compression.
     * - ENABLED: The setting for compression is enabled.
     * - DISABLED: The setting for compression is disabled.
     */
    char szCompression[COMPRES_LEN];       ///< Indicates the actual compression property
    /**
     * - "BASIC" is intended for bulk load operations. The database does not compress data modified using conventional DML.
     * - "ADVANCED" is intended for OLTP applications and compresses data manipulated by any SQL operation.
     * - "QUERY LOW", "QUERY HIGH", "ARCHIVE LOW", and "ARCHIVE HIGH" values are associated with
     *   Hybrid Columnar Compression, a feature of the Enterprise Edition of Oracle Database
     *   that is dependent on the underlying storage system.
     * @note See Oracle Database [Concepts](https://docs.oracle.com/database/121/CNCPT/tablecls.htm#CNCPT89198)
     *   for more information.
     */
    char szCompressFor[COMPFOR_LEN];       ///< Default compression for what kind of operations:
    ps::lib::sql::ind_t nCompressForInd;   ///< ind_t::VAL_IS_NULL: value is NULL
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
        , nCompressForInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , szHighValue(nullptr)
        , nHighValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHighValue(0)
        , sHighValue("")
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szComposite, 0, sizeof(szComposite));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::memset(szCompressFor, 0, sizeof(szCompressFor));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
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
        , nCompressForInd(rhs.nCompressForInd)
        , szHighValue(new char[rhs.iHighValue + 1])
        , nHighValueInd(rhs.nHighValueInd)
        , iHighValue(rhs.iHighValue)
        , sHighValue(rhs.sHighValue)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szComposite, 0, sizeof(szComposite));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::memset(szCompressFor, 0, sizeof(szCompressFor));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szComposite, rhs.szComposite);
        ::strcpy(szPartitionName, rhs.szPartitionName);
        ::strcpy(szTablespaceName, rhs.szTablespaceName);
        ::strcpy(szGenerated, rhs.szGenerated);
        ::strcpy(szCompression, rhs.szCompression);
        ::strcpy(szCompressFor, rhs.szCompressFor);
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
            , szTableName, rhs.szTableName
            , iPartitionPosition, rhs.iPartitionPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iFullPartitionName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCompTabPartByQty(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        )
        // Is the prefix of szPartitionName matched with "SYS_P" ?
        && ps::lib::iCompTernary(
            strncmp, szPartitionName, "SYS_P", 5
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllTabPartitions& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepeatTabPartByQty(std::ostream& os, const size_t& i, cAllTabPartitions& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllTabPartitions::tAttributes::vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllTabPartitions& oCtx) const
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
    if (nHighValueInd != ps::lib::sql::ind_t::VAL_IS_NULL)
    {
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
        os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    // Compress option.
    // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129627
    cCompressObject oCompressObject(szCompression, szCompressFor, nCompressForInd);
    if (oCtx.oCompressObject_ && oCompressObject != *oCtx.oCompressObject_)
    {
        os << oCompressObject.sGetClause();
    }
    os << std::endl;
    if (iCompBinary(strcmp, szComposite, "YES"))
    {
        oCtx.oAllTabSubPartitions_.vRptAllTabSubPartitions(
            os
            , std::make_tuple(szOwner, szTableName, szPartitionName, ""), true
            , oCtx.szSubpartitioningType_, szTablespaceName, &oCompressObject
        );
    }
}

void cAllTabPartitions::tAttributes::vOnRepeatTabPartByQty(std::ostream& os, const size_t& i, cAllTabPartitions& oCtx) const
{
    auto res = oCtx.oTbsNames_.insert(szTablespaceName);
    if (res.second)
    {
        // oCtx.oTbsNames_ did not have szTablespaceName (so insertion got success).
        const auto sEnclosed = ps::lib::sMakeEnclosedName(szTablespaceName);
        if (i == 0)
        {
            os << boost::format("%s store in (%s")
                % oCtx.szQuantityPrefix_
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

std::string cAllTabPartitions::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szComposite='%s', szPartitionName='%s'  "
        R"(, iSubpartitionCount=%u,, iPartitionPosition=%u, szTablespaceName='%s'(%s))"
        R"(, szGenerated='%s', szCompression='%s', szCompressFor='%s'(%s))"
        R"(, iHighValue=%u, szHighValue='%s', sHighValue='%s')"
    ) % szOwner % szTableName % szComposite % szPartitionName
        % iSubpartitionCount % iPartitionPosition % szTablespaceName % nTablespaceNameInd
        % szGenerated % szCompression % szCompressFor % nCompressForInd
        % iHighValue % szHighValue % sHighValue
    ;
    return oss.str();
}

/**
 * @struct cAllTabPartitions::cRetriever
 */
class cAllTabPartitions::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTabPartitions::oList_.
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

const uint32_t cAllTabPartitions::cRetriever::iBulkSize_ = 100;

cAllTabPartitions::tAttributes cAllTabPartitions::cRetriever::rTable_[cAllTabPartitions::cRetriever::iBulkSize_];

cAllTabPartitions::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTabPartitions
        , "Reading table partition", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szHighValue, &rTable_->nHighValueInd, &rTable_->iHighValue)
    , oFunctionalReplace_(oFunctionalReplace)
{
    const auto later_9iR2 = oSvc.iCompDbVersion("0902",4) >= 0; // true: equal or later.
    const auto later_11gR1 = oSvc.iCompDbVersion("1101",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_9iR2 ? szComprsReady : szComprsNotReady
        , later_11gR1 ? szCompForReady : szCompForNotReady
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szComposite, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSubpartitionCount, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPartitionPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompression, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompressFor, SQLT_STR, &rTable_->nCompressForInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllTabPartitions::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTabPartitions::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
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

void cAllTabPartitions::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTabPartitions::szAllTabPartitions[] = {
"select tp.table_owner "
", tp.table_name "
", tp.composite "
", tp.partition_name "
", tp.subpartition_count "
", tp.partition_position "
", tp.tablespace_name "
", ao.generated "
", %s " // compression
", %s " // compress_for
", tp.high_value "
"from all_tab_partitions tp "
", all_objects ao "
"where tp.table_owner in %s "
"and ao.owner = tp.table_owner "
"and ao.object_name = tp.table_name "
"and ao.subobject_name = tp.partition_name "
"and ao.object_type = 'TABLE PARTITION' "
};

const char cAllTabPartitions::szComprsReady[] = {"tp.compression"};
const char cAllTabPartitions::szComprsNotReady[] = {"'DISABLED' as compression"};
const char cAllTabPartitions::szCompForReady[] = {"tp.compress_for"};
const char cAllTabPartitions::szCompForNotReady[] = {"null as compress_for"};

/*
 * In Oracle R8.1.6.3.0, error ORA-00255 will occure.
 * To avoid this failure, use VARCHAR2 column of any of the all_tab_subpartitions table instead of null.
 * In R8.1.7.4.0, It can use null.
 */

cAllTabPartitions::cAllTabPartitions(
    cAllTabSubPartitions& oAllTabSubPartitions
)
    : oAllTabSubPartitions_(oAllTabSubPartitions)
    , szPartitioningType_(nullptr)
    , szSubpartitioningType_(nullptr)
    , szDefTablespaceName_(nullptr)
    , oCompressObject_(nullptr)
    , szQuantityPrefix_(nullptr)
{
    // nothing to do
}

cAllTabPartitions::~cAllTabPartitions()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTabPartitions::oSubmit(
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

void cAllTabPartitions::vRptAllTabPartitions(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const char *const szPartitioningType
    , const char *const szSubpartitioningType
    , const char *const szDefTablespaceName
    , const cCompressObject *oCompressObject
){
    BOOST_ASSERT(szPartitioningType);
    BOOST_ASSERT(::strlen(szPartitioningType) > 0);
    BOOST_ASSERT(szSubpartitioningType);
    BOOST_ASSERT(::strlen(szSubpartitioningType) > 0);
    BOOST_ASSERT(szDefTablespaceName);
    szPartitioningType_ = szPartitioningType;
    szSubpartitioningType_ = szSubpartitioningType;
    szDefTablespaceName_ = szDefTablespaceName;
    oCompressObject_ = oCompressObject;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllTabPartitions::tAttributes::iFullPartitionName
        , &cAllTabPartitions::tAttributes::vOnRepFullPartitionName
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllTabPartitions::vPostRepFullPartitionName
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

void cAllTabPartitions::vRptTabPartByQuantity(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
	, const char *const szQuantityPrefix
){
    BOOST_ASSERT(szQuantityPrefix);
    szQuantityPrefix_ = szQuantityPrefix;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllTabPartitions::tAttributes::iCompTabPartByQty
        , &cAllTabPartitions::tAttributes::vOnRepeatTabPartByQty
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllTabPartitions::vPostRepeatTabPartByQty
        , &cAllTabPartitions::vNotFoundTabPartByQty
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

void cAllTabPartitions::vPostRepFullPartitionName(std::ostream& os, const tAttributes&) const
{
    os << std::string(") ");
}

void cAllTabPartitions::vPostRepeatTabPartByQty(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

void cAllTabPartitions::vNotFoundTabPartByQty(std::ostream& os, const tAttributes&) const
{
    if (ps::lib::iCompTernary(strncmp, szQuantityPrefix_, "interval", 8))
    {
        os << szQuantityPrefix_ << std::endl;
    }
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
