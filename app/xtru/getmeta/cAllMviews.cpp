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

#define UPDATABLE_LEN               (1+1)
#define REWRITABLE_LEN              (1+1)
#define REFMODE_LEN                 (6+1)
#define REFMETHOD_LEN               (8+1)
#define REFWITH_LEN                (11+1)
#define BUILDMODE_LEN               (9+1)
#define USE_NO_INDEX_LEN            (1+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/** 
 * @struct cAllMviews::tAttributes
 * See to study about [ALL_MVIEWS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-8B9432B5-6B66-411A-936E-590D9D7671E9.htm#REFRN20139)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllMviews::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szMviewName[OBJECT_NAME_LEN];     ///< 
	char szContainerName[OBJECT_NAME_LEN]; ///< 
	char szUpdatable[UPDATABLE_LEN];       ///< 
	char szRewriteEnabled[REWRITABLE_LEN]; ///< 
	char szRefreshMode[REFMODE_LEN];       ///< 
	char szRefreshMethod[REFMETHOD_LEN];   ///< derived from all_mviews.refresh_method
	char szBuildMode[BUILDMODE_LEN];       ///< 
	char szRefreshWith[REFWITH_LEN];       ///< derived from all_snapshots.refresh_method
	char szUseNoIndex[USE_NO_INDEX_LEN];   ///< 
	char szConstraintName[OBJECT_NAME_LEN];///< 
    ps::lib::sql::ind_t nConstraintNameInd;///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iLevel;                            ///< ==1: Regular MVIEW, >=2: Nested MVIEW
	char *szQuery;                         ///< Comment body
	ps::lib::sql::ind_t nQueryInd;         ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iQuery;                            ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nConstraintNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iLevel(0)
        , szQuery(nullptr)
        , nQueryInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iQuery(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szMviewName, 0, sizeof(szMviewName));
        ::memset(szContainerName, 0, sizeof(szContainerName));
        ::memset(szUpdatable, 0, sizeof(szUpdatable));
        ::memset(szRewriteEnabled, 0, sizeof(szRewriteEnabled));
        ::memset(szRefreshMode, 0, sizeof(szRefreshMode));
        ::memset(szRefreshMethod, 0, sizeof(szRefreshMethod));
        ::memset(szBuildMode, 0, sizeof(szBuildMode));
        ::memset(szRefreshWith, 0, sizeof(szRefreshWith));
        ::memset(szUseNoIndex, 0, sizeof(szUseNoIndex));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::strcpy(szOwner, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szQuery which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : nConstraintNameInd(rhs.nConstraintNameInd)
        , iLevel(rhs.iLevel)
        , szQuery(new char[rhs.iQuery + 1])
        , nQueryInd(rhs.nQueryInd)
        , iQuery(rhs.iQuery)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szMviewName, 0, sizeof(szMviewName));
        ::memset(szContainerName, 0, sizeof(szContainerName));
        ::memset(szUpdatable, 0, sizeof(szUpdatable));
        ::memset(szRewriteEnabled, 0, sizeof(szRewriteEnabled));
        ::memset(szRefreshMode, 0, sizeof(szRefreshMode));
        ::memset(szRefreshMethod, 0, sizeof(szRefreshMethod));
        ::memset(szBuildMode, 0, sizeof(szBuildMode));
        ::memset(szRefreshWith, 0, sizeof(szRefreshWith));
        ::memset(szUseNoIndex, 0, sizeof(szUseNoIndex));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szQuery, 0, rhs.iQuery + 1);
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szMviewName, rhs.szMviewName);
        ::strcpy(szContainerName, rhs.szContainerName);
        ::strcpy(szUpdatable, rhs.szUpdatable);
        ::strcpy(szRewriteEnabled, rhs.szRewriteEnabled);
        ::strcpy(szRefreshMode, rhs.szRefreshMode);
        ::strcpy(szRefreshMethod, rhs.szRefreshMethod);
        ::strcpy(szBuildMode, rhs.szBuildMode);
        ::strcpy(szRefreshWith, rhs.szRefreshWith);
        ::strcpy(szUseNoIndex, rhs.szUseNoIndex);
        ::strcpy(szConstraintName, rhs.szConstraintName);
        ::strcpy(szQuery, rhs.szQuery);
    }
    ~tAttributes()
    {
        delete [] szQuery;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szMviewName, rhs.szMviewName
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
    void vPrinter(std::ostream& os, const size_t& i, cAllMviews& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllMviews::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllMviews& oCtx) const
{
    using ps::lib::iCascadeBinaryOr;
    using ps::lib::iCompBinary;
    const std::tuple<const char*, const char*>
        oKeyOfContainer(szOwner, szContainerName)
        , oKeyOfMview(szOwner, szMviewName)
        ;
    os << sPrintLabel("Snps", i + 1, szMviewName);
	char szClusterName[OBJECT_NAME_LEN] = {0};
    // Is  oKeyOfContainer a cluster table or not?
    const auto& iClusterName = oCtx.oAllTables_.iCheckClusterTable(oKeyOfContainer, szClusterName);
    // Is  oKeyOfContainer a IOT table or not?
    const auto& iIotType = oCtx.oAllTables_.iCheckIot(oKeyOfContainer);

    os << boost::format("drop materialized view %s;")
            % ps::lib::sMakeEnclosedName(szMviewName)
        << std::endl
        << boost::format("create materialized view %s")
            % ps::lib::sMakeEnclosedName(szMviewName)
        << std::endl
        ;
    if (iCompBinary(strcmp, szBuildMode, "IMMEDIATE"))
    {
        os << std::string("  ");
        if (iClusterName)
        {
            os << oPrintItems(oCtx.oUserCluColumns_, std::make_tuple(szOwner, szContainerName, szClusterName), true);
        }
        else if (iIotType)
        {
             os << oPrintItems(oCtx.oUserIndexes_, std::make_tuple(szOwner, szContainerName, ""), true, true);
        }
        else
        {
            /* Do nothing */
        }
        oCtx.oAllTables_.vPrintFullItems(os, oKeyOfContainer, true);
    }
    else
    {
        os << std::string("on prebuilt table ") << std::endl;
    }
    if (iCompBinary(strcmp, szUseNoIndex, "Y"))
    {
        os << std::string("using no index") << std::endl;
    }
    else
    {
        if (strlen(szConstraintName))
        {
            oCtx.oUserIndexes_.vRptUsingIndexClause(os, std::make_tuple(szOwner, szMviewName, szConstraintName), true);
        }
        if (nConstraintNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            oCtx.oConsOfAKey_.iSubtractCtxItemList(std::make_tuple(szOwner, "", szConstraintName));
            oCtx.oEffectiveKeyCons_.iSubtractCtxItemList(std::make_tuple(szOwner, "", 0, szConstraintName));
        }
    }
    if (iCompBinary(strcmp, szRefreshMethod, "NEVER"))
    {
        os << boost::format("%s refresh ") % szRefreshMethod;
    }
    else
    {
        os << boost::format("refresh %s on %s ") % szRefreshMethod % szRefreshMode ;
        if (iCascadeBinaryOr(strcmp, szRefreshWith, "ROWID", "PRIMARY KEY"))
        {
            os << boost::format("with %s ") % szRefreshWith;
        }
    }
    os << std::endl
        << boost::format("%s%s query rewrite ")
            % (iCompBinary(strcmp, szUpdatable, "Y") ? "for update " : "")
            % (iCompBinary(strcmp, szRewriteEnabled, "Y") ? "enable" : "disable")
        ;
    // Convert the end-of-line sequence used by Windows to xUNIX.
    std::string sWork =
        ps::lib::sConvertEolSequence(szQuery, "\r\n", "\n");
    // Remove any characters in specific if they are used at the end of the string.
    ps::lib::vRmSpecCharsAtEofStr(sWork, "\r\n ");
    os << std::string("as") << std::endl;
    if (boost::count(sWork, '\n') > 0)
    {
        os << sWork;
    }
    else
    {
        // Format the definition query to make it easy to read.
        os << cSqlFmt(sWork);
    }
    os << std::endl << ';' << std::endl
        << oPrintItems(oCtx.oConsOfAKey_, std::make_tuple(szOwner, szMviewName, ""), true, iIotType, cConsOfAKey::iConsInSnap)
        << oPrintItems(oCtx.oUserIndexes_, std::make_tuple(szOwner, szMviewName, ""), true, false)
        << oPrintItems(oCtx.oEffectiveKeyCons_, std::make_tuple(szOwner, szMviewName, cEffectiveKeyCons::iKeyTypeIsUnique, ""), true)
        << oPrintItems(oCtx.oAllTriggers_, oKeyOfMview, true)
        << oPrintItems(oCtx.oAllTabPrivs_, oKeyOfMview, true)
        << oPrintItems(oCtx.oUserObjAuditOpts_, oKeyOfMview, true)
        << oPrintItems(oCtx.oUserAuditObject_, oKeyOfMview, true)
        << oPrintItems(oCtx.oAllTabComments_, oKeyOfMview, true)
        << oPrintItems(oCtx.oAllColComments_, oKeyOfMview, true)
        << oPrintItems(oCtx.oAllMviewLogs_, oKeyOfMview, true)
        ;
}

std::string cAllMviews::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szMviewName='%s', szContainerName='%s', szUpdatable='%s')"
        R"(, szRewriteEnabled='%s', szRefreshMode='%s', szRefreshMethod='%s', szBuildMode='%s')"
        R"(, szRefreshWith='%s', szUseNoIndex='%s', szConstraintName='%s'(%s))"
        R"(, iQuery=%u, szQuery='%s')"
    ) % szOwner % szMviewName % szContainerName % szUpdatable
        % szRewriteEnabled % szRefreshMode % szRefreshMethod % szBuildMode
        % szRefreshWith % szUseNoIndex % szConstraintName % nConstraintNameInd
        % iQuery % szQuery
    ;
    return oss.str();
}

/**
 * @struct cAllMviews::cRetriever
 */
class cAllMviews::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllMviews::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    /// number of elements in oList_.
    size_t& iNumRows_;
    ps::lib::sql::occi::cPieceVct pv_;
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

const uint32_t cAllMviews::cRetriever::iBulkSize_ = 100;

cAllMviews::tAttributes cAllMviews::cRetriever::rTable_[cAllMviews::cRetriever::iBulkSize_];

cAllMviews::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllMviews
        , "Reading properties of the Mview", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
    , pv_(iSkip_, &rTable_->szQuery, &rTable_->nQueryInd, &rTable_->iQuery)
{
    const auto sOwners (sGetSqlInList(oOwners));
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sOwners, sOwners });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szMviewName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szContainerName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szUpdatable, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szRewriteEnabled, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szRefreshMode, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szRefreshMethod, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szBuildMode, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szRefreshWith, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szUseNoIndex, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, &rTable_->nConstraintNameInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iLevel, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllMviews::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllMviews::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    using ps::lib::iCascadeBinaryOr;
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    for (uint32_t i = 0; i < iNumIter; ++i)
    {
        if (iCascadeBinaryOr(strcmp, rTable_[i].szRefreshWith, "ROWID", "AGGREGATE", "UNKNOWN"))
        {
			::strcpy(rTable_[i].szConstraintName, "I_SNAP$_");
			// The first 18 characters of the MVIEW base table are used.
			::strncat(rTable_[i].szConstraintName, rTable_[i].szContainerName, 18);
        }
    }
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllMviews::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllMviews::szAllMviews[] = {
"select am.owner "
", am.mview_name "
", am.container_name "
", am.updatable "
", am.rewrite_enabled "
", am.refresh_mode "
", am.refresh_method "
", am.build_mode "
", sn.refresh_method "
", am.use_no_index "
", t3.constraint_name "
", nvl(t4.lv, 1) as lv "
", am.query "
"from all_mviews am "
", all_snapshots sn "
", all_constraints t3 "
", ("
	"select max(level) lv, mview_name "
	"from all_mview_detail_relations "
	"where owner in %s "
	"connect by ( "
	 "prior owner = detailobj_owner "
	 "and prior mview_name = detailobj_name "
	") group by mview_name "
") t4 "
"where am.owner in %s "
"and sn.owner = am.owner "
"and sn.name = am.mview_name "
"and t3.owner (+) = am.owner "
"and t3.table_name (+) = am.container_name "
"and t3.constraint_type (+) = 'P' "
"and t4.mview_name (+) = am.mview_name "
};

cAllMviews::cAllMviews(
    cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
    , cConsOfAKey& oConsOfAKey
    , cEffectiveKeyCons& oEffectiveKeyCons
    , cAllTables& oAllTables
    , cAllTriggers& oAllTriggers
    , cAllMviewLogs& oAllMviewLogs
    , cAllTabComments& oAllTabComments
    , cAllColComments& oAllColComments
    , cUserCluColumns& oUserCluColumns
    , cUserIndexes& oUserIndexes
)
    : iNumRows_(0)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
    , oConsOfAKey_(oConsOfAKey)
    , oEffectiveKeyCons_(oEffectiveKeyCons)
    , oAllTables_(oAllTables)
    , oAllTriggers_(oAllTriggers)
    , oAllMviewLogs_(oAllMviewLogs)
    , oAllTabComments_(oAllTabComments)
    , oAllColComments_(oAllColComments)
    , oUserCluColumns_(oUserCluColumns)
    , oUserIndexes_(oUserIndexes)
{
    // nothing to do
}

cAllMviews::~cAllMviews()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllMviews::oSubmit(
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

void cAllMviews::vPrintAllItems(
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
        , &cAllMviews::tAttributes::iKeyMatched
        , &cAllMviews::tAttributes::vPrinter
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
