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
 * @struct cAllViews::tAttributes
 * See to study about [ALL_VIEWS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-0FE29B36-0882-47AE-A54E-A01FC9C4100A.htm#REFRN20305)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllViews::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the view
	char szViewName[OBJECT_NAME_LEN];      ///< Name of the view
	char *szText;                          ///< View text
	ps::lib::sql::ind_t nTextInd;          ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iText;                             ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szText(nullptr)
        , nTextInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iText(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szViewName, 0, sizeof(szViewName));
        ::strcpy(szOwner, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szText which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szText(new char[rhs.iText + 1])
        , nTextInd(rhs.nTextInd)
        , iText(rhs.iText)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szViewName, 0, sizeof(szViewName));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szViewName, rhs.szViewName);
        ::strcpy(szText, rhs.szText);
    }
    ~tAttributes()
    {
        delete [] szText;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szViewName, rhs.szViewName
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
    void vPrinter(std::ostream& os, const size_t& i, cAllViews& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllViews::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllViews& oCtx) const
{
    const std::tuple<const char*, const char*> oKey(szOwner, szViewName);
    // Convert the end-of-line sequence used by Windows to xUNIX.
    std::string sWork =
        ps::lib::sConvertEolSequence(szText, "\r\n", "\n");
    // Remove any characters in specific if they are used at the end of the string.
    ps::lib::vRmSpecCharsAtEofStr(sWork, " \r\n/");
    os << sPrintLabel("View", i + 1, std::string(szViewName))
       << boost::format("create or replace force view %s")
            % ps::lib::sMakeEnclosedName(szViewName)
       << std::endl
       << oPrintItemsFN(oCtx.oAllTabColumns_, &cAllTabColumns::vPrintAllViewColumns, oKey, true)
       << std::string("as")
       << std::endl
       << sWork
       << std::endl
       << std::string(";")
       << std::endl
       << boost::format("show errors %s %s;")
            % "view" % ps::lib::sMakeEnclosedName(szViewName)
       << std::endl
       << oPrintItems(oCtx.oAllTabComments_, oKey, true)
       << oPrintItems(oCtx.oAllColComments_, oKey, true)
       << oPrintItems(oCtx.oAllTriggers_, oKey, true)
       << oPrintItems(oCtx.oAllTabPrivs_, oKey, true)
       << oPrintItems(oCtx.oUserObjAuditOpts_, oKey, true)
       << oPrintItems(oCtx.oUserAuditObject_, oKey, true)
       ;
}

std::string cAllViews::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szViewName='%s')"
        R"(, iText=%u, szText='%s')"
    ) % szOwner % szViewName
        % iText % szText
    ;
    return oss.str();
}

/**
 * @struct cAllViews::cRetriever
 */
class cAllViews::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllViews::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
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

const uint32_t cAllViews::cRetriever::iBulkSize_ = 100;

cAllViews::tAttributes cAllViews::cRetriever::rTable_[cAllViews::cRetriever::iBulkSize_];

cAllViews::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllViews
        , "Reading information of the views", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
    , pv_(iSkip_, &rTable_->szText, &rTable_->nTextInd, &rTable_->iText)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szViewName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
}

void cAllViews::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllViews::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllViews::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllViews::szAllViews[] = {
"select owner "
", view_name "
", text "
"from all_views v "
"where owner in %s "
"and not exists ( " /* exclude view of snapshots */
	"select null "
	"from all_snapshots "
	"where owner = v.owner "
	"and name = v.view_name "
") "
};

cAllViews::cAllViews(
    cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
    , cAllTriggers& oAllTriggers
    , cAllTabComments& oAllTabComments
    , cAllColComments& oAllColComments
    , cAllTabColumns& oAllTabColumns
)
    : iNumRows_(0)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
    , oAllTriggers_(oAllTriggers)
    , oAllTabComments_(oAllTabComments)
    , oAllColComments_(oAllColComments)
    , oAllTabColumns_(oAllTabColumns)
{
    // nothing to do
}

cAllViews::~cAllViews()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllViews::oSubmit(
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

void cAllViews::vPrintAllItems(
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
        , &cAllViews::tAttributes::iKeyMatched
        , &cAllViews::tAttributes::vPrinter
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
