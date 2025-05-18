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

constexpr size_t LOG_GROUP_TYPE_LEN = 19+1;
constexpr size_t ALWAYS_LEN = 11+1;
constexpr size_t NAMETYPE_LEN = 14+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllLogGroups::tAttributes
 * See to study about [ALL_LOG_GROUPS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-A5E53349-0F54-46F5-85A2-4A165570215A.htm#REFRN20120)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllLogGroups::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the log group definition
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table on which the log group is defined
	char szLogGroupName[OBJECT_NAME_LEN];  ///< Name of the log group definition
    /// - PRIMARY KEY LOGGING
    /// - UNIQUE KEY LOGGING
    /// - FOREIGN KEY LOGGING
    /// - ALL COLUMN LOGGING
    /// - USER LOG GROUP
	char szLogGroupType[LOG_GROUP_TYPE_LEN];///< Type of the log group:
    ps::lib::sql::ind_t nLogGroupTypeInd;  ///< ind_t::VAL_IS_NULL: value is NULL
    /// - Y: a row is updated
    /// - N: a member column is updated
	char szAlways[ALWAYS_LEN];             ///< indicates the log group is logged any time
    ps::lib::sql::ind_t nAlwaysInd;        ///< ind_t::VAL_IS_NULL: value is NULL
    /// - GENERATED NAME : log group was system generated 
    /// - USER NAME : or not
	char szGenerated[NAMETYPE_LEN];        ///< Indicates whether the name of the supplemental
    ps::lib::sql::ind_t nGeneratedInd;     ///< ind_t::VAL_IS_NULL: value is NULL
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nLogGroupTypeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nAlwaysInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nGeneratedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szLogGroupName, 0, sizeof(szLogGroupName));
        ::memset(szLogGroupType, 0, sizeof(szLogGroupType));
        ::memset(szAlways, 0, sizeof(szAlways));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szGenerated, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szGenerated, rhs.szGenerated
            , szLogGroupName, rhs.szLogGroupName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szGenerated, rhs.szGenerated
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinterNamed(std::ostream& os, const size_t& i, cAllLogGroups&) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinterGenerated(std::ostream& os, const size_t& i, cAllLogGroups&) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllLogGroups::tAttributes::vPrinterNamed(std::ostream& os, const size_t& i, cAllLogGroups& oCtx) const
{
    os << boost::format(", supplemental log group %s ")
            % ps::lib::sMakeEnclosedName(szLogGroupName)
        << oPrintItems(oCtx.oAllLogGroupColumns_, std::make_tuple(szOwner, szTableName, szLogGroupName), true)
        << boost::format("%s")
            % (ps::lib::iCompBinary(strcmp, szAlways, "ALWAYS") ? " always" : "")
        << std::endl
        ;
}

void cAllLogGroups::tAttributes::vPrinterGenerated(std::ostream& os, const size_t& i, cAllLogGroups& oCtx) const
{
    const static ps::lib::cMap<std::string, std::string> oMap{
          {"PRIMARY KEY LOGGING", "primary key"}
        , {"UNIQUE KEY LOGGING", "unique index"}
        , {"FOREIGN KEY LOGGING", "foreign key"}
        , {"ALL COLUMN LOGGING", "all"}
    };
    const auto& it = oMap.find(szLogGroupType);
    if (it != oMap.end())
    {
        os << boost::format("%s%s")
                % (i == 0 ? "(" : ", ")
                % it->second
            ;
    }
    else 
    {
        RAISE_EX_CONVERT(
            std::runtime_error
            , boost::format("Un-supported group type %s used in %s.%s")
            % ps::lib::sMakeEnclosedName(szLogGroupType)
            % ps::lib::sMakeEnclosedName(szOwner)
            % ps::lib::sMakeEnclosedName(szTableName)
        );
    }
}

std::string cAllLogGroups::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szLogGroupName='%s', szLogGroupType='%s'(%d))"
        R"(, szAlways='%u'(%d), szGenerated='%s'(%d))"
    ) % szOwner % szTableName % szLogGroupName % szLogGroupType % nLogGroupTypeInd
        % szAlways % nAlwaysInd % szGenerated % nGeneratedInd
    ;
    return oss.str();
}

/**
 * @struct cAllLogGroups::cRetriever
 */
class cAllLogGroups::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllLogGroups::oList_.
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

const uint32_t cAllLogGroups::cRetriever::iBulkSize_ = 100;

cAllLogGroups::tAttributes cAllLogGroups::cRetriever::rTable_[cAllLogGroups::cRetriever::iBulkSize_];

cAllLogGroups::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllLogGroups
        , "Reading supplimental log group properties", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    const auto later_10g1 = oSvc.iCompDbVersion("1001",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        (later_10g1 ? "log_group_type" : "'USER LOG GROUP'")
        , (later_10g1 ? "generated" : "'USER NAME'")
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szLogGroupName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szLogGroupType, SQLT_STR, &rTable_->nLogGroupTypeInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szAlways, SQLT_STR, &rTable_->nAlwaysInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, &rTable_->nGeneratedInd, nullptr, nullptr, iSkip_);
}

void cAllLogGroups::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllLogGroups::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllLogGroups::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllLogGroups::szAllLogGroups[] = {
"select owner "
", table_name "
", log_group_name "
", %s as log_group_type "
", always "
", %s as generated "
"from all_log_groups "
"where owner in %s "
};

cAllLogGroups::cAllLogGroups(
    cAllLogGroupColumns& oAllLogGroupColumns
)
    : oAllLogGroupColumns_(oAllLogGroupColumns)
{
    // nothing to do
}

cAllLogGroups::~cAllLogGroups()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllLogGroups::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllLogGroups::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
){
    const bool iNamed = ps::lib::iCompBinary(strcmp, std::get<2>(oKey), "USER NAME");
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cAllLogGroups::tAttributes::iKeyMatched
        , (iNamed  ? &cAllLogGroups::tAttributes::vPrinterNamed
                   : &cAllLogGroups::tAttributes::vPrinterGenerated)
        , (iNamed ? static_cast<tOneTimeAction>(nullptr)
                   : &cAllLogGroups::vPrintPreRows)
        , (iNamed ? static_cast<tOneTimeAction>(nullptr)
                   : &cAllLogGroups::vPrintPostRows)
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

void cAllLogGroups::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(", supplemental log data ");
}

void cAllLogGroups::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(") columns") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
