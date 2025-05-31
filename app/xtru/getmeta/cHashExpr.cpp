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
 * @struct cHashExpr::tAttributes
 * See to study about [ALL_CLUSTER_HASH_EXPRESSIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-CED13490-D1C9-42FD-99D2-B695BAFED0F6.htm#REFRN20038)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cHashExpr::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the cluster
	char szClusterName[OBJECT_NAME_LEN];   ///< Name of the cluster
	char *szHashExpression;                ///< Text of the hash function of the hash cluster
	ps::lib::sql::ind_t nHashExpressionInd;///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iHashExpression;                   ///< length of the Text of the hash function.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szHashExpression(nullptr)
        , nHashExpressionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iHashExpression(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szClusterName, 0, sizeof(szClusterName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szClusterName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szHashExpression which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szHashExpression(new char[rhs.iHashExpression + 1])
        , nHashExpressionInd(rhs.nHashExpressionInd)
        , iHashExpression(rhs.iHashExpression)
    {
        std::memset(szOwner, 0, sizeof(szOwner));
        std::memset(szClusterName, 0, sizeof(szClusterName));
        std::strncpy(szOwner, rhs.szOwner, sizeof(szOwner) - 1);
        szOwner[sizeof(szOwner) - 1] = '\0';
        std::strncpy(szClusterName, rhs.szClusterName, sizeof(szClusterName) - 1);
        szClusterName[sizeof(szClusterName) - 1] = '\0';
        std::strncpy(szHashExpression, rhs.szHashExpression, iHashExpression);
        szHashExpression[iHashExpression] = '\0';
    }
    ~tAttributes()
    {
        delete [] szHashExpression;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szClusterName, rhs.szClusterName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szClusterName, rhs.szClusterName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cHashExpr& oCtx) const
    {
        const std::string sWork(
            &szHashExpression[0]
            , &szHashExpression[iHashExpression]
        );
        os << boost::format("hash is %s ") % sWork;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cHashExpr::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szClusterName='%s')"
        R"(, iHashExpression=%u, szHashExpression='%s')"
    ) % szOwner % szClusterName
        % iHashExpression % szHashExpression
    ;
    return oss.str();
}

/**
 * @struct cHashExpr::cRetriever
 */
class cHashExpr::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cHashExpr::oList_.
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

const uint32_t cHashExpr::cRetriever::iBulkSize_ = 100;

cHashExpr::tAttributes cHashExpr::cRetriever::rTable_[cHashExpr::cRetriever::iBulkSize_];

cHashExpr::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szHashExpr
        , "Reading hash expression", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szHashExpression, &rTable_->nHashExpressionInd, &rTable_->iHashExpression)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szClusterName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
}

void cHashExpr::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cHashExpr::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cHashExpr::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cHashExpr::szHashExpr[] = {
"select owner "
", cluster_name "
", hash_expression "
"from all_cluster_hash_expressions "
"where owner in %s "
};

cHashExpr::cHashExpr()
{
    // nothing to do
}

cHashExpr::~cHashExpr()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cHashExpr::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cHashExpr::vPrintAllItems(
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
        , &cHashExpr::tAttributes::iKeyMatched
        , &cHashExpr::tAttributes::vPrinter
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
