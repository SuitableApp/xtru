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
 * @struct cAllTabComments::tAttributes
 * See to study about [ALL_TAB_COMMENTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-699B7976-C1DF-4502-A70C-7DDEBDBF3127.htm#REFRN20278)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTabComments::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szTableName[OBJECT_NAME_LEN];     ///< Name of object
	char *szComments;                      ///< Comment body
	ps::lib::sql::ind_t nCommentsInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iComments;                         ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szComments(nullptr)
        , nCommentsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iComments(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
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
     * Since tAttributes::szComments which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szComments(new char[rhs.iComments + 1])
        , nCommentsInd(rhs.nCommentsInd)
        , iComments(rhs.iComments)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szComments, rhs.szComments);
    }
    ~tAttributes()
    {
        delete [] szComments;
    }
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
    void vPrinter(std::ostream& os, const size_t& i, cAllTabComments& oCtx) const
    {
        // Convert the end-of-line sequence used by Windows to xUNIX.
        std::string sWork =
            ps::lib::sConvertEolSequence(szComments, "\r\n", "\n");
        // Remove any characters in specific if they are used at the end of the string.
        ps::lib::vRmSpecCharsAtEofStr(sWork, "\r\n/");
        os << boost::format("comment on table %s is '%s';")
            % ps::lib::sMakeEnclosedName(szTableName)
            % sWork
            << std::endl;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllTabComments::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s')"
        R"(, iComments=%u, szComments='%s')"
    ) % szOwner % szTableName
        % iComments % szComments
    ;
    return oss.str();
}

/**
 * @struct cAllTabComments::cRetriever
 */
class cAllTabComments::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTabComments::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    ps::lib::sql::occi::cPieceVct pv_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , const int32_t& iCmntLvl
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllTabComments::cRetriever::iBulkSize_ = 100;

cAllTabComments::tAttributes cAllTabComments::cRetriever::rTable_[cAllTabComments::cRetriever::iBulkSize_];

cAllTabComments::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const int32_t& iCmntLvl
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTabComments
        , "Reading table comments", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szComments, &rTable_->nCommentsInd, &rTable_->iComments)
{
    std::string predicate;
	if (iCmntLvl == 0)      { predicate = "and 0 = 1"; }
    else if (iCmntLvl == 1) { predicate = "and comments is not null"; }
    else                    { predicate = ""; }
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners), predicate });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllTabComments::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTabComments::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllTabComments::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTabComments::szAllTabComments[] = {
"select owner "
", table_name "
// To escape single quotation marks included in comments.
", replace(comments,'''','''''') as comments "
"from all_tab_comments "
"where owner in %s "
"and not table_name like 'MLOG$\%' "
// To embed additional predicates as necessary.
"%s"
};

cAllTabComments::cAllTabComments()
{
    // nothing to do
}

cAllTabComments::~cAllTabComments()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTabComments::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const int32_t& iCmntLvl
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , iCmntLvl
    );
}

void cAllTabComments::vPrintAllItems(
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
        , &cAllTabComments::tAttributes::iKeyMatched
        , &cAllTabComments::tAttributes::vPrinter
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
