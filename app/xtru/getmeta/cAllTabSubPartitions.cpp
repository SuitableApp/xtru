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

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllTabSubPartitions::tAttributes
 * See to study about [ALL_TAB_SUBPARTITIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-E1EC3001-6092-41A7-96EE-C2CF8A0EB5EA.htm#REFRN20285)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTabSubPartitions::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Owner of the table
    char szTableName[OBJECT_NAME_LEN];     ///< Name of the table
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
    ps::lib::sql::ind_t nCompressForInd;
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
        , nCompressForInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , szHighValue(nullptr)
        , nHighValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHighValue(0)
        , sHighValue("")
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::memset(szCompressFor, 0, sizeof(szCompressFor));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szPartitionName, std::get<2>(oKey));
        ::strcpy(szSubPartitionName, std::get<3>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", "", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szHighValue which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iSubPartitionPosition(rhs.iSubPartitionPosition)
        , nCompressForInd(rhs.nCompressForInd)
        , szHighValue(new char[rhs.iHighValue + 1])
        , nHighValueInd(rhs.nHighValueInd)
        , iHighValue(rhs.iHighValue)
        , sHighValue(rhs.sHighValue)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szPartitionName, 0, sizeof(szPartitionName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szCompression, 0, sizeof(szCompression));
        ::memset(szCompressFor, 0, sizeof(szCompressFor));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szPartitionName, rhs.szPartitionName);
        ::strcpy(szSubPartitionName, rhs.szSubPartitionName);
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
            , szPartitionName, rhs.szPartitionName
            , szSubPartitionName, rhs.szSubPartitionName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iFullPartitionName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szPartitionName, rhs.szPartitionName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iPrefixMatchedToSubPartition(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        )
        // Is the prefix of szPartitionName matched with szSubPartitionName ?
        && ps::lib::iCompTernary(
            strncmp, szPartitionName, szSubPartitionName, ::strlen(szPartitionName)
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCompTabPartByQty(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        )
        // Is the prefix of szSubPartitionName matched with "SYS_SUBP" ?
        && ps::lib::iCompTernary(
            strncmp, szSubPartitionName, "SYS_SUBP", 8
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllTabSubPartitions& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vOnRepeatTabPartByQty(std::ostream& os, const size_t& i, cAllTabSubPartitions& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllTabSubPartitions::tAttributes::vOnRepFullPartitionName(std::ostream& os, const size_t& i, cAllTabSubPartitions& oCtx) const
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
    // Compress option.
    // https://docs.oracle.com/database/121/SQLRF/statements_7002.htm#i2129627
    cCompressObject oCompressObject(szCompression, szCompressFor, nCompressForInd);
    if (oCtx.oCompressObject_ && oCompressObject != *oCtx.oCompressObject_)
    {
        os << oCompressObject.sGetClause();
    }
    os << std::endl;
}

void cAllTabSubPartitions::tAttributes::vOnRepeatTabPartByQty(std::ostream& os, const size_t& i, cAllTabSubPartitions& oCtx) const
{
    auto res = oCtx.oTbsNames_.insert(szTablespaceName);
    if (res.second)
    {
        // oCtx.oTbsNames_ did not have szTablespaceName (so insertion got success).
        const auto sEnclosed = ps::lib::sMakeEnclosedName(szTablespaceName);
        if (i == 0)
        {
            os << boost::format("subpartitions %u store in (%s")
                % oCtx.iSubPartitionCount_
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

std::string cAllTabSubPartitions::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szPartitionName='%s', szSubPartitionName='%s')"
        R"(, iSubPartitionPosition=%u, szTablespaceName='%s', szGenerated='%s')"
        R"(, szCompression='%s', szCompressFor='%s'(%s))"
        R"(, iHighValue=%u, szHighValue='%s', sHighValue='%s')"
    ) % szOwner % szTableName % szPartitionName % szSubPartitionName
        % iSubPartitionPosition % szTablespaceName % szGenerated
        % szCompression % szCompressFor % nCompressForInd
        % iHighValue % szHighValue % sHighValue
    ;
    return oss.str();
}

/**
 * @struct cAllTabSubPartitions::cRetriever
 */
class cAllTabSubPartitions::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTabSubPartitions::oList_.
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

const uint32_t cAllTabSubPartitions::cRetriever::iBulkSize_ = 100;

cAllTabSubPartitions::tAttributes cAllTabSubPartitions::cRetriever::rTable_[cAllTabSubPartitions::cRetriever::iBulkSize_];

cAllTabSubPartitions::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTabSubPartitions
        , "Reading table sub partition", &oBind_
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
        , later_9iR2 ? szHvReady : szHvNotReady
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSubPartitionPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompression, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCompressFor, SQLT_STR, &rTable_->nCompressForInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllTabSubPartitions::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTabSubPartitions::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
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

void cAllTabSubPartitions::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTabSubPartitions::szAllTabSubPartitions[] = {
"select tp.table_owner "
", tp.table_name "
", tp.partition_name "
", tp.subpartition_name "
", tp.subpartition_position "
", tp.tablespace_name "
", ao.generated "
", %s " // compression
", %s " // compress_for
", %s " // high_value
"from all_tab_subpartitions tp "
", all_objects ao "
"where tp.table_owner in %s "
"and ao.owner = tp.table_owner "
"and ao.object_name = tp.table_name "
"and ao.subobject_name = tp.subpartition_name "
"and ao.object_type = 'TABLE SUBPARTITION' "
};

const char cAllTabSubPartitions::szComprsReady[]     = {"tp.compression"};
const char cAllTabSubPartitions::szComprsNotReady[]  = {"'DISABLED' as compression"};
const char cAllTabSubPartitions::szCompForReady[]    = {"tp.compress_for"};
const char cAllTabSubPartitions::szCompForNotReady[] = {"null as compress_for"};
const char cAllTabSubPartitions::szHvReady[]         = {"tp.high_value"};
const char cAllTabSubPartitions::szHvNotReady[]      = {"tp.global_stats as high_value"};
/*
 * In Oracle R8.1.6.3.0, error ORA-00255 will occure.
 * To avoid this failure, use VARCHAR2 column of any of the all_tab_subpartitions table instead of null.
 * In R8.1.7.4.0, It can use null.
 */

cAllTabSubPartitions::cAllTabSubPartitions()
    : szSubpartitioningType_(nullptr)
    , szDefTablespaceName_(nullptr)
    , oCompressObject_(nullptr)
    , iSubPartitionCount_(0)
{
    // nothing to do
}

cAllTabSubPartitions::~cAllTabSubPartitions()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTabSubPartitions::oSubmit(
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

void cAllTabSubPartitions::vRptAllTabSubPartitions(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const char *const szSubpartitioningType
    , const char *const szDefTablespaceName
    , const cCompressObject *oCompressObject
){
    BOOST_ASSERT(szSubpartitioningType);
    BOOST_ASSERT(::strlen(szSubpartitioningType) > 0);
    BOOST_ASSERT(szDefTablespaceName);
    szSubpartitioningType_ = szSubpartitioningType;
    szDefTablespaceName_ = szDefTablespaceName;
    oCompressObject_ = oCompressObject;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllTabSubPartitions::tAttributes::iFullPartitionName
        , &cAllTabSubPartitions::tAttributes::vOnRepFullPartitionName
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllTabSubPartitions::vPostRepFullPartitionName
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

size_t cAllTabSubPartitions::iSubtractCtxItemList(
    const tKeyTuple& oKey
){
    return iEraseMatchedKeyItem(
        tAttributes(oKey)
        , oList_
        , &cAllTabSubPartitions::tAttributes::iPrefixMatchedToSubPartition
    );
}

void cAllTabSubPartitions::vRptTabSubPartByQuantity(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const ub4 iSubPartitionCount
){
    BOOST_ASSERT(iSubPartitionCount);
    iSubPartitionCount_ = iSubPartitionCount;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllTabSubPartitions::tAttributes::iCompTabPartByQty
        , &cAllTabSubPartitions::tAttributes::vOnRepeatTabPartByQty
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllTabSubPartitions::vPostRepeatTabPartByQty
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

void cAllTabSubPartitions::vPostRepFullPartitionName(std::ostream& os, const tAttributes&) const
{
    os << std::string("  )") << std::endl;
}

void cAllTabSubPartitions::vPostRepeatTabPartByQty(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
