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

#define OWNER_NAME_LEN               (3+1)
#define LOCATION_LEN              (4000+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllExternalLocations::tAttributes
 * See to study about [ALL_EXTERNAL_LOCATIONS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-964A4CE7-C3F9-4247-95F1-2E7DD29189C1.htm#REFRN20073)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllExternalLocations::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the external table location
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the corresponding external table
	char szLocation[LOCATION_LEN];         ///< External table location clause
	char szDirectoryOwner[OWNER_NAME_LEN]; ///< Owner of the directory containing the external table location
	char szDirectoryName[OBJECT_NAME_LEN]; ///< Name of the directory containing the external table location
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szLocation, 0, sizeof(szLocation));
        ::memset(szDirectoryOwner, 0, sizeof(szDirectoryOwner));
        ::memset(szDirectoryName, 0, sizeof(szDirectoryName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szLocation, rhs.szLocation
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
    void vPrinter(std::ostream& os, const size_t& i, cAllExternalLocations& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllExternalLocations::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllExternalLocations&) const
{
    os << boost::format("  %s  %s:'%s'")
            % (i == 0 ? "(" : ",")
            % ps::lib::sMakeEnclosedName(szDirectoryName)
            % szLocation
        << std::endl
        ;
}

std::string cAllExternalLocations::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szLocation='%s')"
        R"(, szDirectoryOwner='%s', szDirectoryName='%s')"
    ) % szOwner % szTableName % szLocation
        % szDirectoryOwner % szDirectoryName
    ;
    return oss.str();
}

/**
 * @struct cAllExternalLocations::cRetriever
 */
class cAllExternalLocations::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllExternalLocations::oList_.
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

const uint32_t cAllExternalLocations::cRetriever::iBulkSize_ = 10;

cAllExternalLocations::tAttributes cAllExternalLocations::cRetriever::rTable_[cAllExternalLocations::cRetriever::iBulkSize_];

cAllExternalLocations::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllExternalLocations
        , "Reading location of the external data", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLocation, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDirectoryOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDirectoryName, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllExternalLocations::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllExternalLocations::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllExternalLocations::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllExternalLocations::szAllExternalLocations[] = {
"select owner "
", table_name "
", location "
", directory_owner "
", directory_name "
"from all_external_locations "
"where owner in %s "
};

cAllExternalLocations::cAllExternalLocations()
{
    // nothing to do
}

cAllExternalLocations::~cAllExternalLocations()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllExternalLocations::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllExternalLocations::vPrintAllItems(
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
        , &cAllExternalLocations::tAttributes::iKeyMatched
        , &cAllExternalLocations::tAttributes::vPrinter
        , &cAllExternalLocations::vPrintPreRows
        , &cAllExternalLocations::vPrintPostRows
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
void cAllExternalLocations::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os << std::string("location") << std::endl;
}

void cAllExternalLocations::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string("  )") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
