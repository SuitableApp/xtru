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

#define UNIQUENESS_LEN          (10+1)
#define INDEX_TYPE_LEN          (27+1)
#define LOGGING_TYPE_LEN         (7+1)
#define PARTITIONED_LEN          (3+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cUserIndexes::tAttributes
 * See to study about [ALL_INDEXES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-E39825BA-70AC-45D8-AF30-C7FF561373B6.htm#REFRN20088)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cUserIndexes::tAttributes
{
    /// @enum tSelFlg
    typedef enum {
        iSelPctfree = 0x1
        , iSelInitrans = 0x2
        , iSelMaxrans = 0x4
        , iSelLfSpc1 = 0x8
        , iSelLogging = 0x10
        , iSelParallel = 0x20
        , iSelAll =
            iSelPctfree | iSelInitrans | iSelMaxrans | iSelLfSpc1
            | iSelLogging | iSelParallel
        , iSelUsingIndex = iSelLogging
    } tSelFlg
    ;
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the index
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the indexed object
	char szIndexName[OBJECT_NAME_LEN];     ///< Name of the index
	char szUniqueness[UNIQUENESS_LEN];     ///< Indicates whether the index is unique (UNIQUE) or nonunique (NONUNIQUE)
    ps::lib::sql::ind_t nUniquenessInd;    ///< ind_t::VAL_IS_NULL: value is NULL
    /// - LOB
    /// - NORMAL
    /// - NORMAL/REV
    /// - BITMAP
    /// - FUNCTION-BASED NORMAL
    /// - FUNCTION-BASED NORMAL/REV
    /// - FUNCTION-BASED BITMAP
    /// - FUNCTION-BASED DOMAIN
    /// - CLUSTER
    /// - IOT - TOP
    /// - DOMAIN
	char szIndexType[INDEX_TYPE_LEN];      ///< Type of the index:
	ps::lib::sql::ind_t nIndexTypeInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iPctThreshold;                     ///< Threshold percentage of block space allowed per index entry
	ps::lib::sql::ind_t nPctThresholdInd;  ///< ind_t::VAL_IS_NULL: value is NULL
    /// - to be included in index-organized table primary key (non-overflow) index.
    ///   This column maps to the COLUMN_ID column of the *_TAB_COLUMNS view.
	ub4 iIncludeColumn;                    ///< Column ID of the last column
	ps::lib::sql::ind_t nIncludeColumnInd; ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iPctFree;                          ///< Minimum percentage of free space in a block
	ps::lib::sql::ind_t nPctFreeInd;       ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iIniTrans;                         ///< Initial number of transactions
	ps::lib::sql::ind_t nIniTransInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iMaxTrans;                         ///< Maximum number of transactions
	ps::lib::sql::ind_t nMaxTransInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	char szLogging[LOGGING_TYPE_LEN];      ///< Indicates whether or not changes to the index are logged YES or NO.
	ps::lib::sql::ind_t nLoggingInd;       ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iPctIncrease;                      ///< Percentage increase in extent size
	ps::lib::sql::ind_t nPctIncreaseInd;   ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iFreelists;                        ///< Number of process freelists allocated to this segment
	ps::lib::sql::ind_t nFreelistsInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iFreelistGroups;                   ///< Number of freelist groups allocated to this segment
	ps::lib::sql::ind_t nFreelistGroupsInd;///< ind_t::VAL_IS_NULL: value is NULL
    /// - DEFAULT
    /// - KEEP
    /// - RECYCLE
	char szBufferPool[BUFFER_POOL_LEN];    ///< Buffer pool to be used for index blocks:
	ps::lib::sql::ind_t nBufferPoolInd;    ///< ind_t::VAL_IS_NULL: value is NULL
	char szTablespaceName[OBJECT_NAME_LEN];///< Name of the tablespace containing the index
	ps::lib::sql::ind_t nTablespaceNameInd;///< ind_t::VAL_IS_NULL: value is NULL
	char szDegree[DEGREE_LEN];             ///< Number of threads per instance for scanning the index, or DEFAULT
	char szInstances[INSTANCES_LEN];       ///< Number of instances across which the indexes to be scanned, or DEFAULT
	char szPartitioned[PARTITIONED_LEN];   ///< Indicates whether the index is partitioned (YES) or not (NO)
	char szConstraintName[OBJECT_NAME_LEN];///< associated with ALL_CONSTRAINTS.CONSTRAINT_NAME
	ps::lib::sql::ind_t nConstraintNameInd;///< ind_t::VAL_IS_NULL: value is NULL
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nUniquenessInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIndexTypeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctThreshold(0)
        , nPctThresholdInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iIncludeColumn(0)
        , nIncludeColumnInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctFree(0)
        , nPctFreeInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iIniTrans(0)
        , nIniTransInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iMaxTrans(0)
        , nMaxTransInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nLoggingInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iPctIncrease(0)
        , nPctIncreaseInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelists(0)
        , nFreelistsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iFreelistGroups(0)
        , nFreelistGroupsInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nBufferPoolInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nTablespaceNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nConstraintNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::memset(szUniqueness, 0, sizeof(szUniqueness));
        ::memset(szIndexType, 0, sizeof(szIndexType));
        ::memset(szLogging, 0, sizeof(szLogging));
        ::memset(szBufferPool, 0, sizeof(szBufferPool));
        ::memset(szTablespaceName, 0, sizeof(szTablespaceName));
        ::memset(szDegree, 0, sizeof(szDegree));
        ::memset(szInstances, 0, sizeof(szInstances));
        ::memset(szPartitioned, 0, sizeof(szPartitioned));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szIndexName, std::get<2>(oKey));
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
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iFullMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iTableMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCreIotMatched(const tAttributes& rhs) const
    {
        return iTableMatched(rhs)
            && ps::lib::iCompBinary(strcmp, szIndexType, "IOT - TOP");
        ;
    }
    /** @brief
     * - Specify condition for selecting the index to be dropped
     *   before executing SQL*Loader
     * - Select unique index that is not an index-organized table PK
     *   and that is used as PK of regular table
     */
    bool iCompToDrpIdxScr(const tAttributes& rhs) const
    {
        return iTableMatched(rhs)
            && ! ps::lib::iCompBinary(strcmp, szIndexType, "IOT - TOP")
            && ! (
                // Index for enforcing Primary key or Unique key constraint.
                ps::lib::iCompBinary(strcmp, szUniqueness, "UNIQUE")
                && nConstraintNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
            )
        ;
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iCompToCreIdxScr(const tAttributes& rhs) const
    {
        return iTableMatched(rhs)
            && ! ps::lib::iCompBinary(strcmp, szIndexType, "IOT - TOP")
        ;
    }
    /// Block area usage parameter.
    void vPrtIdxBlkParam(std::ostream& os, const tSelFlg iSelFlg) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vDescIot(std::ostream& os, const size_t& i, cUserIndexes& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserIndexes& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vUsingIndexClauseAction(std::ostream& os, const size_t& i, cUserIndexes& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vDropIndex(std::ostream& os, const size_t& i, cUserIndexes& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cUserIndexes::tAttributes::vPrtIdxBlkParam(std::ostream& os, const tSelFlg iSelFlg) const
{
	if (iSelPctfree & iSelFlg)
    {
        os << boost::format("pctfree %u ") % iPctFree;
	}
	if (iSelInitrans & iSelFlg)
    {
        os << boost::format("initrans %u ") % iIniTrans;
	}
	if (iSelMaxrans & iSelFlg)
    {
        os << boost::format("maxtrans %u ") % iMaxTrans;
	}
	// There are two ways NO and NONE except for YES
	if (iSelLogging & iSelFlg)
    {
        const bool iLoging = ps::lib::iCompBinary(strcmp, szLogging, "NO");
        os << std::string(iLoging ? "nologging " : "");
	}
	if (iSelParallel & iSelFlg)
    {
        os << boost::format("parallel (degree %s instances %s) ")
            % szDegree % szInstances;
	}
    os << std::endl;
}

void cUserIndexes::tAttributes::vDescIot(std::ostream& os, const size_t&, cUserIndexes& oCtx) const
{
    os << boost::format("organization index pctthreshold %u") % iPctThreshold
        << std::endl << std::string(2, ' ')
        ;
    vPrtIdxBlkParam(os, iSelAll);
    // Output OVERFLOW segment attribute if present
    oCtx.oAllTables_.vPrintIotOvfItems(
        os, std::make_tuple(szOwner, szTableName)
    );
    os << std::endl;
    // Last nonkey column ID to include in the index
    // (Valid only when OVERFLOW clause is specified)
    if (
        nIncludeColumnInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL
		&& iIncludeColumn > 0
    ){
        os << boost::format("  including %s") %
            ps::lib::sMakeEnclosedName(
                oCtx.oAllTabColumns_.szGetColumnName(szOwner, szTableName, iIncludeColumn)
            )
            << std::endl;
    }
}

void cUserIndexes::tAttributes::vPrinter(std::ostream& os, const size_t&, cUserIndexes& oCtx) const
{
    using ps::lib::iCompBinary;
    using ps::lib::iCascadeTernaryOr;
    using ps::lib::iMatchedBackward;
    std::string sIndexType;
    if (
        iCompBinary(strcmp, szUniqueness, "UNIQUE")
        && iCompBinary(strcmp, szIndexType, "NORMAL")
    ){
        sIndexType = std::string(szUniqueness) + " ";
    }
    if (
        iCompBinary(strcmp, szUniqueness, "NONUNIQUE")
        && iCompBinary(strcmp, szIndexType, "BITMAP")
    ){
        sIndexType = std::string(szIndexType) + " ";
    }
    os << boost::format("create %sindex %s on %s%s")
        % sIndexType
        % ps::lib::sMakeEnclosedName(szIndexName)
        % (iCompBinary(strcmp, szIndexType, "CLUSTER") ? "cluster " : "")
        % ps::lib::sMakeEnclosedName(szTableName)
        << std::endl
    ;
    if (
        iCascadeTernaryOr(strncmp, szIndexType, "NORMAL", "FUNCTION-BASED")
        || iCompBinary(strcmp, szIndexType, "BITMAP")
    ){
        os << oPrintItems(
            oCtx.oAllIndColumns_, std::make_tuple(szOwner, szTableName, szIndexName), true
        );
    }
    else if (iCompBinary(strcmp, szIndexType, "CLUSTER"))
    {
        os << std::string(2, ' ');
    }
    else
    {
        RAISE_EX_CONVERT(
            std::runtime_error
            , boost::format("Un-supported index type %s used in %s.%s.%s")
            % ps::lib::sMakeEnclosedName(szIndexType)
            % ps::lib::sMakeEnclosedName(szOwner)
            % ps::lib::sMakeEnclosedName(szTableName)
            % ps::lib::sMakeEnclosedName(szIndexName)
        );
    }
    if (iMatchedBackward(szIndexType, "/REV"))
    {
        os << std::string("reverse ");
    }
    vPrtIdxBlkParam(os, iSelAll);
    if (iCompBinary(strcmp, szPartitioned, "NO")) // Regular index
    {
        os << std::string("  storage (")
           << oPrintItems(
            oCtx.oUserSegments_, std::make_tuple(szOwner, szIndexName, "INDEX", ""), true
        );
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
        os << boost::format("buffer_pool %s) ") % szBufferPool;
        if (nTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
        }
        os << std::endl;
    }
    else // Partitiond index
    {
		// Representation of local_partitioned_index
        //                or global_partitioned_inde.
        os << oPrintItems(
            oCtx.oAllPartIndexes_, std::make_tuple(szOwner, szTableName, szIndexName), true
        );
    }
    os << std::string(";") << std::endl;
}

void cUserIndexes::tAttributes::vUsingIndexClauseAction(std::ostream& os, const size_t&, cUserIndexes& oCtx) const
{
    os << boost::format("using index /* %s */ ") % szIndexName;
    vPrtIdxBlkParam(os, iSelUsingIndex);
    if (ps::lib::iCompBinary(strcmp, szPartitioned, "NO")) // Regular index
    {
        os << std::string("  storage (")
           << oPrintItems(
            oCtx.oUserSegments_, std::make_tuple(szOwner, szIndexName, "INDEX", ""), true
        );
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
        os << boost::format("buffer_pool %s) ") % szBufferPool;
        if (nTablespaceNameInd == ps::lib::sql::ind_t::VAL_IS_NOTNULL)
        {
            os << boost::format("tablespace %s ") % ps::lib::sMakeEnclosedName(szTablespaceName);
        }
    }
    else // Partitiond index
    {
		// Representation of local_partitioned_index
        //                or global_partitioned_inde.
        os << oPrintItems(
            oCtx.oAllPartIndexes_, std::make_tuple(szOwner, szTableName, szIndexName), true
        );
    }
}

void cUserIndexes::tAttributes::vDropIndex(std::ostream& os, const size_t&, cUserIndexes& oCtx) const
{
    os << boost::format("drop index %s;")
        % ps::lib::sMakeEnclosedName(szIndexName)
        << std::endl;
}

std::string cUserIndexes::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szIndexName='%s', szUniqueness='%s'(%d))"
        R"(, szIndexType='%s'(%d), iPctThreshold=%u(%d), iIncludeColumn=%u(%d))"
        R"(, iPctFree=%u(%d), iIniTrans=%u(%d), iMaxTrans=%u(%d), szLogging='%s'(%d))"
        R"(, iPctIncrease=%u(%d), iFreelists=%ui(%d), iFreelistGroups=%u(%d))"
        R"(, szBufferPool='%s'(%d), szTablespaceName='%s'(%d))"
        R"(, szDegree='%s', szInstances='%s', szPartitioned='%s', szConstraintName='%s'(%d))"
    ) % szOwner % szTableName % szIndexName % szUniqueness % nUniquenessInd
        % szIndexType % nIndexTypeInd % iPctThreshold % nPctThresholdInd % iIncludeColumn % nIncludeColumnInd
        % iPctFree % nPctFreeInd % iIniTrans % nIniTransInd % iMaxTrans % nMaxTransInd % szLogging % nLoggingInd
        % iPctIncrease % nPctIncreaseInd % iFreelists % nFreelistsInd % iFreelistGroups % nFreelistGroupsInd
        % szBufferPool % nBufferPoolInd % szTablespaceName % nTablespaceNameInd
        % szDegree % szInstances % szPartitioned % szConstraintName % nConstraintNameInd
    ;
    return oss.str();
}

/**
 * @struct cUserIndexes::cRetriever
 */
class cUserIndexes::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserIndexes::oList_.
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
        return oPrivsGiven.iAccesible({"SYS", "DBA_INDEXES", "SELECT"});
    }
};

const uint32_t cUserIndexes::cRetriever::iBulkSize_ = 100;

cUserIndexes::tAttributes cUserIndexes::cRetriever::rTable_[cUserIndexes::cRetriever::iBulkSize_];

cUserIndexes::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szUserIndexes
        , "Reading attribute of indexes", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
{
    const bool iGrantedPrivs = iAccesible();
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        (iGrantedPrivs ? "t1.owner" : "USER")
        , (iGrantedPrivs ? "dba_indexes" : "user_indexes")
        , sGetSqlInList(oOwners)
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUniqueness, SQLT_STR, &rTable_->nUniquenessInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexType, SQLT_STR, &rTable_->nIndexTypeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctThreshold, SQLT_UIN, &rTable_->nPctThresholdInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iIncludeColumn, SQLT_UIN, &rTable_->nIncludeColumnInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctFree, SQLT_UIN, &rTable_->nPctFreeInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iIniTrans, SQLT_UIN, &rTable_->nIniTransInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iMaxTrans, SQLT_UIN, &rTable_->nMaxTransInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLogging, SQLT_STR, &rTable_->nLoggingInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iPctIncrease, SQLT_UIN, &rTable_->nPctIncreaseInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelists, SQLT_UIN, &rTable_->nFreelistsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iFreelistGroups, SQLT_UIN, &rTable_->nFreelistGroupsInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBufferPool, SQLT_STR, &rTable_->nBufferPoolInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTablespaceName, SQLT_STR, &rTable_->nTablespaceNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDegree, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInstances, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPartitioned, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, &rTable_->nConstraintNameInd, NULL, NULL, iSkip_);
}

void cUserIndexes::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserIndexes::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserIndexes::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cUserIndexes::szUserIndexes[] = {
"select %s "
", t1.table_name "
", t1.index_name "
", t1.uniqueness "
", t1.index_type "
", t1.pct_threshold "
", t1.include_column "
", decode(t1.partitioned, 'NO', t1.pct_free, t3.def_pct_free) as pct_free "
", decode(t1.partitioned, 'NO', t1.ini_trans, t3.def_ini_trans) as ini_trans "
", decode(t1.partitioned, 'NO', t1.max_trans, t3.def_max_trans) as max_trans "
", decode(t1.partitioned, 'NO', t1.logging, t3.def_logging) as logging "
", t1.pct_increase "
", t1.freelists "
", t1.freelist_groups "
", t1.buffer_pool "
", t1.tablespace_name "
", t1.degree "
", t1.instances "
", t1.partitioned "
", t4.constraint_name "
"from %s t1 "
", all_part_indexes t3 "
", all_constraints t4 "
"where t1.table_owner in %s "
"and ("
	"(t1.index_type like 'FUNCTION-BASED%') "
	"or (t1.index_type in ("
		"'CLUSTER'"
		",'IOT - TOP'"
		",'NORMAL'"
		",'NORMAL/REV'"
		",'BITMAP'"
	")) "
") "
"and t3.owner (+) = t1.table_owner "
"and t3.index_name (+) = t1.index_name "
"and t4.index_owner (+) = t1.table_owner "
"and t4.table_name (+) = t1.table_name "
"and t4.index_name (+) = t1.index_name "
};

cUserIndexes::cUserIndexes(
    cAllIndColumns& oAllIndColumns
    , cUserSegments& oUserSegments
    , cAllTables& oAllTables
    , cAllTabColumns& oAllTabColumns
    , cAllPartIndexes& oAllPartIndexes
)
    : oAllIndColumns_(oAllIndColumns)
    , oUserSegments_(oUserSegments)
    , oAllTables_(oAllTables)
    , oAllTabColumns_(oAllTabColumns)
    , oAllPartIndexes_(oAllPartIndexes)
{
    // nothing to do
}

cUserIndexes::~cUserIndexes()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserIndexes::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cUserIndexes::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const bool& iIot
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , iIot
            ? &cUserIndexes::tAttributes::iCreIotMatched
            : &cUserIndexes::tAttributes::iTableMatched
        , iIot
            ? &cUserIndexes::tAttributes::vDescIot
            : &cUserIndexes::tAttributes::vPrinter
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

void cUserIndexes::vRptUsingIndexClause(
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
        , &cUserIndexes::tAttributes::iFullMatched
        , &cUserIndexes::tAttributes::vUsingIndexClauseAction
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

void cUserIndexes::vRptDrpCreUserIndexes(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const tWayToNext iWayToNext
){
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , iWayToNext == iForward
            ? &cUserIndexes::tAttributes::iCompToDrpIdxScr
            : &cUserIndexes::tAttributes::iCompToCreIdxScr
        , iWayToNext == iForward
            ? &cUserIndexes::tAttributes::vDropIndex
            : &cUserIndexes::tAttributes::vPrinter
        , &cUserIndexes::vOnPreDropCreIndex
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

size_t cUserIndexes::iSubtractCtxItemList(
    const tKeyTuple& oKey
){
    return iEraseMatchedKeyItem(
        tAttributes(oKey)
        , oList_
        , &cUserIndexes::tAttributes::iFullMatched
    );
}

void cUserIndexes::vOnPreDropCreIndex(std::ostream& os, const tAttributes&) const
{
    os << std::string("-- Indexes") << std::endl;
}

void cUserIndexes::vReverse()
{
    oList_.reverse();
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
