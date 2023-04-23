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

#define CONS_TYPE_LEN            (1+1)
#define DEFERRABLE_LEN          (14+1)
#define DEFERRERD_LEN            (9+1)
#define VALIDATED_LEN           (13+1)
#define GENETATED_LEN           (14+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cEffectiveKeyCons::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cEffectiveKeyCons::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the constraint definition
	char szTableName[OBJECT_NAME_LEN];     ///< Name associated with the table (or view) with the constraint definition
	ub4 iConstraintType;                   ///< 1:PK, AK   2:FK
	char szConstraintName[OBJECT_NAME_LEN];///< Name of the constraint definition
    /// - is deferrable (DEFERRABLE)
    /// - or not (NOT DEFERRABLE)
	char szDeferrable[DEFERRABLE_LEN];     ///< Indicates whether the constraint
    /// - was initially deferred (DEFERRED)
    /// - or not (IMMEDIATE)
	char szDeferred[DEFERRERD_LEN];        ///< Indicates whether the constraint
    /// - VALIDATED - All data obeys the constraint (that is, the existing data in the table was validated
    ///   when the constraint was enabled, as well as any subsequent data entered into the table)
    /// - NOT VALIDATED - All data may not obey the constraint (that is, the existing data in the table
    ///   was not validated when the constraint was enabled, but subsequent data entered into the table was validated)
	char szValidated[VALIDATED_LEN];       ///< When STATUS = ENABLED, possible values are:
    /// - is user-generated (USER NAME)
    /// - or system-generated (GENERATED NAME)
	char szGenerated[GENETATED_LEN];       ///< Indicates whether the name of the constraint
    /// - P - Primary key
    /// - U - Unique key
    /// - R - Referential integrity
	char szKeyType[CONS_TYPE_LEN];         ///< Type of the constraint definition:
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iConstraintType(std::get<2>(oKey))
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szDeferrable, 0, sizeof(szDeferrable));
        ::memset(szDeferred, 0, sizeof(szDeferred));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::memset(szKeyType, 0, sizeof(szKeyType));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szConstraintName, std::get<3>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", 0, ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , iConstraintType, rhs.iConstraintType
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , iConstraintType, rhs.iConstraintType
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iToEraseMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cEffectiveKeyCons& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cEffectiveKeyCons::tAttributes::vPrinter(std::ostream& os, const size_t&, cEffectiveKeyCons& oCtx) const
{
    using ps::lib::iCompBinary;
    os << boost::format("  enable %s ")
        % (iCompBinary(strcmp, szValidated,"VALIDATED") ? "validate" : "novalidate");
    if (iCompBinary(strcmp, szGenerated, "GENERATED NAME") && iCompBinary(strcmp, szKeyType, "P"))
    {
        os << boost::format("primary key ") << std::endl;
    }
    else
    {
        os << boost::format("constraint %s ")
            % ps::lib::sMakeEnclosedName(szConstraintName)
            << std::endl;
    }
}

std::string cEffectiveKeyCons::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', iConstraintType=%u, szConstraintName='%s', szDeferrable='%s')"
        R"(, szDeferred='%s', szValidated='%s', szGenerated='%s', szKeyType='%s')"
    ) % szOwner % szTableName % iConstraintType % szConstraintName % szDeferrable
        % szDeferred % szValidated % szGenerated % szKeyType
    ;
    return oss.str();
}

/**
 * @struct cEffectiveKeyCons::cRetriever
 */
class cEffectiveKeyCons::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cEffectiveKeyCons::oList_.
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

const uint32_t cEffectiveKeyCons::cRetriever::iBulkSize_ = 100;

cEffectiveKeyCons::tAttributes cEffectiveKeyCons::cRetriever::rTable_[cEffectiveKeyCons::cRetriever::iBulkSize_];

cEffectiveKeyCons::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szEffectiveKeyCons
        , "Reading effective constraints", &oBind_
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
    oDefine_.vAddItem(rTable_->iConstraintType, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szConstraintName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferrable, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDeferred, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szValidated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGenerated, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szKeyType, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cEffectiveKeyCons::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cEffectiveKeyCons::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cEffectiveKeyCons::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cEffectiveKeyCons::szEffectiveKeyCons[] = {
"select t2.owner "
", t2.table_name "
", decode(t2.constraint_type, 'P', 1, 'U', 1, 2) as constraint_type "
", t2.constraint_name "
", t2.deferrable "
", t2.deferred "
", t2.validated "
", t2.generated "
", t2.constraint_type as key_type "
"from all_constraints t2 "
"where t2.owner in %s "
"and t2.constraint_type in ('P','U','R') "
"and t2.status = 'ENABLED' "
"and not exists ( "
	"select null "
	"from all_indexes "
	"where owner = t2.owner "
	"and index_name = t2.constraint_name "
	"and t2.constraint_type = 'P' "
	"and index_type = 'IOT - TOP' "
") "
};

const ub4 cEffectiveKeyCons::iKeyTypeIsUnique = 1;
const ub4 cEffectiveKeyCons::iKeyTypeIsForeign = 2;

cEffectiveKeyCons::cEffectiveKeyCons()
{
    // nothing to do
}

cEffectiveKeyCons::~cEffectiveKeyCons()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cEffectiveKeyCons::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cEffectiveKeyCons::vPrintAllItems(
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
        , &cEffectiveKeyCons::tAttributes::iKeyMatched
        , &cEffectiveKeyCons::tAttributes::vPrinter
        , &cEffectiveKeyCons::vPrintPreRows
        , &cEffectiveKeyCons::vPrintPostRows
        , &cEffectiveKeyCons::vPrintNotFound
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

size_t cEffectiveKeyCons::iSubtractCtxItemList(
    const tKeyTuple& oKey
){
    return iEraseMatchedKeyItem(
        tAttributes(oKey)
        , oList_
        , &cEffectiveKeyCons::tAttributes::iToEraseMatched
    );
}

void cEffectiveKeyCons::vPrintPreRows(std::ostream& os, const tAttributes& oKey) const
{
    os << boost::format("alter table %s")
        % ps::lib::sMakeEnclosedName(oKey.szTableName)
        << std::endl;
}

void cEffectiveKeyCons::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(";") << std::endl;
}

void cEffectiveKeyCons::vPrintNotFound(std::ostream& os, const tAttributes& oKey) const
{
    if (oKey.iConstraintType == iKeyTypeIsUnique)
    {
        os << std::string("-- No Effective PK or AK constraints.");
    }
    else if (oKey.iConstraintType == iKeyTypeIsForeign)
    {
        os << std::string("-- No Effective FK constraints.");
    }
    else
    {
        os << boost::format("-- Unexpected constraint type %u.") % oKey.iConstraintType;
    }
    os << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
