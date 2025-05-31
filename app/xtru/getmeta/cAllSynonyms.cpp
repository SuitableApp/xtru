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

constexpr size_t DBLINK_NAME_LEN = 128+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllSynonyms::tAttributes
 * See to study about [ALL_SYNONYMS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-DCDB52FF-8339-4EDE-B36A-2E12AFE25D33.htm#REFRN20273)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllSynonyms::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the synonym
	char szSynonymName[OBJECT_NAME_LEN];   ///< Name of the synonym
	char szTableOwner[OBJECT_NAME_LEN];    ///< Owner of the object referenced by the synonym
    ps::lib::sql::ind_t nTableOwnerInd;    ///<
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the object referenced by the synonym
	char szDbLink[DBLINK_NAME_LEN];        ///< Name of the database link referenced, if any
    ps::lib::sql::ind_t nDbLinkInd;        ///<
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nTableOwnerInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nDbLinkInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szSynonymName, 0, sizeof(szSynonymName));
        ::memset(szTableOwner, 0, sizeof(szTableOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szDbLink, 0, sizeof(szDbLink));
        ::strcpy(szOwner, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szSynonymName, rhs.szSynonymName
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
    void vPrinter(std::ostream& os, const size_t& i, cAllSynonyms& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllSynonyms::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllSynonyms& oCtx) const
{
    oss_.str("");
    if (nTableOwnerInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        oss_
            << ps::lib::sMakeEnclosedName(szTableOwner)
            << std::string(".")
            ;
    }
    oss_ << ps::lib::sMakeEnclosedName(szTableName);
    if (nDbLinkInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        oss_
            << std::string("@")
            << ps::lib::sMakeEnclosedName(szDbLink)
            ;
    }
    os
        << sPrintLabel("Syns", i + 1, szSynonymName)
        << boost::format("drop synonym %s;")
           % ps::lib::sMakeEnclosedName(szSynonymName)
        << std::endl
        << boost::format("create synonym %s for %s;")
           % ps::lib::sMakeEnclosedName(szSynonymName)
           % oss_.str()
        << std::endl
        // Showing the trails of the audit.
        << oPrintItems(oCtx.oUserAuditObject_, std::make_tuple(szOwner, szSynonymName), true);
        ;
}

std::string cAllSynonyms::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szSynonymName='%s', szTableOwner='%s'(%d))"
        R"(, szTableName=%s, szDbLink=%s(%d))"
    ) % szOwner % szSynonymName % szTableOwner % nTableOwnerInd
        % szTableName % szDbLink % nDbLinkInd
    ;
    return oss.str();
}

/**
 * @struct cAllSynonyms::cRetriever
 */
class cAllSynonyms::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSynonyms::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
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

const uint32_t cAllSynonyms::cRetriever::iBulkSize_ = 100;

cAllSynonyms::tAttributes cAllSynonyms::cRetriever::rTable_[cAllSynonyms::cRetriever::iBulkSize_];

cAllSynonyms::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllSynonyms
        , "Reading information of the synonym", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szSynonymName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableOwner, SQLT_STR, &rTable_->nTableOwnerInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDbLink, SQLT_STR, &rTable_->nDbLinkInd, nullptr, nullptr, iSkip_);
}

void cAllSynonyms::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllSynonyms::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllSynonyms::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllSynonyms::szAllSynonyms[] = {
"select owner "
", synonym_name "
", table_owner "
", table_name "
", db_link "
"from all_synonyms "
"where owner in %s "
};

cAllSynonyms::cAllSynonyms(
    cUserAuditObject& oUserAuditObject
)
    : iNumRows_(0)
    , oUserAuditObject_(oUserAuditObject)
{
    // nothing to do
}

cAllSynonyms::~cAllSynonyms()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllSynonyms::oSubmit(
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

void cAllSynonyms::vPrintAllItems(
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
        , &cAllSynonyms::tAttributes::iKeyMatched
        , &cAllSynonyms::tAttributes::vPrinter
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
