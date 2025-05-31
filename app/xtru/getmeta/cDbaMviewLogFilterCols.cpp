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
 * @struct cDbaMviewLogFilterCols::tAttributes
 * See to study about [DBA_MVIEW_LOG_FILTER_COLS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-FBC62780-741F-4D54-A5D9-A658292BE916.htm#REFRN23136)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cDbaMviewLogFilterCols::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the master table being logged
	char szName[OBJECT_NAME_LEN];          ///< Name of the master table being logged
	char szColumnName[COLUMN_NAME_LEN];    ///< Column being logged
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szName, 0, sizeof(szName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szName, std::get<1>(oKey));
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
            , szName, rhs.szName
            , szColumnName, rhs.szColumnName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szName, rhs.szName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cDbaMviewLogFilterCols& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cDbaMviewLogFilterCols::tAttributes::vPrinter(std::ostream& os, const size_t& i, cDbaMviewLogFilterCols& oCtx) const
{
    os << boost::format("%c %s%s")
            % (i == 0 ? '(' : ',')
            % sColumnIdCooment(oCtx.oAllTabColumns_.iGetColumnId(szOwner, szName, szColumnName))
            % ps::lib::sMakeEnclosedName(szColumnName)
        << std::endl
        ;
}

std::string cDbaMviewLogFilterCols::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szName='%s', szColumnName='%s')"
    ) % szOwner % szName % szColumnName
    ;
    return oss.str();
}

/**
 * @struct cDbaMviewLogFilterCols::cRetriever
 */
class cDbaMviewLogFilterCols::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cDbaMviewLogFilterCols::oList_.
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

const uint32_t cDbaMviewLogFilterCols::cRetriever::iBulkSize_ = 100;

cDbaMviewLogFilterCols::tAttributes cDbaMviewLogFilterCols::cRetriever::rTable_[cDbaMviewLogFilterCols::cRetriever::iBulkSize_];

cDbaMviewLogFilterCols::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szDbaMviewLogFilterCols
        , "Reading mview log filter columns", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
}

void cDbaMviewLogFilterCols::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cDbaMviewLogFilterCols::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cDbaMviewLogFilterCols::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cDbaMviewLogFilterCols::szDbaMviewLogFilterCols[] = {
"select t1.owner "
", t1.name "
", t1.column_name "
"from dba_mview_log_filter_cols t1 "
"where t1.owner in %s "
"and not exists ( "
	// Exclude column names that compose PK.
	"select null "
	"from all_constraints t3 "
	", all_cons_columns t4 "
	"where t3.owner = t1.owner "
	"and t3.table_name = t1.name "
	"and t3.constraint_type = 'P' "
	"and t4.owner = t3.owner "
	"and t4.constraint_name = t3.constraint_name "
	"and t4.table_name = t3.table_name "
	"and t4.column_name = t1.column_name "
") "
};

cDbaMviewLogFilterCols::cDbaMviewLogFilterCols(
    cAllTabColumns& oAllTabColumns
)
    : oAllTabColumns_(oAllTabColumns)
{
    // nothing to do
}

cDbaMviewLogFilterCols::~cDbaMviewLogFilterCols()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cDbaMviewLogFilterCols::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cDbaMviewLogFilterCols::vPrintAllItems(
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
        , &cDbaMviewLogFilterCols::tAttributes::iKeyMatched
        , &cDbaMviewLogFilterCols::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cDbaMviewLogFilterCols::vPrintPostRows
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

void cDbaMviewLogFilterCols::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(") ");
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
