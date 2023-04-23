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

#define IMPDST_LEN       1+1
#define NEXT_DATE_LEN   24+1
#define INTERVAL_LEN   200+1
#define BROKEN_LEN       1+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllRefresh::tAttributes
 * See to study about [ALL_REFRESH]
 * (https://docs.oracle.com/database/121/REFRN/GUID-CDD8FB99-4442-4731-AC57-E74645DDF210.htm#REFRN20178)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllRefresh::tAttributes
{
	char szROwner[OBJECT_NAME_LEN];     ///< Owner of the refresh group
	char szRName[OBJECT_NAME_LEN];      ///< Name of the refresh group
	ub4 iRefGroup;                      ///< Internal identifier of the refresh group
    /// Indicates whether the refresh group is destroyed when its last item is subtracted
	char szImplicitDestroy[IMPDST_LEN]; ///< (Y) or not (N)
	char szRollbackSeg[OBJECT_NAME_LEN];///< Name of the rollback segment to use while refreshing
	ps::lib::sql::ind_t nRollbackSegInd;///< ps::lib::sql::ind_t::VAL_IS_NOTNULL or ps::lib::sql::ind_t::VAL_IS_NULL
	char szNextDate[NEXT_DATE_LEN];     ///< Date that this job will next be refreshed automatically, if not broken
	char szInterval[INTERVAL_LEN];      ///< A date function used to compute the next NEXT_DATE
	ps::lib::sql::ind_t nIntervalInd;   ///< ps::lib::sql::ind_t::VAL_IS_NOTNULL or ps::lib::sql::ind_t::VAL_IS_NULL
	char szBroken[BROKEN_LEN];          ///< Indicates whether the job is broken and will never be run (Y) or not (N)
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iRefGroup(0)
        , nRollbackSegInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIntervalInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szROwner, 0, sizeof(szROwner));
        ::memset(szRName, 0, sizeof(szRName));
        ::memset(szImplicitDestroy, 0, sizeof(szImplicitDestroy));
        ::memset(szRollbackSeg, 0, sizeof(szRollbackSeg));
        ::memset(szNextDate, 0, sizeof(szNextDate));
        ::memset(szInterval, 0, sizeof(szInterval));
        ::memset(szBroken, 0, sizeof(szBroken));
        ::strcpy(szROwner, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szROwner, rhs.szROwner
            , szRName, rhs.szRName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szROwner, rhs.szROwner
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllRefresh& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllRefresh::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllRefresh& oCtx) const
{
    using ps::lib::iCompBinary;
    os << sPrintLabel("Refs", i + 1, std::string(szRName) + "_RG")
        << std::string("declare") << std::endl
        << std::string("  mviews dbms_utility.uncl_array;") << std::endl
        << boost::format("  refg_name varchar2(%d) default '%s.%s';")
            % ((OBJECT_NAME_LEN * 2) + 5)
            % ps::lib::sMakeEnclosedName(szROwner)
            % ps::lib::sMakeEnclosedName(szRName)
        << std::endl
        << std::string("begin") << std::endl
        << std::string("  for rec in (") << std::endl
        << std::string("    select rname from all_refresh") << std::endl
        << boost::format("    where rowner = '%s'") % szROwner
        << std::endl
        << boost::format("    and rname = '%s'") % szRName
        << std::endl
        << std::string("  ) loop") << std::endl
        << std::string("    dbms_refresh.destroy (refg_name);") << std::endl
        << std::string("  end loop;") << std::endl
        << oPrintItems(
            oCtx.oAllRefreshChildren_, std::make_tuple(szROwner, szRName), true
        )
        << std::string("  dbms_refresh.make") << std::endl
        << std::string("  (  name                 => refg_name") << std::endl
        << std::string("  ,  tab                  => mviews") << std::endl
        << boost::format("  ,  next_date            => to_date('%s','%s')")
            % szNextDate % oCtx.sDateMask_
        << std::endl
        << boost::format("  ,  interval             => %s")
            % ((nIntervalInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
                ? (std::string("'") + szInterval + "'")
                : "null"
            )
        << std::endl
        << boost::format("  ,  implicit_destroy     => %s")
            % (iCompBinary(strcmp, szImplicitDestroy, "Y") ? "true" : "false")
        << std::endl
        << boost::format("  ,  rollback_seg         => %s")
            % ((nRollbackSegInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
                ? (std::string("'") + ps::lib::sMakeEnclosedName(szRollbackSeg) + "'")
                : "null"
            )
        << std::endl
        << std::string("  );") << std::endl
        << std::string("end;") << std::endl
        << std::string("/") << std::endl
        ;
}

std::string cAllRefresh::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szROwner='%s', szRName='%s', iRefGroup=%u, szImplicitDestroy=%s)"
        R"(, szRollbackSeg=%s(%d), szNextDate=%s, szInterval=%s(%d), szBroken=%s)"
    ) % szROwner % szRName % iRefGroup % szImplicitDestroy
    % szRollbackSeg % nRollbackSegInd % szNextDate % szInterval % nIntervalInd % szBroken
    ;
    return oss.str();
}

/**
 * @struct cAllRefresh::cRetriever
 */
class cAllRefresh::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllRefresh::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    /// number of elements in oList_.
    size_t& iNumRows_;
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

const uint32_t cAllRefresh::cRetriever::iBulkSize_ = 100;

cAllRefresh::tAttributes cAllRefresh::cRetriever::rTable_[cAllRefresh::cRetriever::iBulkSize_];

cAllRefresh::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllRefresh
        , "Reading tables referring to other tables", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szROwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iRefGroup, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szImplicitDestroy, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRollbackSeg, SQLT_STR, &rTable_->nRollbackSegInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szNextDate, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInterval, SQLT_STR, &rTable_->nIntervalInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBroken, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllRefresh::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllRefresh::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllRefresh::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllRefresh::szAllRefresh[] = {
"select rowner "
", rname "
", refgroup "
", implicit_destroy "
", rollback_seg "
", next_date "
", replace(interval,'''','''''') as interval "
", broken "
"from all_refresh "
"where rowner in %s "
};

cAllRefresh::cAllRefresh(
    const std::string sDateMask
    , cAllRefreshChildren& oAllRefreshChildren
)
    : iNumRows_(0)
    , sDateMask_(sDateMask)
    , oAllRefreshChildren_(oAllRefreshChildren)
{
    // nothing to do
}

cAllRefresh::~cAllRefresh()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllRefresh::oSubmit(
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

void cAllRefresh::vPrintAllItems(
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
        , &cAllRefresh::tAttributes::iKeyMatched
        , &cAllRefresh::tAttributes::vPrinter
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
