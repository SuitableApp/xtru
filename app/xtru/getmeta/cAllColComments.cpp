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
 * @struct cAllColComments::tAttributes
 * See to study about [ALL_COL_COMMENTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-D93FC2B3-A23A-48FD-AD14-4DA3D49FB5F8.htm#REFRN20040)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllColComments::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szTableName[OBJECT_NAME_LEN];     ///< Name of object.
	char szColumnName[OBJECT_NAME_LEN];    ///< Name of table column.
	ub4 iColumnId;                         ///< Order number of column.
	char *szComments;                      ///< Comment body
	ps::lib::sql::ind_t nCommentsInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iComments;                         ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnId(0)
        , szComments(nullptr)
        , nCommentsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iComments(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
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
        : iColumnId(rhs.iColumnId)
        , szComments(new char[rhs.iComments + 1])
        , nCommentsInd(rhs.nCommentsInd)
        , iComments(rhs.iComments)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        std::strncpy(szOwner, rhs.szOwner, sizeof(szOwner) - 1);
        szOwner[sizeof(szOwner) - 1] = '\0';
        std::strncpy(szTableName, rhs.szTableName, sizeof(szTableName) - 1);
        szTableName[sizeof(szTableName) - 1] = '\0';
        std::strncpy(szColumnName, rhs.szColumnName, sizeof(szColumnName) - 1);
        szColumnName[sizeof(szColumnName) - 1] = '\0';
        std::strncpy(szComments, rhs.szComments, rhs.iComments);
        szComments[rhs.iComments] = '\0';
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
            , iColumnId, rhs.iColumnId
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
    void vPrinter(std::ostream& os, const size_t& i, cAllColComments& oCtx) const
    {
        // Convert the end-of-line sequence used by Windows to xUNIX.
        std::string sWork =
            ps::lib::sConvertEolSequence(std::string(&szComments[0], &szComments[iComments]), "\r\n", "\n");
        // Remove any characters in specific if they are used at the end of the string.
        ps::lib::vRmSpecCharsAtEofStr(sWork, "\r\n/");
        os << boost::format("comment on column %s%s.%s is '%s';")
            % sColumnIdCooment(iColumnId)
            % ps::lib::sMakeEnclosedName(szTableName)
            % ps::lib::sMakeEnclosedName(szColumnName)
            % sWork
            << std::endl;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllColComments::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szColumnName='%s')"
        R"(, iColumnId=%u, iComments=%u, szComments='%s')"
    ) % szOwner % szTableName % szColumnName
        % iColumnId % iComments % szComments
    ;
    return oss.str();
}

/**
 * @struct cAllColComments::cRetriever
 */
class cAllColComments::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSubPartKeyColumns::oList_.
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

const uint32_t cAllColComments::cRetriever::iBulkSize_ = 1000;

cAllColComments::tAttributes cAllColComments::cRetriever::rTable_[cAllColComments::cRetriever::iBulkSize_];

cAllColComments::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const int32_t& iCmntLvl
)
    : cDescriber(oSvc, iBulkSize_
        , szAllColComments
        , "Reading column comments", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szComments, &rTable_->nCommentsInd, &rTable_->iComments)
{
    std::string predicate;
	if (iCmntLvl == 0)      { predicate = "and 0 = 1"; }
    else if (iCmntLvl == 1) { predicate = "and t1.comments is not null"; }
    else                    { predicate = ""; }
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners), predicate });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
}

void cAllColComments::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllColComments::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllColComments::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllColComments::szAllColComments[] = {
"select t1.owner "
", t1.table_name "
", t1.column_name "
", t2.column_id "
// To escape single quotation marks included in comments.
", replace(t1.comments,'''','''''') as comments "
"from all_tab_columns t2 "
", all_col_comments t1 "
"where t1.owner in %s "
"and t2.owner = t1.owner "
"and t2.table_name = t1.table_name "
"and t2.column_name = t1.column_name "
// To embed additional predicates as necessary.
"%s"
};

cAllColComments::cAllColComments()
{
    // nothing to do
}

cAllColComments::~cAllColComments()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllColComments::oSubmit(
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

void cAllColComments::vPrintAllItems(
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
        , &cAllColComments::tAttributes::iKeyMatched
        , &cAllColComments::tAttributes::vPrinter
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
