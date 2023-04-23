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

#define PROPERTY_LEN                      (6+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllLogGroupColumns::tAttributes
 * See to study about [ALL_LOG_GROUP_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-8BA9188F-38FF-432B-911E-47AF6F00CC1A.htm#REFRN20119)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllLogGroupColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the log group definition
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table in which the log group is defined
	char szLogGroupName[OBJECT_NAME_LEN];  ///< Name of the log group definition
	char szColumnName[OBJECT_NAME_LEN];    ///< Name of the column or attribute of the object type column specified in the log group definition
    ps::lib::sql::ind_t nColumnNameInd;    ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iPosition;                         ///< Original position of the column or attribute in the definition of the object
    ps::lib::sql::ind_t nPositionInd;      ///< ind_t::VAL_IS_NULL: value is NULL
    /// - logged (LOG)
    /// - or not (NO LOG)
	char szLoggingProperty[PROPERTY_LEN];  ///< Indicates whether the column or attribute would be supplementally:
    ps::lib::sql::ind_t nLoggingPropertyInd;///< ind_t::VAL_IS_NULL: value is NULL
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nColumnNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPosition(0)
        , nPositionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nLoggingPropertyInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szLogGroupName, 0, sizeof(szLogGroupName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::memset(szLoggingProperty, 0, sizeof(szLoggingProperty));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szLogGroupName, std::get<2>(oKey));
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
            , szLogGroupName, rhs.szLogGroupName
            , iPosition, rhs.iPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szLogGroupName, rhs.szLogGroupName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllLogGroupColumns& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllLogGroupColumns::tAttributes::vPrinter(std::ostream& os, const size_t& i, cAllLogGroupColumns&) const
{
    os << boost::format("%s %s%s")
            % (i == 0 ? "(" : ",")
            % ps::lib::sMakeEnclosedName(szColumnName)
            % (ps::lib::iCompBinary(strcmp, szLoggingProperty, "NO LOG") ? " no log" : "")
        ;
}

std::string cAllLogGroupColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szLogGroupName='%s', szColumnName='%s'(%d))"
        R"(, iPosition='%u'(%d), szLoggingProperty='%s'(%d))"
    ) % szOwner % szTableName % szLogGroupName % szColumnName % nColumnNameInd
        % iPosition % nPositionInd % szLoggingProperty % nLoggingPropertyInd
    ;
    return oss.str();
}

/**
 * @struct cAllLogGroupColumns::cRetriever
 */
class cAllLogGroupColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllLogGroupColumns::oList_.
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

const uint32_t cAllLogGroupColumns::cRetriever::iBulkSize_ = 100;

cAllLogGroupColumns::tAttributes cAllLogGroupColumns::cRetriever::rTable_[cAllLogGroupColumns::cRetriever::iBulkSize_];

cAllLogGroupColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllLogGroupColumns
        , "Reading supplimental log group columns", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    const auto later_10g1 = oSvc.iCompDbVersion("1001",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        (later_10g1 ? "logging_property" : "'LOG'")
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogGroupName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, &rTable_->nColumnNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPosition, SQLT_UIN, &rTable_->nPositionInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLoggingProperty, SQLT_STR, &rTable_->nLoggingPropertyInd, NULL, NULL, iSkip_);
}

void cAllLogGroupColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllLogGroupColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllLogGroupColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllLogGroupColumns::szAllLogGroupColumns[] = {
"select owner "
", table_name "
", log_group_name "
", column_name "
", position "
", %s as logging_property "
"from all_log_group_columns "
"where owner in %s "
};

cAllLogGroupColumns::cAllLogGroupColumns()
{
    // nothing to do
}

cAllLogGroupColumns::~cAllLogGroupColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllLogGroupColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllLogGroupColumns::vPrintAllItems(
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
        , &cAllLogGroupColumns::tAttributes::iKeyMatched
        , &cAllLogGroupColumns::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllLogGroupColumns::vPrintPostRows
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

void cAllLogGroupColumns::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(")");
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
