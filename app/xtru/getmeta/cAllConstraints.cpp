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
constexpr size_t DEL_RULE_LEN = 9+1;
constexpr size_t STATUS_LEN = 8+1;
constexpr size_t DEFERRABLE_LEN = 14+1;
constexpr size_t DEFERRERD_LEN = 9+1;
constexpr size_t VALIDATED_LEN = 13+1;
constexpr size_t GENETATED_LEN = 14+1;
namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllConstraints::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllConstraints::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];            ///< Name of object owner.
	char szConstraintName[OBJECT_NAME_LEN];   ///< Name of constraint.
	char szConstraintType[CONSTR_TYPE_LEN];   ///< Name of table column.
	char szTableName[OBJECT_NAME_LEN];        ///< Name of table column.
    /**
     * Text of search condition for a check constraint. This column returns
     *  the correct value onlywhen the row originates from the current container.
     */
	char *szSearchCondition;                  ///< Content of the check constraint.
	ps::lib::sql::ind_t nSearchConditionInd;  ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iSearchCondition;                     ///< length of the expression body.
    /**
     * Reference to row type 'P' or 'U', when this row type is 'R'.
     */
	char szROwner[OBJECT_NAME_LEN];           ///< Name of owner when this record is FK,
    ps::lib::sql::ind_t nROwnerInd;           ///< ind_t::VAL_IS_NULL: value is NULL
    /**
     * Reference to row type 'P' or 'U', when this row type is 'R'.
     */
	char szRConstraintName[OBJECT_NAME_LEN];  ///< Name of constraint when this record is FK,
    ps::lib::sql::ind_t nRConstraintNameInd;  ///< ind_t::VAL_IS_NULL: value is NULL
    /**
     * See [references_clause](https://docs.oracle.com/database/121/SQLRF/clauses002.htm#i1002156)
     * to study how Oracle Database automatically maintains referential integrity.
     */
	char szDeleteRule[DEL_RULE_LEN];          ///< Instruction to keep referential integrity.
    ps::lib::sql::ind_t nDeleteRuleInd;       ///< ind_t::VAL_IS_NULL: value is NULL
    /**
     * - ENABLED
     * - DISABLED
     */
	char szStatus[STATUS_LEN];                ///< Enforcement status of the constraint:
    /**
     * - 'DEFERRABLE'
     * - 'NOT DEFERRABLE'
     */
	char szDeferrable[DEFERRABLE_LEN];        ///< Indicates whether the constraint is deferrable or not
    /**
     * - 'DEFERRED'
     * - 'IMMEDIATE'
     */
	char szDeferred[DEFERRERD_LEN];           ///< Indicates whether the constraint was initially deferred or not
    /**
     * - When tAttributes::szStatus = 'ENABLED', possible values are:
     *   - 'VALIDATED' : All data obeys the constraint (that is, the existing data
     *                 in the table was validated when the constraint was enabled,
     *                 as well as any subsequent data entered into the table)
     *   - 'NOT VALIDATED' : All data may not obey the constraint (that is,
     *                 the existing data in the table was not validated when the constraint
     *                 was enabled, but subsequent data entered into the table was validated)
     * - When tAttributes::szStatus = 'DISABLED', possible values are:
     *   - 'VALIDATED' : All data obeys the constraint, but the unique index on the constraint
     *                 has been dropped. This setting is useful in data warehousing environments,
     *                 but has some restrictions. Refer to Oracle Database Data Warehousing Guide
     *                 for more information on this setting.
     *   - 'NOT VALIDATED' : All data may not obey the constraint
     */
	char szValidated[VALIDATED_LEN];          ///< The meaning depends on the value of tAttributes::szStatus.
    /**
     * - 'USER NAME'
     * - 'GENERATED NAME'
     */
	char szGenerated[GENETATED_LEN];          ///< Indicates whether the name of the constraint is user-generated or system-generated
	char szIndexOwner[OBJECT_NAME_LEN];       ///< Name of the user owning the index.
    ps::lib::sql::ind_t nIndexOwnerInd;       ///< ind_t::VAL_IS_NULL: value is NULL
	char szIndexName[OBJECT_NAME_LEN];        ///< Name of the index (only shown for unique and primary-key constraints).
    ps::lib::sql::ind_t nIndexNameInd;        ///< ind_t::VAL_IS_NULL: value is NULL
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szSearchCondition(nullptr)
        , nSearchConditionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iSearchCondition(0)
        , nROwnerInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nRConstraintNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nDeleteRuleInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIndexOwnerInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nIndexNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szConstraintType, 0, sizeof(szConstraintType));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szROwner, 0, sizeof(szROwner));
        ::memset(szRConstraintName, 0, sizeof(szRConstraintName));
        ::memset(szDeleteRule, 0, sizeof(szDeleteRule));
        ::memset(szStatus, 0, sizeof(szStatus));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szIndexOwner, 0, sizeof(szIndexOwner));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szConstraintName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szSearchCondition which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szSearchCondition(new char[rhs.iSearchCondition + 1])
        , nSearchConditionInd(rhs.nSearchConditionInd)
        , iSearchCondition(rhs.iSearchCondition)
        , nROwnerInd(rhs.nROwnerInd)
        , nRConstraintNameInd(rhs.nRConstraintNameInd)
        , nDeleteRuleInd(rhs.nDeleteRuleInd)
        , nIndexOwnerInd(rhs.nIndexOwnerInd)
        , nIndexNameInd(rhs.nIndexNameInd)
    {
        // clean up.
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szConstraintType, 0, sizeof(szConstraintType));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szROwner, 0, sizeof(szROwner));
        ::memset(szRConstraintName, 0, sizeof(szRConstraintName));
        ::memset(szDeleteRule, 0, sizeof(szDeleteRule));
        ::memset(szStatus, 0, sizeof(szStatus));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szIndexOwner, 0, sizeof(szIndexOwner));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        // copy the data.
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szConstraintName, rhs.szConstraintName);
        ::strcpy(szConstraintType, rhs.szConstraintType);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szSearchCondition, rhs.szSearchCondition);
        ::strcpy(szROwner, rhs.szROwner);
        ::strcpy(szRConstraintName, rhs.szRConstraintName);
        ::strcpy(szDeleteRule, rhs.szDeleteRule);
        ::strcpy(szStatus, rhs.szStatus);
        ::strcpy(szDeferrable, rhs.szDeferrable);
        ::strcpy(szDeferred, rhs.szDeferred);
        ::strcpy(szValidated, rhs.szValidated);
        ::strcpy(szGenerated, rhs.szGenerated);
        ::strcpy(szIndexOwner, rhs.szIndexOwner);
        ::strcpy(szIndexName, rhs.szIndexName);
    }
    ~tAttributes()
    {
        delete [] szSearchCondition;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szConstraintType, rhs.szConstraintType
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szConstraintType, rhs.szConstraintType
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllConstraints& oCtx) const
    {
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllConstraints::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szConstraintName='%s', szConstraintType='%s', szTableName='%s')"
        R"(, szSearchCondition='%s', szROwner='%s', szRConstraintName='%s')"
        R"(, szDeleteRule='%s', szStatus='%s', szDeferrable='%s', szDeferred='%s')"
        R"(, szValidated='%s', szGenerated='%s', szIndexOwner='%s', szIndexName='%s')"
    ) % szOwner % szConstraintName % szConstraintType % szTableName
        % szSearchCondition % szROwner % szRConstraintName
        % szDeleteRule % szStatus % szDeferrable % szDeferred
        % szValidated % szGenerated % szIndexOwner % szIndexName
    ;
    return oss.str();
}

/**
 * @struct cAllConstraints::cRetriever
 */
class cAllConstraints::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllConstraints::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::oBind_
    /// @copybrief cAllSequences::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::iSkip_
    const size_t iSkip_;
    /**
     *  @brief
     */
    ps::lib::sql::occi::cPieceVct pv_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
    );
private:
    /// @copybrief cAllSequences::vPreBulkAction()
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /// @copybrief cAllSequences::vPostBulkAction()
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /// @copybrief cAllSequences::vPostRepeatAction()
    virtual void vPostRepeatAction();
};

const uint32_t cAllConstraints::cRetriever::iBulkSize_ = 100;

cAllConstraints::tAttributes cAllConstraints::cRetriever::rTable_[cAllConstraints::cRetriever::iBulkSize_];

cAllConstraints::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllConstraints
        , "Reading constraints", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szSearchCondition, &rTable_->nSearchConditionInd, &rTable_->iSearchCondition)
{
    const auto later_9iR1 = oSvc.iCompDbVersion("0900",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ later_9iR1 ? szAplddT03_r9 : szAplddT03_r8, sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintType, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
    oDefine_.vAddItem(rTable_->szROwner, SQLT_STR, &rTable_->nROwnerInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szRConstraintName, SQLT_STR, &rTable_->nRConstraintNameInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDeleteRule, SQLT_STR, &rTable_->nDeleteRuleInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szStatus, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexOwner, SQLT_STR, &rTable_->nIndexOwnerInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, &rTable_->nIndexNameInd, nullptr, nullptr, iSkip_);
}

void cAllConstraints::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllConstraints::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllConstraints::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllConstraints::szAllConstraints[] = {
"select t2.owner, t2.constraint_name, t2.constraint_type, t2.table_name "
", t2.search_condition "
", t2.r_owner, t2.r_constraint_name, t2.delete_rule, t2.status "
", t2.deferrable, t2.deferred, t2.validated, t2.generated "
"%s "
"from all_constraints t2  "
"where t2.owner in %s "
"and t2.table_name not like 'BIN$%%' "
};

const char cAllConstraints::szAplddT03_r9[] = {
/*
 * user_constraints.index_owner, dba_constraints.index_owner returns
 * the same value as owner. However it is null in all_constraints.index_owner.
 * It is not described in the product manual as the specification of
 * only all_constraints. bug?
*/
", nvl(t2.index_owner, t2.owner), t2.index_name "
};

const char cAllConstraints::szAplddT03_r8[] = {
", t2.owner as index_owner, t2.constraint_name as index_name "
};

cAllConstraints::cAllConstraints()
{
    // nothing to do
}

cAllConstraints::~cAllConstraints()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllConstraints::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllConstraints::vPrintAllItems(
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
        , &cAllConstraints::tAttributes::iKeyMatched
        , &cAllConstraints::tAttributes::vPrinter
        , &cAllConstraints::vPrintPreRows
        , &cAllConstraints::vPrintPostRows
        , &cAllConstraints::vPrintNotFound
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

void cAllConstraints::vPrintPreRows(std::ostream&, const tAttributes&) const
{
    // nothing to do
}

void cAllConstraints::vPrintPostRows(std::ostream&, const tAttributes&) const
{
    // nothing to do
}

void cAllConstraints::vPrintNotFound(std::ostream&, const tAttributes&) const
{
    // nothing to do
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
