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
 * @struct cAllConsColumns::tAttributes
 * See to study about [ALL_CONS_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-5B08133E-40D0-4A9E-9033-BFCC1493BA3F.htm#REFRN20045)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllConsColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the constraint definition
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table with the constraint definition
	char szConstraintName[OBJECT_NAME_LEN];///< Name of the constraint definition
	char szColumnName[OBJECT_NAME_LEN];    ///< Name of the column or attribute of the object type column specified in the constraint definition
	ub4 iPosition;                         ///< Original position of the column or attribute in the definition of the object
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iPosition(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szConstraintName, std::get<2>(oKey));
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
            , szConstraintName, rhs.szConstraintName
            , iPosition, rhs.iPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllConsColumns& oCtx) const
    {
        os << std::string(2, ' ')
            << boost::format("%s%s%s")
            % (i == 0 ? "( " : ", ")
            % sColumnIdCooment(oCtx.oAllTabColumns_.iGetColumnId(
                szOwner, szTableName, szColumnName
            ))
            % ps::lib::sMakeEnclosedName(szColumnName)
            << std::endl
        ;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllConsColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szConstraintName='%s')"
        R"(, szColumnName='%s', iPosition=%u)"
    ) % szOwner % szTableName % szConstraintName
        % szColumnName % iPosition
    ;
    return oss.str();
}

/**
 * @struct cAllConsColumns::cRetriever
 */
class cAllConsColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllConsColumns::oList_.
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
        , const bool& iQueryType
    );
private:
    /// @copybrief cAllSequences::cRetriever::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::cRetriever::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::cRetriever::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllConsColumns::cRetriever::iBulkSize_ = 100;

cAllConsColumns::tAttributes cAllConsColumns::cRetriever::rTable_[cAllConsColumns::cRetriever::iBulkSize_];

cAllConsColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const bool& iQueryType
)
    : cDescriber(oSvc, iBulkSize_
        , iQueryType
            ? szAllConsColumnsKeyA
            : szAllConsColumnsRefA
        , iQueryType
            ? "Reading column names in PK, AK, FK"
            : "Reading Column names in AK referenced by FK"
        , &oBind_
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
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPosition, SQLT_UIN, NULL, NULL, NULL, iSkip_);
}

void cAllConsColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllConsColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllConsColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllConsColumns::szAllConsColumnsKeyA[] = {
"select cc.owner "
", cc.table_name "
", cc.constraint_name "
", cc.column_name "
", cc.position "
"from all_cons_columns cc "
"where cc.owner in %s "
"and cc.position is not null "
};

const char cAllConsColumns::szAllConsColumnsRefA[] = {
"select t2.owner "
", t2.table_name "
", t2.constraint_name "
", t2.column_name "
", t2.position "
"from all_constraints t1 "
", all_cons_columns t2 "
"where t1.owner in %s "
"and t1.constraint_type in ('P', 'U') "
"and t2.owner = t1.owner "
"and t2.table_name = t1.table_name "
"and t2.constraint_name = t1.constraint_name "
};

cAllConsColumns::cAllConsColumns(
    const bool iQueryType
    , cAllTabColumns& oAllTabColumns
)
    : iQueryType_(iQueryType)
    , oAllTabColumns_(oAllTabColumns)
{
    // nothing to do
}

cAllConsColumns::~cAllConsColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllConsColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , iQueryType_
    );
}

void cAllConsColumns::vPrintAllItems(
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
        , &cAllConsColumns::tAttributes::iKeyMatched
        , &cAllConsColumns::tAttributes::vPrinter
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
