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
constexpr size_t HOST_NAME_LEN = 2000+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cUserDbLinks::tAttributes
 * See to study about [ALL_DB_LINKS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-53867320-13A9-4EC1-A7E4-0C88AE38083A.htm#REFRN20050)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cUserDbLinks::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szDbLink[DBLINK_NAME_LEN];        ///< Name of object
	char szUsername[OBJECT_NAME_LEN];      ///< Name of the user on the remote database.
	ps::lib::sql::ind_t nUsernameInd;      ///
	char szHost[HOST_NAME_LEN];            ///< Usually Net Service Name is stored.
	ps::lib::sql::ind_t nHostInd;          ///
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nUsernameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nHostInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szDbLink, 0, sizeof(szDbLink));
        ::memset(szUsername, 0, sizeof(szUsername));
        ::memset(szHost, 0, sizeof(szHost));
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
            , szDbLink, rhs.szDbLink
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
    void vPrinter(std::ostream& os, const size_t& i, cUserDbLinks& oCtx) const
    {
        os << sPrintLabel("Dlns", i + 1, szDbLink);
        if (nUsernameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            oss_ << std::string("connect to ");
            if (ps::lib::iCompBinary(strcmp, szUsername, "CURRENT_USER"))
            {
                oss_ << szUsername;
            }
            else
            {
                oss_ << boost::format(R"("%s" identified by "&PWD_%03d")")
                    % szUsername % (i + 1);
            }
            oss_ << std::endl;
        }
        if (nHostInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            auto sHost = boost::algorithm::replace_all_copy(std::string(szHost), "\r\n", "\n");
            oss_ << boost::format(R"(using '%s')") % sHost
                << std::endl;
        }
        os  << boost::format(R"(drop database link "%s";)") % szDbLink << std::endl
            << boost::format(R"(create database link "%s")") % szDbLink << std::endl
            << boost::format(R"(%s;)") % oss_.str() << std::endl
            ;
        os  << oPrintItems(oCtx.oUserAuditObject_,  std::make_tuple(szOwner, szDbLink), true)
            ;
        oss_.str("");
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cUserDbLinks::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szDbLink='%s')"
            R"(, szUsername='%s', szHost='%s')"
        ) % szOwner % szDbLink
             % szUsername % szHost
        ;
    return oss.str();
}

/**
 * @struct cUserDbLinks::cRetriever
 */
class cUserDbLinks::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserDbLinks::oList_.
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
    bool iAccesible() const
    {
        const auto& oPrivsGiven(cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_DB_LINKS", "SELECT"});
    }
};

const uint32_t cUserDbLinks::cRetriever::iBulkSize_ = 100;

cUserDbLinks::tAttributes cUserDbLinks::cRetriever::rTable_[cUserDbLinks::cRetriever::iBulkSize_];

cUserDbLinks::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , (iAccesible() ? szDbaDbLinks : szAllDbLinks)
        , "Reading database links", &oBind_
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
    oDefine_.vAddItem(rTable_->szDbLink, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szUsername, SQLT_STR, &rTable_->nUsernameInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szHost, SQLT_STR, &rTable_->nHostInd, nullptr, nullptr, iSkip_);
}

void cUserDbLinks::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserDbLinks::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserDbLinks::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cUserDbLinks::szDbaDbLinks[] = {
"select owner, db_link "
", username "
", host "
"from dba_db_links "
"where owner in %s "
};

const char cUserDbLinks::szAllDbLinks[] = {
"select owner, db_link "
", username "
", host "
"from all_db_links "
"where owner in %s "
};

cUserDbLinks::cUserDbLinks(
    cUserAuditObject& oUserAuditObject
)
    : iNumRows_(0)
    , oUserAuditObject_(oUserAuditObject)
{
    // nothing to do
}

cUserDbLinks::~cUserDbLinks()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserDbLinks::oSubmit(
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

void cUserDbLinks::vPrintAllItems(
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
        , &cUserDbLinks::tAttributes::iKeyMatched
        , &cUserDbLinks::tAttributes::vPrinter
        , &cUserDbLinks::vPrintPreRows
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

void cUserDbLinks::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os << std::string("set echo off;") << std::endl;
    auto i = 0;
    for (const auto& oItem : oList_)
    {
        if (
            oItem.nUsernameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
            && ! ps::lib::iCompBinary(strcmp, oItem.szUsername, "CURRENT_USER")
        ){
            os << boost::format(R"(accept PWD_%03d char default '%s' prompt -)")
                % (i + 1) % oItem.szUsername << std::endl
                << boost::format(R"(  'Imput a password for connecting with "%s" to the "%s".  [%s] : ' hide )")
                % oItem.szUsername % oItem.szDbLink % oItem.szUsername << std::endl
                ;
        }
        i++;
    }
    os << std::string("set echo on;") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

