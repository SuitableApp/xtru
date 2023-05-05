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
 * @struct cAllRefreshChildren::tAttributes
 * See to study about [ALL_REFRESH_CHILDREN]
 * (https://docs.oracle.com/database/121/REFRN/GUID-EEAA39C5-A3C5-4FD0-A767-252DFF7E5B0B.htm#REFRN20179)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllRefreshChildren::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the object in the refresh group
	char szName[OBJECT_NAME_LEN];          ///< Name of the object in the refresh group
	char szROwner[OBJECT_NAME_LEN];        ///< Owner of the refresh group
	char szRName[OBJECT_NAME_LEN];         ///< Name of the refresh group
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szName, 0, sizeof(szName));
        ::memset(szROwner, 0, sizeof(szROwner));
        ::memset(szRName, 0, sizeof(szRName));
        ::strcpy(szROwner, std::get<0>(oKey));
        ::strcpy(szRName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("",""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szROwner, rhs.szROwner
            , szRName, rhs.szRName
            , szOwner, rhs.szOwner
            , szName, rhs.szName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szROwner, rhs.szROwner
            , szRName, rhs.szRName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllRefreshChildren& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllRefreshChildren::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllRefreshChildren& oCtx) const
{
    os << boost::format("  mviews(%u) := '%s.%s';") 
            % (i + 1)
            % ps::lib::sMakeEnclosedName(szOwner)
            % ps::lib::sMakeEnclosedName(szName)
        << std::endl
        ;
}

std::string cAllRefreshChildren::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szName='%s', szROwner='%s', szRName='%s')"
    ) % szOwner % szName % szROwner % szRName
    ;
    return oss.str();
}

/**
 * @struct cAllRefreshChildren::cRetriever
 */
class cAllRefreshChildren::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllRefreshChildren::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
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

const uint32_t cAllRefreshChildren::cRetriever::iBulkSize_ = 100;

cAllRefreshChildren::tAttributes cAllRefreshChildren::cRetriever::rTable_[cAllRefreshChildren::cRetriever::iBulkSize_];

cAllRefreshChildren::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllRefreshChildren
        , "Reading elements of the refresh group", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szROwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRName, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllRefreshChildren::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllRefreshChildren::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllRefreshChildren::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllRefreshChildren::szAllRefreshChildren[] = {
"select owner "
", name "
", rowner "
", rname "
"from all_refresh_children "
"where rowner in %s "
};

cAllRefreshChildren::cAllRefreshChildren()
{
    // nothing to do
}

cAllRefreshChildren::~cAllRefreshChildren()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllRefreshChildren::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllRefreshChildren::vPrintAllItems(
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
        , &cAllRefreshChildren::tAttributes::iKeyMatched
        , &cAllRefreshChildren::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllRefreshChildren::vPrintPostRows
        , &cAllRefreshChildren::vPrintPostRows
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

void cAllRefreshChildren::vPrintPostRows(std::ostream& os, const tAttributes& rKey) const
{
    os << std::string("  mviews(mviews.last + 1) := null;")
        << std::endl
        ;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
