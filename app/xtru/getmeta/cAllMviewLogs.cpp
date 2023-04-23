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

#define LOGGING_TYPE_LEN               (3+1)
#define CACHE_TYPE_LEN                 (1+1)
#define ROWIDS_LEN                     (3+1)
#define PK_TYPE_LEN                    (3+1)
#define OBJECT_ID_LEN                  (3+1)
#define FILTER_COL_LEN                 (3+1)
#define SEQUENCE_LEN                   (3+1)
#define INCLUDE_NEW_VALUES_LEN         (3+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllMviewLogs::tAttributes
 * See to study about [ALL_MVIEW_LOGS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-B7F8737C-AC38-4DDB-9DEB-2D2CA1D7571D.htm#REFRN20137)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllMviewLogs::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the materialized view log
	char szMaster[OBJECT_NAME_LEN];        ///< Name of the master table or master materialized view whose changes are logged
	char szLogTable[OBJECT_NAME_LEN];      ///< Name of the table where the changes to the master table or master materialized view are logged
	ub4 iPctFree;                          ///< associated with ALL_TABLES.
	ub4 iPctUsed;                          ///< associated with ALL_TABLES.
    ps::lib::sql::ind_t nPctUsedInd;       ///< equal ind_t::VAL_IS_NULL in ASSM
	ub4 iIniTrans;                         ///< associated with ALL_TABLES.
	ub4 iMaxTrans;                         ///< associated with ALL_TABLES.
	char szLogging[LOGGING_TYPE_LEN];      ///< associated with ALL_TABLES.
	ub4 iPctIncrease;                      ///< associated with ALL_TABLES.
    ps::lib::sql::ind_t nPctIncreaseInd;   ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iFreelists;                        ///< associated with ALL_TABLES.
    ps::lib::sql::ind_t nFreelistsInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iFreelistGroups;                   ///< associated with ALL_TABLES.
    ps::lib::sql::ind_t nFreelistGroupsInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szBufferPool[BUFFER_POOL_LEN];    ///< associated with ALL_TABLES.
    ps::lib::sql::ind_t nBufferPoolInd    ;///< ind_t::VAL_IS_NULL: value is NULL
	char szTablespaceName[OBJECT_NAME_LEN];///< associated with ALL_TABLES.
	ps::lib::sql::ind_t nTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szDegree[DEGREE_LEN];             ///< associated with ALL_TABLES.
	char szInstances[INSTANCES_LEN];       ///< associated with ALL_TABLES.
	char szCache[CACHE_TYPE_LEN];          ///< associated with ALL_TABLES.
	char szRowIds[ROWIDS_LEN];             ///< Indicates whether rowid information is recorded (YES) or not (NO)
	char szPrimaryKey[PK_TYPE_LEN];        ///< Indicates whether primary key information is recorded (YES) or not (NO)
	char szObjectId[OBJECT_ID_LEN];        ///< Indicates whether object identifier information in an object table is recorded (YES) or not (NO)
	char szFilterColumns[FILTER_COL_LEN];  ///< Indicates whether filter column information is recorded (YES) or not (NO)
	char szSequence[SEQUENCE_LEN];         ///< Indicates whether the sequence value, which provides additional ordering information, is recorded (YES) or not (NO)
	char szIncludeNewValues[INCLUDE_NEW_VALUES_LEN];///< 
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iPctFree(0)
        , iPctUsed(0)
        , nPctUsedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iIniTrans(0)
        , iMaxTrans(0)
        , iPctIncrease(0)
        , nPctIncreaseInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelists(0)
        , nFreelistsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelistGroups(0)
        , nFreelistGroupsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nBufferPoolInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szMaster, 0, sizeof(szMaster));
        ::memset(szLogTable, 0, sizeof(szLogTable));
        ::memset(szLogging, 0, sizeof(szLogging));
        ::memset(szBufferPool, 0, sizeof(szBufferPool));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szDegree, 0, sizeof(szDegree));
        ::memset(szInstances, 0, sizeof(szInstances));
        ::memset(szCache, 0, sizeof(szCache));
        ::memset(szRowIds, 0, sizeof(szRowIds));
        ::memset(szPrimaryKey, 0, sizeof(szPrimaryKey));
        ::memset(szObjectId, 0, sizeof(szObjectId));
        ::memset(szFilterColumns, 0, sizeof(szFilterColumns));
        ::memset(szSequence, 0, sizeof(szSequence));
        ::memset(szIncludeNewValues, 0, sizeof(szIncludeNewValues));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szMaster, std::get<1>(oKey));
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
            , szMaster, rhs.szMaster
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szMaster, rhs.szMaster
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllMviewLogs& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cAllMviewLogs::tAttributes::vPrinter(std::ostream& os, const size_t&, cAllMviewLogs& oCtx) const
{
    using ps::lib::iCompBinary;
    const std::string sPctUsed =
        nPctUsedInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
        ? (boost::format(" pctused %u") % iPctUsed ).str()
        : std::string("")
        ;
    os << std::string("create materialized view log on ")
        << boost::format("%s") % ps::lib::sMakeEnclosedName(szMaster)
        << std::endl
        << boost::format("  pctfree %u%s initrans %u maxtrans %u ")
            % iPctFree % sPctUsed % (iIniTrans ? iIniTrans : 2) % iMaxTrans
        << boost::format("%s %s parallel (degree %s instances %s) ")
            % (iCompBinary(strcmp, szLogging, "YES") ? "logging" : "nologging")
            % (iCompBinary(strcmp, szCache, "Y") ? "cache" : "nocache")
            % szDegree
            % szInstances
        << std::endl
        << std::string("  storage (")
        << oPrintItems(oCtx.oUserSegments_, std::make_tuple(szOwner, szMaster, "TABLE", ""), true)
    ;

    if (nPctIncreaseInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("pctincrease %u ") % iPctIncrease;
    }
    if (nFreelistsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("freelists %u ") % iFreelists;
    }
    if (nFreelistGroupsInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("freelist groups %u ") % iFreelistGroups;
    }
    if (nBufferPoolInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("buffer_pool %s) ") % szBufferPool;
    }
    if (nTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
    {
        os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
    }
    // 
	bool iNumWith = false;
    std::ostringstream oss;
    if (iCompBinary(strcmp, szRowIds, "YES"))
    {
        oss << std::string("rowid");
		iNumWith = true;
    }
    if (iCompBinary(strcmp, szPrimaryKey, "YES"))
    {
        oss << boost::format("%s%s") % (iNumWith ? ", " : "") % "primary key";
		iNumWith = true;
    }
    if (iCompBinary(strcmp, szObjectId, "YES"))
    {
        oss << boost::format("%s%s") % (iNumWith ? ", " : "") % "object id";
		iNumWith = true;
    }
    if (iCompBinary(strcmp, szSequence, "YES"))
    {
        oss << boost::format("%s%s") % (iNumWith ? ", " : "") % "sequence";
		iNumWith = true;
    }
    // 
    os << std::endl << boost::format("with %s ") % oss.str();
    // Describe the filter column to be logged.
    if (oCtx.oDbaMviewLogFilterCols_) // this is a reference to the smart-pointer.
    {
        oCtx.oDbaMviewLogFilterCols_->vPrintAllItems(
            os, std::make_tuple(szOwner, szMaster), true
        );
    }
    os << boost::format("%s new values")
            % (iCompBinary(strcmp, szIncludeNewValues, "YES") ? "including" : "excluding")
        << std::endl << std::string(";") << std::endl
        // Print object privileges
        << oPrintItems(oCtx.oAllTabPrivs_, std::make_tuple(szOwner, szLogTable), true)
        // Print audit optionS
        << oPrintItems(oCtx.oUserObjAuditOpts_, std::make_tuple(szOwner, szLogTable), true)
        // Print audit trails
        << oPrintItems(oCtx.oUserAuditObject_, std::make_tuple(szOwner, szLogTable), true)
        // Print table comment.
        << oPrintItems(oCtx.oAllTabComments_, std::make_tuple(szOwner, szLogTable), true)
        // Print column comments.
        << oPrintItems(oCtx.oAllColComments_, std::make_tuple(szOwner, szLogTable), true)
    ;
}

std::string cAllMviewLogs::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        "szOwner='%s', szMaster='%s', szLogTable='%s'"
        ", iPctFree=%u, iPctUsed=%u(%d), iIniTrans=%u, iMaxTrans=%u, szLogging='%s'"
        ", iPctIncrease=%u(%d), iFreelists=%u(%d), iFreelistGroups=%u(%d)"
        ", szBufferPool='%s'(%d), szTablespaceName='%s'(%d)"
        ", szDegree='%s', szInstances='%s', szCache='%s', szRowIds='%s', szPrimaryKey='%s'"
        ", szObjectId='%s', szFilterColumns='%s', szSequence='%s', szIncludeNewValues='%s'"
    ) % szOwner % szMaster % szLogTable
        % iPctFree % iPctUsed % nPctUsedInd % iIniTrans % iMaxTrans % szLogging
        % iPctIncrease % nPctIncreaseInd % iFreelists % nFreelistsInd % iFreelistGroups % nFreelistGroupsInd
        % szBufferPool % nBufferPoolInd % szTablespaceName % nTablespaceNameInd
        % szDegree % szInstances % szCache % szRowIds % szPrimaryKey
        % szObjectId % szFilterColumns % szSequence % szIncludeNewValues
    ;
    return oss.str();
}

/**
 * @struct cAllMviewLogs::cRetriever
 */
class cAllMviewLogs::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllMviewLogs::oList_.
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

const uint32_t cAllMviewLogs::cRetriever::iBulkSize_ = 100;

cAllMviewLogs::tAttributes cAllMviewLogs::cRetriever::rTable_[cAllMviewLogs::cRetriever::iBulkSize_];

cAllMviewLogs::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllMviewLogs
        , "Reading mview log properties", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    const auto later_9ir2 = oSvc.iCompDbVersion("0902",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_9ir2 ? "object_id" : "'NO'"
        , later_9ir2 ? "sequence" : "'NO'"
        , later_9ir2 ? "include_new_values" : "'NO'"
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szMaster, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogTable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctFree, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctUsed, SQLT_UIN, &rTable_->nPctUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iIniTrans, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxTrans, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogging, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctIncrease, SQLT_UIN, &rTable_->nPctIncreaseInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelists, SQLT_UIN, &rTable_->nFreelistsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelistGroups, SQLT_UIN, &rTable_->nFreelistGroupsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBufferPool, SQLT_STR, &rTable_->nBufferPoolInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDegree, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInstances, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCache, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRowIds, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPrimaryKey, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectId, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szFilterColumns, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSequence, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIncludeNewValues, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllMviewLogs::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllMviewLogs::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllMviewLogs::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllMviewLogs::szAllMviewLogs[] = {
"select tb.owner "
", sl.master "
", sl.log_table "
", tb.pct_free "
", tb.pct_used "
", tb.ini_trans "
", tb.max_trans "
", tb.logging "
", tb.pct_increase "
", tb.freelists "
", tb.freelist_groups "
", tb.buffer_pool "
", tb.tablespace_name "
", ltrim(tb.degree) as degree "
", ltrim(tb.instances) as instances "
", ltrim(tb.cache) "
", sl.rowids "
", sl.primary_key "
", sl.object_id "
", sl.filter_columns "
", sl.sequence "
", sl.include_new_values "
"from ( "
	"select distinct log_owner "
	", master "
	", log_table "
	", rowids "
	", primary_key "
	", %s as object_id "
	", filter_columns "
	", %s as sequence "
	", %s as include_new_values "
	"from all_mview_logs "
	"where log_owner in %s "
") sl "
", all_tables tb "
"where tb.owner = sl.log_owner "
"and tb.table_name = sl.log_table "
};

cAllMviewLogs::cAllMviewLogs(
    cUserSegments& oUserSegments
    , cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
    , cAllTabComments& oAllTabComments
    , cAllColComments& oAllColComments
    , std::shared_ptr<cDbaMviewLogFilterCols>& oDbaMviewLogFilterCols
)
    : oUserSegments_(oUserSegments)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
    , oAllTabComments_(oAllTabComments)
    , oAllColComments_(oAllColComments)
    , oDbaMviewLogFilterCols_(oDbaMviewLogFilterCols)
{
    // nothing to do
}

cAllMviewLogs::~cAllMviewLogs()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllMviewLogs::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllMviewLogs::vPrintAllItems(
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
        , &cAllMviewLogs::tAttributes::iKeyMatched
        , &cAllMviewLogs::tAttributes::vPrinter
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
