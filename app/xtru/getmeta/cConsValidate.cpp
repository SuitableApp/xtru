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

#define VALIDATED_LEN  13+1
#define GENETATED_LEN  14+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cConsValidate::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cConsValidate::tAttributes
{
    char szOwner[OBJECT_NAME_LEN];         ///< 
    char szKeyName[OBJECT_NAME_LEN];       ///< A part of key identification.
    char szTableName[OBJECT_NAME_LEN];     ///< A name supplies to "alter table".
    ps::lib::sql::ind_t nTableNameInd;     ///<
    char szConstraintName[OBJECT_NAME_LEN];///< 
    ps::lib::sql::ind_t nConstraintNameInd;///< 
    char szValidated[VALIDATED_LEN];       ///< 
    ps::lib::sql::ind_t nValidatedInd;     ///< 
    char szGenerated[GENETATED_LEN];       ///< 
    ps::lib::sql::ind_t nGeneratedInd;     ///< 
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nTableNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nConstraintNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nValidatedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nGeneratedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szKeyName, 0, sizeof(szKeyName));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szConstraintName, 0, sizeof(szConstraintName));
        ::memset(szValidated, 0, sizeof(szValidated));
        ::memset(szGenerated, 0, sizeof(szGenerated));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szKeyName, std::get<1>(oKey));
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
            , szKeyName, rhs.szKeyName
            , szConstraintName, rhs.szConstraintName
            , szValidated, rhs.szValidated
            , szGenerated, rhs.szGenerated
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szKeyName, rhs.szKeyName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iSearchConsName(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szKeyName, rhs.szKeyName
            , szConstraintName, rhs.szConstraintName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintFkForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void cPrintFkBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintAkCkForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintAkCkBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintNNForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintNNBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintTrgForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrintTrgBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cConsValidate::tAttributes::vPrintFkForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableDisable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szTableName)
            % ps::lib::sMakeEnclosedName(szConstraintName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::cPrintFkBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    using ps::lib::iCompBinary;
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableEnable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szTableName)
            % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
            % ps::lib::sMakeEnclosedName(szConstraintName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintAkCkForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableDisable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szKeyName)
            % ps::lib::sMakeEnclosedName(szConstraintName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintAkCkBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    using ps::lib::iCompBinary;
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableEnable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szKeyName)
            % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
            % ps::lib::sMakeEnclosedName(szConstraintName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintNNForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableDisable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szKeyName)
            % ps::lib::sMakeEnclosedName(szConstraintName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintNNBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    using ps::lib::iCompBinary;
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableEnable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szKeyName)
            % ps::lib::sMakeEnclosedName(szConstraintName)
            % (iCompBinary(strcmp, szValidated, "VALIDATED") ? "validate" : "novalidate")
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintTrgForward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableDisable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szTableName)
        << std::endl
        ;
}

void cConsValidate::tAttributes::vPrintTrgBackward(std::ostream& os, const size_t& i, cConsValidate& oCtx) const
{
    const auto& oVal = oCtx.oVal_;
    os
        << boost::format(oVal.sAltTableEnable)
            % oVal.sCommentStr
            % ps::lib::sMakeEnclosedName(szTableName)
        << std::endl
        ;
}

std::string cConsValidate::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szKeyName='%s', szTableName='%s(%d)')"
        R"(, szConstraintName='%s'(%d))"
        R"(, szValidated='%s(%d)', szGenerated='%s(%d)')"
    ) % szOwner % szKeyName % szTableName % nTableNameInd
        % szConstraintName % nConstraintNameInd
        % szValidated % nValidatedInd % szGenerated % nGeneratedInd
    ;
    return oss.str();
}

namespace /* annonymous */ {
const std::string
  DB_CONS_C1024("%salter table %s disable constraint %s;") ///< also using for C1025
, DB_CONS_C1026("%salter table %s disable constraint %s drop index;")
, EB_CONS_C1024("%salter table %s enable %s constraint %s;") ///< also using for C1025 and C1026
, DB_CONS_C1027("%salter table %s modify (%s null);")
, EB_CONS_C1027("%salter table %s modify (%s not null %s);")
, DB_CONS_C1028("%salter trigger %s disable;")
, EB_CONS_C1028("%salter trigger %s enable;")
;

/**
 * @brief
 * - returns the referential constraint name of the child table
 *   that refers to the parent table of the output target
 * - OR
 *   If FK corresponding to the above is not found, passively
 *   look for FK that is being enforced on output target itself.
 */
const std::string sC1024 =
"select t1.owner "
    // szKeyName : A part of key identification.
", ifnull(t0.table_name, t2.table_name) "
    // szTableName : A name supplies to "alter table".
", t1.table_name "
", t1.constraint_name "
", t1.validated "
", t1.generated "
"from EFFECTIVE_CONS t1 "        // referrer (ex.EMP)
"inner join EFFECTIVE_CONS t2 "  // referent (ex.DEPT)
"on t2.owner = t1.r_owner "
"and t2.constraint_name = t1.r_constraint_name "
"left outer join TARGET_TABLES t0 " // referrer is also a target?
"on t0.table_name = t1.table_name "
"where t1.owner in %s "
"and t1.constraint_type = 'R' "
"and t1.status = 'ENABLED' "
;
/**
 * @brief
 *   returns the name of the check constraint (exclude NN)
 *   to be enforced to the output target table
 */
const std::string sC1025 =
"select owner "
", table_name "
", NULL "
", constraint_name "
", validated "
", generated "
"from EFFECTIVE_CONS "
"where owner in %s "
"and constraint_type = 'C' "
"and status = 'ENABLED' "
"and search_condition not like '%% IS NOT NULL' "
;
/**
 * @brief
 *   returns primary and unique key constraint name
 *   enforced to output target tablea.
 */
const std::string sC1026 =
"select t1.owner "
", t1.table_name "
", NULL "
", t1.constraint_name "
", t1.validated "
", t1.generated "
"from EFFECTIVE_CONS t1 "
", ALL_INDEXES t2 "
"where t1.owner in %s "
"and t1.constraint_type in ('P','U') "
"and t1.status = 'ENABLED' "
"and t2.owner = t1.index_owner "
"and t2.index_name = t1.index_name "
"and t2.index_type != 'IOT - TOP' "
;
/**
 * @brief
 *   returns the Not NULL constraint name
 *   to be enforced to the output target table.
 */
const std::string sC1027 =
"select t0.owner "
", t0.table_name "
", NULL "
", t1.column_name "
", t0.validated "
", t0.generated "
"from EFFECTIVE_CONS t0 "
", ALL_TAB_COLUMNS t1 "
"where t0.owner in %s "
"and t0.constraint_type = 'C' "
"and t0.search_condition like '%%IS NOT NULL' "
"and t1.owner = t0.owner "
"and t1.table_name = t0.table_name "
"and '\"' || t1.column_name || '\"' || ' IS NOT NULL' = t0.search_condition "
"and t1.nullable = 'N' "
"and not exists ( "
	 /* Exclude the NN that is the basis of the primary key constraint */
	"select null "
	"from EFFECTIVE_CONS t2 "
	", ALL_CONS_COLUMNS t3 "
	"where t2.owner = t0.owner "
	"and t2.table_name = t0.table_name "
	"and t2.constraint_type = 'P' "
	"and t3.owner = t2.owner "
	"and t3.table_name = t2.table_name "
	"and t3.constraint_name = t2.constraint_name "
	"and t3.column_name = t1.column_name "
") "
;
/**
 * @brief
 *   Returns the database trigger name
 *   to be enforced to the output target table
 */
const std::string sC1028 =
"select t1.owner "
", t1.table_name "
", t1.trigger_name "
", NULL "
", NULL "
", NULL "
"from TARGET_TABLES t0 "
", ALL_TRIGGERS t1 "
"where t1.owner in %s "
"and t1.owner = t0.owner "
"and t1.owner = t0.table_name "
"and t1.status = 'ENABLED' "
;

} /* annonymous */

cConsValidate::tMap cConsValidate::oMap =
{ cConsValidate::tMap::value_type(
    iC1024,
    { 0x8000              // iMask
    , sC1024              // sSelectStmt
    , std::string()       // sCommentStr
    , "FK constraints"    // sProcTitle
    , DB_CONS_C1024       // sAltTableDisable
    , EB_CONS_C1024       // sAltTableEnable
    , &cConsValidate::tAttributes::vPrintFkForward
    , &cConsValidate::tAttributes::cPrintFkBackward
    })
, cConsValidate::tMap::value_type(
    iC1025,
    { 0x4000
    , sC1025
    , std::string()
    , "check constraints"
    , DB_CONS_C1024
    , EB_CONS_C1024
    , &cConsValidate::tAttributes::vPrintAkCkForward
    , &cConsValidate::tAttributes::vPrintAkCkBackward
    })
, cConsValidate::tMap::value_type(
    iC1026,
    { 0x2000
    , sC1026
    , std::string()
    , "PK or AK constraints"
    , DB_CONS_C1026
    , EB_CONS_C1024
    , &cConsValidate::tAttributes::vPrintAkCkForward
    , &cConsValidate::tAttributes::vPrintAkCkBackward
    })
, cConsValidate::tMap::value_type(
    iC1027,
    { 0x1000
    , sC1027
    , std::string()
    , "Not NULL constraints"
    , DB_CONS_C1027
    , EB_CONS_C1027
    , &cConsValidate::tAttributes::vPrintNNForward
    , &cConsValidate::tAttributes::vPrintNNBackward
    })
, cConsValidate::tMap::value_type(
    iC1028,
    { 0x0800
    , sC1028
    , std::string()
    , "triggers"
    , DB_CONS_C1028
    , EB_CONS_C1028
    , &cConsValidate::tAttributes::vPrintTrgForward
    , &cConsValidate::tAttributes::vPrintTrgBackward
    })
};

cConsValidate::cSingleton::cSingleton(const int iMngScrOut, tMap& oMap)
{
    for (auto& item : oMap)
    {
        item.second.sCommentStr = iMngScrOut & item.second.iMask ? "" : "-- ";
    }
}

void cConsValidate::vSetMask(const int iMngScrOut)
{
    static cSingleton s(iMngScrOut, oMap);
}

cConsValidate::cConsValidate(
    const tConsType& iConsType
)
    : oVal_(oMap[iConsType])
{}

cConsValidate::~cConsValidate()
{
    // nothing to do
}

void cConsValidate::vExecuteAndFetch(
    ps::lib::sql::lite3::cSqliteDb& oDb
    , const ps::lib::str_vct& oOwners
){
//  ps::lib::cTracer& trc_(ps::lib::cTracer::get_mutable_instance());
    ps::lib::cDistributor& mos_(ps::lib::cDistributor::get_mutable_instance());
    tAttributes rRowBuf;
    const size_t iSkip_ = sizeof(tAttributes);
    ps::lib::sql::lite3::cSqliteStmt oStmt(oDb, oVal_.sSelectStmt);
    oStmt.vConvPlaceHolder({ sGetSqlInList(oOwners) });
    ASSERT_OR_RAISE_FNC(oStmt.iParse() == SQLITE_OK, std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb));
    // tells user memory addresses to ps::lib::sql::lite3::cSqliteStmt
    ps::lib::sql::lite3::cDefine& oDefine(oStmt.oGetDefine());
    using ps::lib::sql::lite3::cAttr;
    oDefine.vAddItem(rRowBuf.szOwner, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szKeyName, cAttr::STR, NULL, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szTableName, cAttr::STR, &rRowBuf.nTableNameInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szConstraintName, cAttr::STR, &rRowBuf.nConstraintNameInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szValidated, cAttr::STR, &rRowBuf.nValidatedInd, iSkip_, iSkip_);
    oDefine.vAddItem(rRowBuf.szGenerated, cAttr::STR, &rRowBuf.nGeneratedInd, iSkip_, iSkip_);
    ps::lib::sql::lite3::cDirectiveHolder oDirectiveHolder(
        [&] { oList_.push_back(tAttributes(rRowBuf)); }
        , [&] { mos_ << boost::format(R"(Start to read "%s")") % oVal_.sProcTitle << std::endl; }
        , [&] { mos_ << boost::format(R"(Finished to read and returned %d rows)") % oList_.size() <<  std::endl; }
        , [&] { mos_ << boost::format(R"(Not found "%s")") % oVal_.sProcTitle << std::endl; }
        , [&] { oList_.sort(); }
    );
    ASSERT_OR_RAISE_FNC(oStmt.iFetch(oDirectiveHolder) == SQLITE_DONE
        , std::runtime_error, ps::lib::sql::lite3::cCheckErr(oDb));
}

void cConsValidate::vPrintAllItems(
    std::ostream& os
    , const tKeyTuple& oKey
    , const bool& iDispose
    , const tWayToNext& iWayToNext
){
    ps::lib::cList<tIter> oArr;
    ps::app::xtru::getmeta::vPrintMatched(
        os
        , *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , &cConsValidate::tAttributes::iKeyMatched
        , iWayToNext == iForward
            ? oVal_.vPrintForward
            : oVal_.vPrintBackward
        , &cConsValidate::vPrintPreRows
        , &cConsValidate::vPrintPostRows
        , &cConsValidate::vPrintNotFound
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

void cConsValidate::vPrintPreRows(std::ostream& os, const tAttributes& rKey) const
{
    os << boost::format("-- %s")
            % oVal_.sProcTitle
        << std::endl;
}

void cConsValidate::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    // Nothing to do.
}

void cConsValidate::vPrintNotFound(std::ostream& os, const tAttributes&) const
{
    os
        << boost::format("-- Any %s were not found.")
            % oVal_.sProcTitle
        << std::endl;
}

size_t cConsValidate::iSubtractCtxItemList(
    const tKeyTuple& oKey
){
    return ps::app::xtru::getmeta::iEraseMatchedKeyItem(
        tAttributes(oKey)
        , oList_
        , &cConsValidate::tAttributes::iSearchConsName
    );
}

void cConsValidate::vReverse()
{
    oList_.reverse();
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
