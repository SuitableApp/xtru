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
 * @struct cAllIndColumns::tAttributes
 * See to study about [ALL_IND_COLUMNS]
 * (https://docs.oracle.com/cd/E11882_01/server.112/e40402/statviews_1103.htm#REFRN20084)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllIndColumns::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the index
	char szTableName[OBJECT_NAME_LEN];     ///< Name of the table or cluster
	char szIndexName[OBJECT_NAME_LEN];     ///< Name of the index
	ub4 iColumnPosition;                   ///< Position of the column or attribute within the index
	ub4 iFunctionBased;                    ///< Used to indicate whether it is a constituent column of a function index
	ps::lib::sql::ind_t nFunctionBasedInd; ///< ind_t::VAL_IS_NULL: value is NULL
	char *szColumnName;                    ///< Column name or attribute of the object type column
	ps::lib::sql::ind_t nColumnNameInd;    ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iColumnName;                       ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnPosition(0)
        , iFunctionBased(0)
        , nFunctionBasedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , szColumnName(nullptr)
        , nColumnNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iColumnName(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szIndexName, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szColumnName which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iColumnPosition(rhs.iColumnPosition)
        , iFunctionBased(rhs.iFunctionBased)
        , nFunctionBasedInd(rhs.nFunctionBasedInd)
        , szColumnName(new char[rhs.iColumnName + 1])
        , nColumnNameInd(rhs.nColumnNameInd)
        , iColumnName(rhs.iColumnName)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szIndexName, 0, sizeof(szIndexName));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szIndexName, rhs.szIndexName);
        ::strcpy(szColumnName, rhs.szColumnName);
    }
    ~tAttributes()
    {
        delete [] szColumnName;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
            , iColumnPosition, rhs.iColumnPosition
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szIndexName, rhs.szIndexName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cAllIndColumns& oCtx) const
    {
        if (nFunctionBasedInd == ps::lib::sql::ind_t::VAL_IS_NULL)
        {
            const std::string sWork(&szColumnName[0], &szColumnName[iColumnName]);
            os << boost::format("%s%s%s")
                % (i == 0 ? "( " : ", ")
                % sColumnIdCooment(oCtx.oAllTabColumns_.iGetColumnId(
                    szOwner, szTableName, sWork.c_str()
                ))
                % ps::lib::sMakeEnclosedName(sWork)
                << std::endl;
        }
        else
        {
            os << oPrintItems(
                oCtx.oAllIndExpressions_
                , std::make_tuple(szOwner, szTableName, szIndexName, iColumnPosition)
                , true
            );
        }
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllIndColumns::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szIndexName='%s', iFunctionBased=%u(%s))"
        R"(, iColumnPosition=%u, szColumnName='%s'(%d), iColumnName=%u)"
    ) % szOwner % szTableName % szIndexName % iFunctionBased % nFunctionBasedInd
        % iColumnPosition % szColumnName % nColumnNameInd % iColumnName
    ;
    return oss.str();
}

/**
 * @struct cAllIndColumns::cRetriever
 */
class cAllIndColumns::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSubPartKeyColumns::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    ps::lib::sql::occi::cPieceVct pv_;
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

const uint32_t cAllIndColumns::cRetriever::iBulkSize_ = 100;

cAllIndColumns::tAttributes cAllIndColumns::cRetriever::rTable_[cAllIndColumns::cRetriever::iBulkSize_];

cAllIndColumns::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllIndColumns
        , "Reading attributes of index column", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szColumnName, &rTable_->nColumnNameInd, &rTable_->iColumnName)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szIndexName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnPosition, SQLT_UIN, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->iFunctionBased, SQLT_UIN, &rTable_->nFunctionBasedInd, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, static_cast<void*>(&pv_)
    );
}

void cAllIndColumns::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllIndColumns::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllIndColumns::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllIndColumns::szAllIndColumns[] = {
"select ic.table_owner "
", ic.table_name "
", ic.index_name "
", ic.column_position "
", ie.column_position "
", ic.column_name "
"from all_ind_columns ic "
", all_ind_expressions ie "
"where ic.table_owner in %s "
"and ic.index_owner = ie.index_owner(+) "
"and ic.index_name = ie.index_name(+) "
"and ic.column_position = ie.column_position(+) "
};

cAllIndColumns::cAllIndColumns(
    cAllTabColumns& oAllTabColumns
    , cAllIndExpressions& oAllIndExpressions
)
    : oAllTabColumns_(oAllTabColumns)
    , oAllIndExpressions_(oAllIndExpressions)
{
    // nothing to do
}

cAllIndColumns::~cAllIndColumns()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllIndColumns::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllIndColumns::vPrintAllItems(
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
        , &cAllIndColumns::tAttributes::iKeyMatched
        , &cAllIndColumns::tAttributes::vPrinter
        , static_cast<tOneTimeAction>(nullptr)
        , &cAllIndColumns::vPrintPostRows
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

void cAllIndColumns::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(") ");
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
