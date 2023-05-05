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

#define KEY_TYPE_LEN        (12+1)
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
 * @struct cConsOfAKey::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cConsOfAKey::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner
    char szTableName[OBJECT_NAME_LEN];     ///< Name of object
    ub4 iOrdering;                         ///< 1:PK, 2:UK
    char szConstraintName[OBJECT_NAME_LEN];///< Name of the constraint definition
    char szUniqueKeyType[OBJECT_NAME_LEN]; ///< Name associated with the table (or view) with the constraint definition
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
    char szDeferrable[DEFERRABLE_LEN];     ///< @copydoc cConsOfCheck::tAttributes::szDeferrable
    char szDeferred[DEFERRERD_LEN];        ///< @copydoc cConsOfCheck::tAttributes::szDeferred
    char szValidated[VALIDATED_LEN];       ///< @copydoc cConsOfCheck::tAttributes::szValidated
    char szGenerated[GENETATED_LEN];       ///< @copydoc cConsOfCheck::tAttributes::szGenerated
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iOrdering(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szUniqueKeyType, 0, sizeof(szUniqueKeyType));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
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
        // Place the primary key constraint ahead of the unique key constraint
            , iOrdering, rhs.iOrdering
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
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iSearchConsName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cConsOfAKey& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cConsOfAKey::tAttributes::vPrinter(std::ostream& os, const size_t& i, cConsOfAKey& oCtx) const
{
    using ps::lib::iCompBinary;
    using ps::lib::sMakeEnclosedName;
    const std::string sClauseFmt = std::string(
        iCompBinary(strcmp, szGenerated, "USER NAME")
        ? "constraint %s "
        : "/* constraint %s */ "
    );
    os << boost::format("%s")
            % (i == 0 && oCtx.iConsInAny_ != iConsInTable ? " " : ", ")
        << boost::format(sClauseFmt + std::string("%s "))
            % sMakeEnclosedName(szConstraintName, MINIMUM_COLUMN_LENGTH)
            % szUniqueKeyType
        << std::endl
        << oPrintItems(oCtx.oAllConsColumnsKey_, std::make_tuple(szOwner, szTableName, szConstraintName), true)
        ;
    const std::string sStatus =
        std::string(iOrdering == 1 && oCtx.iIotType_ ? "" : "disable");

    os << boost::format("  ) %s %s %s %s")
        % (iCompBinary(strcmp, szDeferrable, "NOT DEFERRABLE") ? "not deferrable" : "deferrable")
        % (iCompBinary(strcmp, szDeferred, "IMMEDIATE") ? "initially immediate" : "initially deferred")
        % sStatus
        % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
        << std::endl
        ;
}

std::string cConsOfAKey::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', iOrdering=%d, szConstraintName='%s', szUniqueKeyType='%s')"
        R"(, szDeferrable='%s', szDeferred='%s', szValidated='%s', szGenerated='%s')"
    ) % szOwner % szTableName % iOrdering % szConstraintName % szUniqueKeyType
        % szDeferrable % szDeferred % szValidated % szGenerated
    ;
    return oss.str();
}

/**
 * @struct cConsOfAKey::cRetriever
 */
class cConsOfAKey::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cConsOfAKey::oList_.
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

const uint32_t cConsOfAKey::cRetriever::iBulkSize_ = 100;

cConsOfAKey::tAttributes cConsOfAKey::cRetriever::rTable_[cConsOfAKey::cRetriever::iBulkSize_];

cConsOfAKey::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szConsOfAKey
        , "Reading alternate key constraints", &oBind_
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
    oDefine_.vAddItem(rTable_->iOrdering, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUniqueKeyType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cConsOfAKey::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cConsOfAKey::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cConsOfAKey::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cConsOfAKey::szConsOfAKey[] = {
"select owner "
", table_name "
", decode(constraint_type,'P','1','2') "
	"as constraint_type "
", constraint_name "
", decode(constraint_type,'P','primary key','unique') "
	"as unique_key_type "
", deferrable "
", deferred "
", validated "
", generated "
"from all_constraints "
"where owner in %s "
"and constraint_type in ('P','U') "
};

cConsOfAKey::cConsOfAKey(
    cAllConsColumns& oAllConsColumnsKey
)
    : oAllConsColumnsKey_(oAllConsColumnsKey)
{
    // nothing to do
}

cConsOfAKey::~cConsOfAKey()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cConsOfAKey::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cConsOfAKey::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const bool& iIotType
    , const tConsInAny& iConsInAny
){
    iIotType_ = iIotType;
    iConsInAny_ = iConsInAny;
    ps::lib::cList<tIter> oArr;
    vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cConsOfAKey::tAttributes::iKeyMatched
        , &cConsOfAKey::tAttributes::vPrinter
        , iConsInAny == iConsInTable
            ? static_cast<tOneTimeAction>(nullptr)
            : &cConsOfAKey::vPrintPreRows
        , iConsInAny == iConsInTable
            ? static_cast<tOneTimeAction>(nullptr)
            : &cConsOfAKey::vPrintPostRows
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

void cConsOfAKey::vPrintPreRows(std::ostream& os, const tAttributes& rKey) const
{
    os << boost::format("alter table %s add")
        % ps::lib::sMakeEnclosedName(rKey.szTableName)
        << std::endl;
}

void cConsOfAKey::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(";") << std::endl;
}

size_t cConsOfAKey::iSubtractCtxItemList(
    const tKeyTuple& oKey
){
    return iEraseMatchedKeyItem(
        tAttributes(oKey)
        , oList_
        , &cConsOfAKey::tAttributes::iSearchConsName
    );
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
