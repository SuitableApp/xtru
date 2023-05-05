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

#define DEL_RULE_LEN         (9+1)
#define DEFERRABLE_LEN      (14+1)
#define DEFERRERD_LEN        (9+1)
#define VALIDATED_LEN       (13+1)
#define GENETATED_LEN       (14+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cConsOfFKey::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cConsOfFKey::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner
    char szTableName[OBJECT_NAME_LEN];     ///< Name of object
    char szConstraintName[OBJECT_NAME_LEN];///< Name of the constraint definition
    char szRTableName[OBJECT_NAME_LEN];    ///< Name associated with the table (or view) with the constraint definition
    char szRConstraintName[OBJECT_NAME_LEN];///< Name of the unique constraint definition for the referenced table
    /**
     * [references_clause]
     * (https://docs.oracle.com/database/121/SQLRF/clauses002.htm#i1002156)
     * - CASCADE, the delete operation is propagated to the dependent table (and
     *   that table's dependents, if applicable).
     * - SET NULL, each nullable column of the dependent table's foreign key is set to null.
     *   (Again, if the dependent table also has dependent tables, nullable columns in those tables'
     *   foreign keys are also set to null.)
     * - NO ACTION, Derby checks the dependent tables for foreign key constraints after all deletes
     *   have been executed but before triggers have been executed. If any row in a dependent table
     *   violates a foreign key constraint, the statement is rejected.
     */
    char szDeleteRule[DEL_RULE_LEN];       ///< Delete rule for a referential constraint:
    char szDeferrable[DEFERRABLE_LEN];     ///< @copydoc cConsOfCheck::tAttributes::szDeferrable
    char szDeferred[DEFERRERD_LEN];        ///< @copydoc cConsOfCheck::tAttributes::szDeferred
    char szValidated[VALIDATED_LEN];       ///< @copydoc cConsOfCheck::tAttributes::szValidated
    char szGenerated[GENETATED_LEN];       ///< @copydoc cConsOfCheck::tAttributes::szGenerated
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szRTableName, 0, sizeof(szRTableName));
        ::memset(szRConstraintName, 0, sizeof(szRConstraintName));
        ::memset(szDeleteRule, 0, sizeof(szDeleteRule));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
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
            , szTableName, rhs.szTableName
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cConsOfFKey& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cConsOfFKey::tAttributes::vPrinter(std::ostream& os, const size_t& i, cConsOfFKey& oCtx) const
{
    using ps::lib::iCompBinary;
    using ps::lib::sMakeEnclosedName;
    os << boost::format("%s constraint %s foreign key ")
            % (i == 0 && oCtx.iInvCtx_ == iInAltTbl ? "" : ",")
            % sMakeEnclosedName(szConstraintName, MINIMUM_COLUMN_LENGTH)
        << std::endl
        << oPrintItems(oCtx.oAllConsColumnsKey_, std::make_tuple(szOwner, szTableName, szConstraintName), true)
        << boost::format("  ) references %s")
            % sMakeEnclosedName(szRTableName)
        << std::endl
        << oPrintItems(oCtx.oAllConsColumnsRef_, std::make_tuple(szOwner, szRTableName, szRConstraintName), true)
        ;
    const std::string sOnDeleteRuleClause =
        iCompBinary(strcmp, szDeleteRule, "NO ACTION")
            ? ""
            : "on delete " + boost::algorithm::to_lower_copy(std::string(szDeleteRule)) + " "
        ;
    os << boost::format("  ) %s%s %s disable %s")
        % sOnDeleteRuleClause
        % (iCompBinary(strcmp, szDeferrable, "NOT DEFERRABLE") ? "not deferrable" : "deferrable")
        % (iCompBinary(strcmp, szDeferred, "IMMEDIATE") ? "initially immediate" : "initially deferred")
        % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
        << std::endl
        ;
}

std::string cConsOfFKey::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szConstraintName='%s', szRTableName='%s')"
        R"(, szRConstraintName='%s', szDeleteRule='%s', szDeleteRule='%s')"
        R"(, szDeferrable='%s', szDeferred='%s', szValidated='%s', szGenerated='%s')"
    ) % szOwner % szTableName % szConstraintName % szRTableName
        % szRConstraintName % szDeleteRule % szDeleteRule
        % szDeferrable % szDeferred % szValidated % szGenerated
    ;
    return oss.str();
}

/**
 * @struct cConsOfFKey::cRetriever
 */
class cConsOfFKey::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cConsOfFKey::oList_.
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

const uint32_t cConsOfFKey::cRetriever::iBulkSize_ = 100;

cConsOfFKey::tAttributes cConsOfFKey::cRetriever::rTable_[cConsOfFKey::cRetriever::iBulkSize_];

cConsOfFKey::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szConsOfFKey
        , "Reading referencial constraints", &oBind_
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
    oDefine_.vAddItem(rTable_->szRTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeleteRule, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cConsOfFKey::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cConsOfFKey::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cConsOfFKey::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cConsOfFKey::szConsOfFKey[] = {
"select t1.owner "
", t1.table_name "
", t1.constraint_name "
", t2.table_name as r_table_name "
", t1.r_constraint_name "
", t1.delete_rule "
", t1.deferrable "
", t1.deferred "
", t1.validated "
", t1.generated "
"from all_constraints t1 "
", all_constraints t2 "
"where t1.owner in %s "
"and t1.constraint_type = 'R' "
"and t2.owner = t1.owner "
"and t2.constraint_name = t1.r_constraint_name "
"and t2.constraint_type in ('P', 'U') "
};

cConsOfFKey::cConsOfFKey(
    cAllConsColumns& oAllConsColumnsKey
    , cAllConsColumns& oAllConsColumnsRef
)
    : oAllConsColumnsKey_(oAllConsColumnsKey)
    , oAllConsColumnsRef_(oAllConsColumnsRef)
{
    // nothing to do
}

cConsOfFKey::~cConsOfFKey()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cConsOfFKey::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cConsOfFKey::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const tInvCtx& iInvCtx
){
    iInvCtx_ = iInvCtx;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cConsOfFKey::tAttributes::iKeyMatched
        , &cConsOfFKey::tAttributes::vPrinter
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
