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
 * @struct cAllSubPartitionTemplates::tAttributes
 * See to study about [ALL_SUBPARTITION_TEMPLATES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-DE05ECA2-7375-4774-AD63-7FBFC04425E0.htm#REFRN20263)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllSubPartitionTemplates::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Owner of the table
    char szTableName[OBJECT_NAME_LEN];     ///< Name of the table
    char szSubPartitionName[OBJECT_NAME_LEN];///< Name of the subpartition
    ub4 iSubPartitionPosition;             ///< Position of the subpartition within the partition
    char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the subpartition
    ps::lib::sql::ind_t nTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    char *szHighBound;                     ///< Literal list values of the subpartition
    ps::lib::sql::ind_t nHighBoundInd;     ///< ind_t::VAL_IS_NULL: value is NULL
    ub4 iHighBound;                        ///< length of the comment body.
    /**
     * - Enter tAttributes::szHighBound and lead cFunctionalReplace::sConvertStringFormat to derive.
     */
    std::string sHighBound;                ///< It is not a variable bound to SQL.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        iSubPartitionPosition = 0;
        nTablespaceNameInd = ps::lib::sql::ind_t::VAL_IS_NULL;
        szHighBound = nullptr;
        nHighBoundInd = ps::lib::sql::ind_t::VAL_IS_NULL;
        iHighBound = 0;
        sHighBound = "";
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szHighBound which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iSubPartitionPosition(rhs.iSubPartitionPosition)
        , nTablespaceNameInd(rhs.nTablespaceNameInd)
        , szHighBound(new char[rhs.iHighBound + 1])
        , nHighBoundInd(rhs.nHighBoundInd)
        , iHighBound(rhs.iHighBound)
        , sHighBound(rhs.sHighBound)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szSubPartitionName, 0, sizeof(szSubPartitionName));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szSubPartitionName, rhs.szSubPartitionName);
        ::strcpy(szTablespaceName, rhs.szTablespaceName);
        ::strcpy(szHighBound, rhs.szHighBound);
    }
    ~tAttributes()
    {
        delete [] szHighBound;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , iSubPartitionPosition, rhs.iSubPartitionPosition
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
    void vPrinter(std::ostream& os, const size_t& i, cAllSubPartitionTemplates& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllSubPartitionTemplates::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllSubPartitionTemplates& oCtx) const
{
    BOOST_ASSERT(oCtx.sSubpartitioningType_.size());
    os << boost::format("%s%ssubpartition %s ")
        % (i == 0 ? "( " : ", ")
        % sColumnIdCooment(iSubPartitionPosition)
        % ps::lib::sMakeEnclosedName(szSubPartitionName)
    ;
    if (nHighBoundInd != ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        os << sGetSubpertitionElement(oCtx.sSubpartitioningType_, 35, 45, 4, 2, sHighBound);
    }
    else
    {
        // Sub partition is organized HASH partitioning,
        // if nHighBoundInd is ps::lib::sql::ind_t::VAL_IS_NULL.
        // There is no additional grammar about the boundary value.
    }
    if (nTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("tablespace %s") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    os << std::endl;
}

std::string cAllSubPartitionTemplates::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szSubPartitionName='%s'"
        R"(, iSubPartitionPosition=%u, szTablespaceName='%s'(%d))"
        R"(, iHighBound=%u, szHighBound='%s', sHighBound='%s')"
    ) % szOwner % szTableName % szSubPartitionName
        % iSubPartitionPosition % szTablespaceName % nTablespaceNameInd
        % iHighBound % szHighBound % sHighBound
    ;
    return oss.str();
}

/**
 * @struct cAllSubPartitionTemplates::cRetriever
 */
class cAllSubPartitionTemplates::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSubPartitionTemplates::oList_.
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

const uint32_t cAllSubPartitionTemplates::cRetriever::iBulkSize_ = 100;

cAllSubPartitionTemplates::tAttributes cAllSubPartitionTemplates::cRetriever::rTable_[cAllSubPartitionTemplates::cRetriever::iBulkSize_];

cAllSubPartitionTemplates::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const cFunctionalReplace& oFunctionalReplace
)
    : cDescriber(oSvc, iBulkSize_
        , szAllSubPartitionTemplates
        , "Reading template of sub partitioning", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szHighBound, &rTable_->nHighBoundInd, &rTable_->iHighBound)
    , oFunctionalReplace_(oFunctionalReplace)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSubPartitionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSubPartitionPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllSubPartitionTemplates::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllSubPartitionTemplates::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    for (uint32_t i = 0; i < iNumIter; ++i)
    {
        if (rTable_[i].nHighBoundInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            // To abstract long-lasting expressions.
            rTable_[i].sHighBound = oFunctionalReplace_.sConvertStringFormat(
                rTable_[i].szHighBound, rTable_[i].iHighBound
            );
        }
    }
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllSubPartitionTemplates::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllSubPartitionTemplates::szAllSubPartitionTemplates[] = {
"select spt.user_name "
", spt.table_name "
", spt.subpartition_name "
", spt.subpartition_position "
", spt.tablespace_name "
", spt.high_bound "
"from all_subpartition_templates spt "
"where spt.user_name in %s "
};

/*
 * In Oracle R8.1.6.3.0, error ORA-00255 will occure.
 * To avoid this failure, use VARCHAR2 column of any of the all_tab_subpartitions table instead of null.
 * In R8.1.7.4.0, It can use null.
 */

cAllSubPartitionTemplates::cAllSubPartitionTemplates(
    cAllTabSubPartitions& oAllTabSubPartitions
)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , oAllTabSubPartitions_(oAllTabSubPartitions)
{
    // nothing to do
}

cAllSubPartitionTemplates::~cAllSubPartitionTemplates()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllSubPartitionTemplates::oSubmit(
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

void cAllSubPartitionTemplates::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const std::string& sSubpartitioningType
){
    BOOST_ASSERT(sSubpartitioningType.size());
    sSubpartitioningType_ = sSubpartitioningType;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllSubPartitionTemplates::tAttributes::iKeyMatched
        , &cAllSubPartitionTemplates::tAttributes::vPrinter
        , &cAllSubPartitionTemplates::vPrintPreRows
        , &cAllSubPartitionTemplates::vPrintPostRows
        , static_cast<tOneTimeAction>(nullptr)
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
    // Remove subpartitions derived from templates
    // (automatically assigned names from the system) from the list.
    const auto iNumEraseRows = oAllTabSubPartitions_.iSubtractCtxItemList(
        std::make_tuple(
            std::get<0>(oKey)          // szOwner
            , std::get<1>(oKey)        // szTableName
            // Since there is a naming convention,
            // these sub-partitions can be specified without giving a name
            , "<This key is not used>"
            , "<This key is not used>"
        )
    );
    trc_ << boost::format("%d rows erased from cAllTabSubPartitions::oList_.")
        % iNumEraseRows
        << std::endl;
}

void cAllSubPartitionTemplates::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os << std::string("subpartition template") << std::endl;
}

void cAllSubPartitionTemplates::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
