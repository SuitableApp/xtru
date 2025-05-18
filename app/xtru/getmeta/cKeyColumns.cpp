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

constexpr size_t CONSTR_TYPE_LEN = 1+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cKeyColumns::tAttributes
 * See to study about [ALL_CONS_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-5B08133E-40D0-4A9E-9033-BFCC1493BA3F.htm#REFRN20045)
 * , [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cKeyColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];          /// Name of owner.
	char szTableName[OBJECT_NAME_LEN];      /// Name of table.
	char szColumnName[OBJECT_NAME_LEN];     /// ColumnName on created.
	char szConstraintType[CONSTR_TYPE_LEN]; /// Type of constraint. One of 'P' or 'R'.
	char szConstraintName[OBJECT_NAME_LEN]; /// Name of constraint.
	ub4 iColumnId;                          /// ID of column in the table.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnId(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::memset(szConstraintType, 0, sizeof(szConstraintType));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szColumnName, std::get<2>(oKey));
        ::strcpy(szConstraintType, std::get<3>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", "", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szColumnName, rhs.szColumnName
            , szConstraintType, rhs.szConstraintType
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szColumnName, rhs.szColumnName
            , szConstraintType, rhs.szConstraintType
        );
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cKeyColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szTableName='%s', szColumnName='%s', szConstraintType='%s')"
            R"(, szConstraintName='%s', iColumnId=%u)"
        ) % szOwner % szTableName % szConstraintType
            % szConstraintName % iColumnId % szColumnName
        ;
    return oss.str();
}

/**
 * @struct cAllTabColumns::cRetriever
 */
class cKeyColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cKeyColumns::oList_.
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

const uint32_t cKeyColumns::cRetriever::iBulkSize_ = 100;

cKeyColumns::tAttributes cKeyColumns::cRetriever::rTable_[cKeyColumns::cRetriever::iBulkSize_];

cKeyColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szKeyColumns
        , "Reading columns organizing key constraints", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
}

void cKeyColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cKeyColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cKeyColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cKeyColumns::szKeyColumns[] = {
"select /*+ ordered */ t1.owner "
", t1.table_name  "
", t2.column_name "
", t1.constraint_type "
", t2.constraint_name "
", tc.column_id "
"from all_constraints t1 "
", all_cons_columns t2 "
", all_tab_columns tc "
"where t1.owner in %s "
"and t1.constraint_type in ('P','R') "
"and t2.owner = t1.owner "
"and t2.constraint_name = t1.constraint_name "
"and t2.table_name = t1.table_name "
"and tc.owner = t2.owner "
"and tc.table_name = t2.table_name "
"and tc.column_name = t2.column_name "
};

cKeyColumns::cKeyColumns()
{
    // nothing to do
}

cKeyColumns::~cKeyColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cKeyColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

ps::lib::sql::ind_t cKeyColumns::iDoesConstraintHas(
    const tKeyTuple& oKey
){
    ps::lib::cList<tIter> oArr;
    const std::string str = sConcatMatched(
        *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , [](const tAttributes& lhs, const tAttributes& rhs)
            { return ps::lib::composite_equal(
                lhs.szOwner, rhs.szOwner
                , lhs.szTableName, rhs.szTableName
                , lhs.szColumnName, rhs.szColumnName
                , lhs.szConstraintType, rhs.szConstraintType
            );}
        , [](const tAttributes&, const size_t&, cKeyColumns&){ return std::string(""); }
        , [](cKeyColumns&){ return std::string("Founded!"); }
        , [](cKeyColumns&){ return std::string(""); }
        , [](cKeyColumns&){ return std::string(""); }
        , std::back_inserter(oArr)
    );
    boost::for_each(
        oArr, [this](tIter it){oList_.erase(it);}
    );
    return
        str.size()
        ? ps::lib::sql::ind_t::VAL_IS_NOTNULL
        : ps::lib::sql::ind_t::VAL_IS_NULL
    ;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

