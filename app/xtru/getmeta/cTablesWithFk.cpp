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
 * @struct cTablesWithFk::tAttributes
 * See to study about [ALL_CONSTRAINTS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-9C96DA92-CFE0-4A3F-9061-C5ED17B43EFE.htm#REFRN20047)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cTablesWithFk::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the cluster
	char szTableName[OBJECT_NAME_LEN];     ///< Clustered table name
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
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
    void vPrinter(std::ostream& os, const size_t& i, cTablesWithFk& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cTablesWithFk::tAttributes::vPrinter(std::ostream& os, const size_t& i, cTablesWithFk& oCtx) const
{
    os << sPrintLabel("Chds", i + 1, std::string(szTableName) + "_FK");
    if (! oCtx.iDependent_)
    {
        os << boost::format("alter table %s add")
                % ps::lib::sMakeEnclosedName(szTableName)
           << std::endl
           << std::string("(")
           << oPrintItems(
                oCtx.oConsOfFKey_, std::make_tuple(szOwner, szTableName)
                ,true , cConsOfFKey::tInvCtx::iInAltTbl
           )
           << std::string(");")
           << std::endl
           ;
    }
    os << oPrintItems(
        oCtx.oEffectiveKeyCons_, std::make_tuple(szOwner, szTableName, cEffectiveKeyCons::iKeyTypeIsForeign, ""), true
    );
}

std::string cTablesWithFk::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s')"
    ) % szOwner % szTableName
    ;
    return oss.str();
}

/**
 * @struct cTablesWithFk::cRetriever
 */
class cTablesWithFk::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cTablesWithFk::oList_.
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

const uint32_t cTablesWithFk::cRetriever::iBulkSize_ = 100;

cTablesWithFk::tAttributes cTablesWithFk::cRetriever::rTable_[cTablesWithFk::cRetriever::iBulkSize_];

cTablesWithFk::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szTablesWithFk
        , "Reading tables referring to other tables", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cTablesWithFk::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cTablesWithFk::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cTablesWithFk::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cTablesWithFk::szTablesWithFk[] = {
"select distinct owner, table_name "
"from all_constraints "
"where owner in %s "
"and constraint_type = 'R' "
};

cTablesWithFk::cTablesWithFk(
    const bool iDependent
    , cEffectiveKeyCons& oEffectiveKeyCons
    , cConsOfFKey& oConsOfFKey
)
    : iNumRows_(0)
    , iDependent_(iDependent)
    , oEffectiveKeyCons_(oEffectiveKeyCons)
    , oConsOfFKey_(oConsOfFKey)
{
    // nothing to do
}

cTablesWithFk::~cTablesWithFk()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cTablesWithFk::oSubmit(
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

void cTablesWithFk::vPrintAllItems(
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
        , &cTablesWithFk::tAttributes::iKeyMatched
        , &cTablesWithFk::tAttributes::vPrinter
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
