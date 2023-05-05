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

#define TAB_COL_NAME_LEN    (4000+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cUserCluColumns::tAttributes
 * See to study about [DBA_CLU_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-844ADF06-067C-47E4-AD9E-54A88FDC6FF2.htm#REFRN23037)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cUserCluColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the cluster
	char szTableName[OBJECT_NAME_LEN];     ///< Clustered table name
	char szClusterName[OBJECT_NAME_LEN];   ///< Name of the cluster
	char szTabColumnName[TAB_COL_NAME_LEN];///< Key column or attribute of the object type column
	ub4 iColumnId;                         ///< associated with all_tab_columns.column_id
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnId(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szClusterName, 0, sizeof(szClusterName));
        ::memset(szTabColumnName, 0, sizeof(szTabColumnName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szClusterName, std::get<2>(oKey));
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
            , szClusterName, rhs.szClusterName
            , iColumnId, rhs.iColumnId
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szClusterName, rhs.szClusterName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserCluColumns& oCtx) const
    {
        os << boost::format("%s%s")
            % (i == 0 ? "(" : ", ")
            % ps::lib::sMakeEnclosedName(szTabColumnName)
        ;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cUserCluColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szClusterName='%s')"
        R"(, szTabColumnName='%s', iColumnId=%u)"
    ) % szOwner % szTableName % szClusterName
        % szTabColumnName % iColumnId
    ;
    return oss.str();
}

/**
 * @struct cUserCluColumns::cRetriever
 */
class cUserCluColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserCluColumns::oList_.
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
    bool iAccesible() const
    {
        const auto& oPrivsGiven(cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_CLU_COLUMNS", "SELECT"});
    }
};

const uint32_t cUserCluColumns::cRetriever::iBulkSize_ = 10;

cUserCluColumns::tAttributes cUserCluColumns::cRetriever::rTable_[cUserCluColumns::cRetriever::iBulkSize_];

cUserCluColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , iAccesible() ? szDbaCluColumns : szUserCluColumns
        , "Reading keys for clusters", &oBind_
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
    oDefine_.vAddItem(rTable_->szClusterName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTabColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_UIN, NULL, NULL, NULL, iSkip_);
}

void cUserCluColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserCluColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserCluColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cUserCluColumns::szDbaCluColumns[] = {
"select t3.owner "
", t3.table_name "
", t3.cluster_name "
", t3.tab_column_name "
", t2.column_id "
"from all_clusters t1 "
", all_tab_columns t2 "
", dba_clu_columns t3 "
"where t1.owner in %s "
"and t2.owner = t1.owner "
"and t2.table_name = t1.cluster_name "
"and t3.owner = t2.owner "
"and t3.cluster_name = t2.table_name "
"and t3.clu_column_name = t2.column_name "
};

const char cUserCluColumns::szUserCluColumns[] = {
"select user "
", t3.table_name "
", t3.cluster_name "
", t3.tab_column_name "
", t2.column_id "
"from all_clusters t1 "
", all_tab_columns t2 "
", user_clu_columns t3 "
"where t1.owner in %s "
"and t2.owner = t1.owner "
"and t2.table_name = t1.cluster_name "
"and user = t2.owner "
"and t3.cluster_name = t2.table_name "
"and t3.clu_column_name = t2.column_name "
};

cUserCluColumns::cUserCluColumns()
{
    // nothing to do
}

cUserCluColumns::~cUserCluColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserCluColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cUserCluColumns::vPrintAllItems(
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
        , &cUserCluColumns::tAttributes::iKeyMatched
        , &cUserCluColumns::tAttributes::vPrinter
        , &cUserCluColumns::vPrintPreRows
        , &cUserCluColumns::vPrintPostRows
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

void cUserCluColumns::vPrintPreRows(std::ostream& os, const tAttributes& rKey) const
{
    os << boost::format("cluster %s ") % ps::lib::sMakeEnclosedName(rKey.szClusterName);
}

void cUserCluColumns::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
