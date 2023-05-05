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
 * @struct cOutputOrderTbl::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * and [ALL_TABLES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-6823CD28-0681-468E-950B-966C6F71325D.htm#REFRN20286)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cOutputOrderTbl::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the cluster
    ub4 iLevel;
	char szTableName[OBJECT_NAME_LEN];     ///< Clustered table name
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iLevel(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
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
            , iLevel, rhs.iLevel
            , szTableName, rhs.szTableName
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
    void vPrinter(std::ostream& os, const size_t& i, cOutputOrderTbl& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cOutputOrderTbl::tAttributes::vPrinter(std::ostream& os, const size_t& i, cOutputOrderTbl& oCtx) const
{
    const std::tuple<const char*, const char*> oKey(szOwner, szTableName);
	char szClusterName[OBJECT_NAME_LEN] = {0};
    // Is  oKey a cluster table or not?
    const auto& iClusterName = oCtx.oAllTables_.iCheckClusterTable(oKey, szClusterName);
    // Is  oKey a IOT table or not?
    const auto& iIotType = oCtx.oAllTables_.iCheckIot(oKey);
    const auto& iExternal = oCtx.oAllExternalTables_.iCheckExternal(oKey);
    os << sPrintLabel("Tabs", i + 1, szTableName);
    if (! iClusterName)
    {
        os << boost::format("drop table %s") % ps::lib::sMakeEnclosedName(szTableName)
            << oPrintItems(oCtx.oParentWithChild_, oKey, true)
            << std::string(";") << std::endl
            ;
    }
    os << boost::format("create table %s") % ps::lib::sMakeEnclosedName(szTableName)
        << std::endl
        << oPrintItemsFN(oCtx.oAllTabColumns_, &cAllTabColumns::vPrintAllTabColumns,  oKey, true)
        << oPrintItems(oCtx.oConsOfAKey_, std::make_tuple(szOwner, szTableName, ""), true, iIotType, cConsOfAKey::iConsInTable)
        ;
    if (oCtx.iDependent_)
    {
        os  << oPrintItems(oCtx.oConsOfFKey_, oKey, true, cConsOfFKey::tInvCtx::iInCreTbl);
    }
    os 
        << oPrintItems(oCtx.oConsOfCheck_, oKey, true)
        << oPrintItems(oCtx.oAllLogGroups_, std::make_tuple(szOwner, szTableName, "GENERATED NAME"), true)
        << oPrintItems(oCtx.oAllLogGroups_, std::make_tuple(szOwner, szTableName, "USER NAME"), true)
        ;
    os << std::string(") ");
    if (iClusterName)
    {
        os << oPrintItems(oCtx.oUserCluColumns_, std::make_tuple(szOwner, szTableName, szClusterName), true);
    }
    else if (iIotType)
    {
         os << oPrintItems(oCtx.oUserIndexes_, std::make_tuple(szOwner, szTableName, ""), true, true);
    }
    else if (iExternal)
    {
         os << oPrintItems(oCtx.oAllExternalTables_, oKey, true);
    }
    else
    {
		/* Do nothing */
    }
    os 
        << oPrintItems(oCtx.oAllLobs_, oKey, true)
        << oPrintItemsFN(oCtx.oAllTables_, &cAllTables::vPrintFullItems, oKey, true)
        << std::string(";") << std::endl
        << oPrintItems(oCtx.oDataDefault_, std::make_tuple(szOwner, szTableName, ""), true)
        << oPrintItems(oCtx.oUserIndexes_, std::make_tuple(szOwner, szTableName, ""), true, false)
        << oPrintItems(oCtx.oEffectiveKeyCons_, std::make_tuple(szOwner, szTableName, cEffectiveKeyCons::iKeyTypeIsUnique, ""), true)
        << oPrintItems(oCtx.oAllTriggers_, oKey, true)
        << oPrintItems(oCtx.oAllTabPrivs_, oKey, true)
        << oPrintItems(oCtx.oUserObjAuditOpts_, oKey, true)
        << oPrintItems(oCtx.oUserAuditObject_, oKey, true)
        << oPrintItems(oCtx.oAllTabComments_, oKey, true)
        << oPrintItems(oCtx.oAllColComments_, oKey, true)
        << oPrintItems(oCtx.oAllMviewLogs_, oKey, true)
        ;
}

std::string cOutputOrderTbl::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s')"
    ) % szOwner % szTableName
    ;
    return oss.str();
}

/**
 * @struct cOutputOrderTbl::cRetriever
 */
class cOutputOrderTbl::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cOutputOrderTbl::oList_.
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
};

const uint32_t cOutputOrderTbl::cRetriever::iBulkSize_ = 100;

cOutputOrderTbl::tAttributes cOutputOrderTbl::cRetriever::rTable_[cOutputOrderTbl::cRetriever::iBulkSize_];

cOutputOrderTbl::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szOutputOrderTbl
        , "Reading the order of outputting", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder(
    { sGetSqlInList(oOwners)
    , sGetSqlInList(oOwners)
    , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iLevel, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cOutputOrderTbl::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cOutputOrderTbl::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cOutputOrderTbl::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cOutputOrderTbl::szOutputOrderTbl[] = {
/* Tables with integrity constraints */
"select owner "
", max(level) lv "
", table_name "
"from all_constraints "
"where constraint_type in ('P','U','R') "
"start with ( "
    "owner in %s "
    "and r_constraint_name is null "
") connect by ( "
    "prior owner = r_owner "
    "and prior constraint_name = r_constraint_name "
") group by owner, table_name "
"union "
/* Tables with no integrity constraints */
"select at.owner "
", 1 as lv "
", at.table_name "
"from all_tables at "
"where at.owner in %s "
"and not exists ( "
    "select null "
    "from all_constraints "
    "where owner = at.owner "
    "and table_name = at.table_name "
    "and constraint_type in ('P','U','R') "
") "
"and not ("
    "table_name like 'MLOG$\%' "
    "or table_name like 'RUPD$\%' "
") "
"and at.iot_type is null "
"and at.dropped = 'NO' "
"minus "
"select owner, 1, mview_name "
"from all_mviews "
"where owner in %s "
"and build_mode = 'IMMEDIATE' "
// "order by owner, lv, table_name "
};

cOutputOrderTbl::cOutputOrderTbl(
    const bool iDependent
    , cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
    , cAllTabColumns& oAllTabColumns
    , cAllTables& oAllTables
    , cAllTabComments& oAllTabComments
    , cAllColComments& oAllColComments
    , cUserCluColumns& oUserCluColumns
    , cEffectiveKeyCons& oEffectiveKeyCons
    , cParentWithChild& oParentWithChild
    , cConsOfCheck& oConsOfCheck
    , cConsOfFKey& oConsOfFKey
    , cConsOfAKey& oConsOfAKey
    , cAllLobs& oAllLobs
    , cDataDefault& oDataDefault
    , cAllTriggers& oAllTriggers
    , cAllMviewLogs& oAllMviewLogs
    , cUserIndexes& oUserIndexes
    , cAllExternalTables& oAllExternalTables
    , cAllLogGroups& oAllLogGroups
)
    : iNumRows_(0)
    , iDependent_(iDependent)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
    , oAllTabColumns_(oAllTabColumns)
    , oAllTables_(oAllTables)
    , oAllTabComments_(oAllTabComments)
    , oAllColComments_(oAllColComments)
    , oUserCluColumns_(oUserCluColumns)
    , oEffectiveKeyCons_(oEffectiveKeyCons)
    , oParentWithChild_(oParentWithChild)
    , oConsOfCheck_(oConsOfCheck)
    , oConsOfFKey_(oConsOfFKey)
    , oConsOfAKey_(oConsOfAKey)
    , oAllLobs_(oAllLobs)
    , oDataDefault_(oDataDefault)
    , oAllTriggers_(oAllTriggers)
    , oAllMviewLogs_(oAllMviewLogs)
    , oUserIndexes_(oUserIndexes)
    , oAllExternalTables_(oAllExternalTables)
    , oAllLogGroups_(oAllLogGroups)
{
    // nothing to do
}

cOutputOrderTbl::~cOutputOrderTbl()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cOutputOrderTbl::oSubmit(
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

void cOutputOrderTbl::vPrintAllItems(
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
        , &cOutputOrderTbl::tAttributes::iKeyMatched
        , &cOutputOrderTbl::tAttributes::vPrinter
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
