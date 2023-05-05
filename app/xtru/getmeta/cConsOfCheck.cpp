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

#define STATUS_LEN           (8+1)
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
 * @struct cConsOfCheck::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cConsOfCheck::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];            ///< Name of object owner.
	char szTableName[OBJECT_NAME_LEN];        ///< Name of table column.
	char szConstraintName[OBJECT_NAME_LEN];   ///< Name of constraint.
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
    /**
     * Text of search condition for a check constraint. This column returns
     *  the correct value onlywhen the row originates from the current container.
     */
	char *szSearchCondition;                  ///< Content of the check constraint.
	ps::lib::sql::ind_t nSearchConditionInd;  ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iSearchCondition;                     ///< length of the expression body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szSearchCondition(nullptr)
        , nSearchConditionInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iSearchCondition(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szStatus, 0, sizeof(szStatus));
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
    {
        // clean up.
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szStatus, 0, sizeof(szStatus));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        // copy the data.
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szConstraintName, rhs.szConstraintName);
        ::strcpy(szSearchCondition, rhs.szSearchCondition);
        ::strcpy(szStatus, rhs.szStatus);
        ::strcpy(szDeferrable, rhs.szDeferrable);
        ::strcpy(szDeferred, rhs.szDeferred);
        ::strcpy(szValidated, rhs.szValidated);
        ::strcpy(szGenerated, rhs.szGenerated);
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
    void vPrinter(std::ostream& os, const size_t& i, cConsOfCheck& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cConsOfCheck::tAttributes::vPrinter(std::ostream& os, const size_t&, cConsOfCheck&) const
{
    using ps::lib::iCompBinary;
    const auto sWork = std::string(
        &szSearchCondition[0]
        , &szSearchCondition[iSearchCondition]
    );
    // Consolidate multiple blanks into a single LF.
    const auto sStr = ps::lib::sConsolidatePolyBlanksIntoMonoLf(sWork);
    os << std::string(", ")
        << boost::format(
            iCompBinary(strcmp, szGenerated, "USER NAME") ? "constraint %s " : "/* constraint %s */ " 
        ) % ps::lib::sMakeEnclosedName(szConstraintName, MINIMUM_COLUMN_LENGTH)
        << std::string("check ") << std::endl
        << boost::format("  ( %s") % sStr << std::endl
        << boost::format("  ) %s %s %s %s")
            % (iCompBinary(strcmp, szDeferrable, "NOT DEFERRABLE") ? "not deferrable" : "deferrable")
            % (iCompBinary(strcmp, szDeferred, "IMMEDIATE") ? "initially immediate" : "initially deferred")
            % (iCompBinary(strcmp, szStatus, "ENABLED") ? "enable" : "disable")
            % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
        << std::endl
        ;
}

std::string cConsOfCheck::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szConstraintName='%s')"
        R"(, szStatus='%s', szDeferrable='%s', szDeferred='%s')"
        R"(, szValidated='%s', szGenerated='%s', szSearchCondition='%s'(%d))"
    ) % szOwner % szTableName % szConstraintName
        % szStatus % szDeferrable % szDeferred
        % szValidated % szGenerated % szSearchCondition % nSearchConditionInd
    ;
    return oss.str();
}

/**
 * @struct cConsOfCheck::cRetriever
 */
class cConsOfCheck::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cConsOfCheck::oList_.
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

const uint32_t cConsOfCheck::cRetriever::iBulkSize_ = 100;

cConsOfCheck::tAttributes cConsOfCheck::cRetriever::rTable_[cConsOfCheck::cRetriever::iBulkSize_];

cConsOfCheck::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szConsOfCheck
        , "Reading check constraints", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szSearchCondition, &rTable_->nSearchConditionInd, &rTable_->iSearchCondition)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szStatus, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cConsOfCheck::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cConsOfCheck::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    uint32_t iDeleteRows = 0;
    std::copy_if(
        &rTable_[0], &rTable_[iNumIter]
        , std::inserter(oList_, oList_.end())
        , [&iDeleteRows](const tAttributes& item)
        {
            // except for 'x IS NOT NULL'
            const bool iRet = ! ps::lib::iMatchedBackward(item.szSearchCondition, "IS NOT NULL");
            if (iRet) ++iDeleteRows;
            return iRet;
        }
    );
    vAddOutputBytes(iSkip_ * (iNumIter - iDeleteRows));
    vAddOutputRows(iNumIter - iDeleteRows);
}

void cConsOfCheck::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cConsOfCheck::szConsOfCheck[] = {
"select owner "
", table_name "
", constraint_name "
", status "
", deferrable "
", deferred "
", validated "
", generated "
", search_condition "
"from all_constraints "
"where owner in %s "
"and constraint_type = 'C' "
// Oops! You will take "ORA-00932: inconsistent datatypes: expected CHAR got LONG", if following is valid.
// "and search_condition not like '% IS NOT NULL' "
};

cConsOfCheck::cConsOfCheck()
{
    // nothing to do
}

cConsOfCheck::~cConsOfCheck()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cConsOfCheck::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cConsOfCheck::vPrintAllItems(
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
        , &cConsOfCheck::tAttributes::iKeyMatched
        , &cConsOfCheck::tAttributes::vPrinter
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
