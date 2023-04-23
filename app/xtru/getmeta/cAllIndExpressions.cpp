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

#define DESCNEND_TYP_LEN        (4+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/** 
 * @struct cAllIndExpressions::tAttributes
 * See to study about [ALL_IND_EXPRESSIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-E34E8770-4BCE-47A8-B91A-6BF82889E0C0.htm#REFRN20085)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllIndExpressions::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the index
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table or cluster
	char szIndexName[OBJECT_NAME_LEN];     ///< Name of the index
	char szDescend[DESCNEND_TYP_LEN];      ///< Direction of the key entry
	ub4 iColumnPosition;                   ///< Position of the column or attribute within the index
	char *szColumnExpression;              ///< Function-based index expression defining the column
	ps::lib::sql::ind_t nColumnExpressionInd;///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iColumnExpression;                 ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnPosition(std::get<3>(oKey))
        , szColumnExpression(nullptr)
        , nColumnExpressionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iColumnExpression(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szDescend, 0, sizeof(szDescend));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szIndexName, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", "", 0))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szColumnExpression which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iColumnPosition(rhs.iColumnPosition)
        , szColumnExpression(new char[rhs.iColumnExpression + 1])
        , nColumnExpressionInd(rhs.nColumnExpressionInd)
        , iColumnExpression(rhs.iColumnExpression)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szDescend, 0, sizeof(szDescend));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szIndexName, rhs.szIndexName);
        ::strcpy(szDescend, rhs.szDescend);
        ::strcpy(szColumnExpression, rhs.szColumnExpression);
    }
    ~tAttributes()
    {
        delete [] szColumnExpression;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
            , iColumnPosition, rhs.iColumnPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
            , iColumnPosition, rhs.iColumnPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllIndExpressions& oCtx) const
    {
        const std::string sWork(&szColumnExpression[0], &szColumnExpression[iColumnExpression]);
        os << boost::format("%c %s %s")
            % ((iColumnPosition - 1) == 0 ? '(' : ',')
            % sWork
            % szDescend
            << std::endl;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllIndExpressions::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szIndexName='%s', szDescend='%s')"
        R"(, iColumnPosition=%u, szColumnExpression='%s'(%d), iColumnExpression=%u)"
    ) % szOwner % szTableName % szIndexName % szDescend
        % iColumnPosition % szColumnExpression % nColumnExpressionInd % iColumnExpression
    ;
    return oss.str();
}

/**
 * @struct cAllIndExpressions::cRetriever
 */
class cAllIndExpressions::cRetriever
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
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllIndExpressions::cRetriever::iBulkSize_ = 100;

cAllIndExpressions::tAttributes cAllIndExpressions::cRetriever::rTable_[cAllIndExpressions::cRetriever::iBulkSize_];

cAllIndExpressions::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllIndExpressions
        , "Reading expressions of function base index", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szColumnExpression, &rTable_->nColumnExpressionInd, &rTable_->iColumnExpression)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDescend, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllIndExpressions::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllIndExpressions::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllIndExpressions::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllIndExpressions::szAllIndExpressions[] = {
"select ie.index_owner "
", ie.table_name "
", ie.index_name "
", ic.descend "
", ie.column_position "
", ie.column_expression "
"from all_ind_expressions ie "
", all_ind_columns ic "
"where ie.index_owner in %s "
"and ic.index_owner = ie.index_owner "
"and ic.index_name = ie.index_name "
"and ic.column_position = ie.column_position "
};

cAllIndExpressions::cAllIndExpressions()
{
    // nothing to do
}

cAllIndExpressions::~cAllIndExpressions()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllIndExpressions::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllIndExpressions::vPrintAllItems(
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
        , &cAllIndExpressions::tAttributes::iKeyMatched
        , &cAllIndExpressions::tAttributes::vPrinter
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
