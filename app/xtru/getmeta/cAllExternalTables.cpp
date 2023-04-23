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

#define TYPE_OWNER_LEN             (3+1)
#define REJECT_LIMIT_LEN          (40+1)
#define ACCESS_TYPE_LEN            (7+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/** 
 * @struct cAllExternalTables::tAttributes
 * See to study about [ALL_EXTERNAL_TABLES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-B34CB41B-3ED0-4505-9C2C-E9A26705535A.htm#REFRN20074)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllExternalTables::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the external table
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the external table
	char szTypeOwner[TYPE_OWNER_LEN];      ///< Owner of the implementation type for the external table access driver
	char szTypeName[OBJECT_NAME_LEN];      ///< Name of the implementation type for the external table access driver
	char szDefaultDirectoryOwner[TYPE_OWNER_LEN];///< Owner of the default directory for the external table
	char szDefaultDirectoryName[OBJECT_NAME_LEN];///< Name of the default directory for the external table
	char szRejectLimit[REJECT_LIMIT_LEN];  ///< Reject limit for the external table, or UNLIMITED
    /// - BLOB
    /// - CLOB
	char szAccessType[ACCESS_TYPE_LEN];    ///< Type of access parameters for the external table
	char *szAccessParameters;              ///< Access parameters for the external table
	ps::lib::sql::ind_t nAccessParametersInd;///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iAccessParameters;                 ///< 
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szAccessParameters(nullptr)
        , nAccessParametersInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iAccessParameters(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szTypeOwner, 0, sizeof(szTypeOwner));
        ::memset(szTypeName, 0, sizeof(szTypeName));
        ::memset(szDefaultDirectoryOwner, 0, sizeof(szDefaultDirectoryOwner));
        ::memset(szDefaultDirectoryName, 0, sizeof(szDefaultDirectoryName));
        ::memset(szRejectLimit, 0, sizeof(szRejectLimit));
        ::memset(szAccessType, 0, sizeof(szAccessType));
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
     * Since tAttributes::szAccessParameters which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szAccessParameters(new char[rhs.iAccessParameters + 1])
        , nAccessParametersInd(rhs.nAccessParametersInd)
        , iAccessParameters(rhs.iAccessParameters)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szTypeOwner, 0, sizeof(szTypeOwner));
        ::memset(szTypeName, 0, sizeof(szTypeName));
        ::memset(szDefaultDirectoryOwner, 0, sizeof(szDefaultDirectoryOwner));
        ::memset(szDefaultDirectoryName, 0, sizeof(szDefaultDirectoryName));
        ::memset(szRejectLimit, 0, sizeof(szRejectLimit));
        ::memset(szAccessType, 0, sizeof(szAccessType));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szTypeOwner, rhs.szTypeOwner);
        ::strcpy(szTypeName, rhs.szTypeName);
        ::strcpy(szDefaultDirectoryOwner, rhs.szDefaultDirectoryOwner);
        ::strcpy(szDefaultDirectoryName, rhs.szDefaultDirectoryName);
        ::strcpy(szRejectLimit, rhs.szRejectLimit);
        ::strcpy(szAccessType, rhs.szAccessType);
        ::strcpy(szAccessParameters, rhs.szAccessParameters);
    }
    ~tAttributes()
    {
        delete [] szAccessParameters;
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
    void vPrinter(std::ostream& os, const size_t& i, cAllExternalTables& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllExternalTables::tAttributes::vPrinter(std::ostream& os, const size_t&, cAllExternalTables& oCtx) const
{
    // Convert the end-of-line sequence used by Windows to xUNIX.
    const std::string sWork =
        ps::lib::sConvertEolSequence(szAccessParameters, "\r\n", "\n");
    os << std::string("organization external (")
        << std::endl
        << boost::format("  type %s")
            % szTypeName
        << std::endl
        << boost::format("  default directory %s")
            % ps::lib::sMakeEnclosedName(szDefaultDirectoryName)
        << std::endl
        << std::string("  access parameters (")
        << std::endl
        << boost::format("    %s) ")
            % sWork
        << oPrintItems(oCtx.oAllExternalLocations_, std::make_tuple(szOwner, szTableName), true)
        << std::string(")")
        ;
}

std::string cAllExternalTables::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szTypeOwner='%s', szTypeName='%s')"
        R"(, szDefaultDirectoryOwner='%s', szDefaultDirectoryName='%s', szRejectLimit='%s', szAccessType='%s')"
        R"(, iAccessParameters=%u, szAccessParameters='%s')"
    ) % szOwner % szTableName % szTypeOwner % szTypeName
        % szDefaultDirectoryOwner % szDefaultDirectoryName % szRejectLimit % szAccessType
        % iAccessParameters % szAccessParameters
    ;
    return oss.str();
}

/**
 * @struct cAllExternalTables::cRetriever
 */
class cAllExternalTables::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllExternalTables::oList_.
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

const uint32_t cAllExternalTables::cRetriever::iBulkSize_ = 100;

cAllExternalTables::tAttributes cAllExternalTables::cRetriever::rTable_[cAllExternalTables::cRetriever::iBulkSize_];

cAllExternalTables::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllExternalTables
        , "Reading information of the external tables", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szAccessParameters, &rTable_->nAccessParametersInd, &rTable_->iAccessParameters)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTypeOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTypeName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefaultDirectoryOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDefaultDirectoryName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRejectLimit, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szAccessType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllExternalTables::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllExternalTables::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllExternalTables::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllExternalTables::szAllExternalTables[] = {
"select owner "
", table_name "
", type_owner "
", type_name "
", default_directory_owner "
", default_directory_name "
", reject_limit "
", access_type "
", access_parameters "
"from all_external_tables "
"where owner in %s "
"order by table_name "
};

cAllExternalTables::cAllExternalTables(
    cAllExternalLocations& oAllExternalLocations
)
    : oAllExternalLocations_(oAllExternalLocations)
{
    // nothing to do
}

cAllExternalTables::~cAllExternalTables()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllExternalTables::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllExternalTables::vPrintAllItems(
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
        , &cAllExternalTables::tAttributes::iKeyMatched
        , &cAllExternalTables::tAttributes::vPrinter
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

bool cAllExternalTables::iCheckExternal(
    const tKeyTuple& oKey
){
    auto it = oList_.begin(), ite = oList_.end();
    const tAttributes rhs(oKey);
    while (it != ite)
    {
        if (it->iKeyMatched(rhs))
        {
            return true;
        }
        ++it;
    }
    return false;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
