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

constexpr size_t LONG_COL_NAME_LEN = 4000+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllSubPartKeyColumns::tAttributes
 * See to study about [ALL_SUBPART_KEY_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-55D9D12F-A915-4F2F-86D0-F88C4A2F186C.htm#REFRN20262)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllSubPartKeyColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the partitioned table or index.
	char szName[OBJECT_NAME_LEN];          ///< Name of the partitioned table or index.
	char szObjectType[OBJECT_TYPE_LEN];    ///< Object type: "TABLE" or "INDEX".
	char szColumnName[LONG_COL_NAME_LEN];  ///< Column name.
	uint32_t iColumnPosition;              ///< Position of the column within the subpartitioning key.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnPosition(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szName, 0, sizeof(szName));
        ::memset(szObjectType, 0, sizeof(szObjectType));
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
            , iColumnPosition, rhs.iColumnPosition
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
    void vPrinter(std::ostream& os, const size_t& i, cAllSubPartKeyColumns& oCtx) const
    {
        os << boost::format("%s%s")
            % (i == 0 ? "(" : ", ")
            % ps::lib::sMakeEnclosedName(szColumnName)
        ;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllSubPartKeyColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szName='%s', szObjectType='%s', szColumnName='%s')"
        R"(, iColumnPosition=%d)"
    ) % szOwner % szName % szObjectType % szColumnName
        % iColumnPosition
    ;
    return oss.str();
}

/**
 * @struct cAllSubPartKeyColumns::cRetriever
 */
class cAllSubPartKeyColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSubPartKeyColumns::oList_.
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

const uint32_t cAllSubPartKeyColumns::cRetriever::iBulkSize_ = 10;

cAllSubPartKeyColumns::tAttributes cAllSubPartKeyColumns::cRetriever::rTable_[cAllSubPartKeyColumns::cRetriever::iBulkSize_];

cAllSubPartKeyColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllSubPartKeyColumns
        , "Reading subpartitioning key column name", &oBind_
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
    oDefine_.vAddItem(rTable_->szObjectType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnPosition, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
}

void cAllSubPartKeyColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllSubPartKeyColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllSubPartKeyColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllSubPartKeyColumns::szAllSubPartKeyColumns[] = {
"select t1.owner "
", t1.name "
", t1.object_type "
", t1.column_name "
", t1.column_position "
"from all_subpart_key_columns t1 "
"where t1.owner in %s "
"and not exists ("
	"select null  "
	"from all_part_indexes ip "
	"where ip.owner = t1.owner "
	"and ip.index_name = t1.name "
	"and ip.locality = 'LOCAL' "
") "
};

cAllSubPartKeyColumns::cAllSubPartKeyColumns()
{
    // nothing to do
}

cAllSubPartKeyColumns::~cAllSubPartKeyColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllSubPartKeyColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllSubPartKeyColumns::vPrintAllItems(
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
        , &cAllSubPartKeyColumns::tAttributes::iKeyMatched
        , &cAllSubPartKeyColumns::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllSubPartKeyColumns::vPrintPostRows
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

void cAllSubPartKeyColumns::vPrintPostRows(std::ostream& os, const tAttributes& rKey) const
{
    os << std::string(")") << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
