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
 * @struct cDataDefault::tAttributes
 * See to study about [ALL_TAB_COLUMNS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-F218205C-7D76-4A83-8691-BFD2AD372B63.htm#REFRN20277)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cDataDefault::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szTableName[OBJECT_NAME_LEN];     ///< Name of table.
	char szColumnName[OBJECT_NAME_LEN];    ///< Name of column.
	ub4 iColumnId;                         ///< ID of column in table.
	char *szDetaDefault;                   ///< Comment body
	ps::lib::sql::ind_t nDetaDefaultInd;   ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iDetaDefault;                      ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iColumnId(0)
        , szDetaDefault(nullptr)
        , nDetaDefaultInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iDetaDefault(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
        ::strcpy(szColumnName, std::get<2>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", "", ""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szDetaDefault which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : iColumnId(rhs.iColumnId)
        , szDetaDefault(new char[rhs.iDetaDefault + 1])
        , nDetaDefaultInd(rhs.nDetaDefaultInd)
        , iDetaDefault(rhs.iDetaDefault)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szColumnName, 0, sizeof(szColumnName));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szColumnName, rhs.szColumnName);
        ::strcpy(szDetaDefault, rhs.szDetaDefault);
    }
    ~tAttributes()
    {
        delete [] szDetaDefault;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , iColumnId, rhs.iColumnId
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
    void vPrinter(std::ostream& os, const size_t& i, cDataDefault& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cDataDefault::tAttributes::vPrinter(std::ostream& os, const size_t& i, cDataDefault& oCtx) const
{
    // Convert the end-of-line sequence used by Windows to xUNIX.
    std::string sWork =
        ps::lib::sConvertEolSequence(szDetaDefault, "\r\n", "\n");
    // Remove any characters in specific if they are used at the end of the string.
    ps::lib::vRmSpecCharsAtEofStr(sWork, "\r\n ");
    os << boost::format("%c %s%s default %s")
            % (i == 0 ? '(' : ',')
            % sColumnIdCooment(iColumnId)
            % ps::lib::sMakeEnclosedName(szColumnName, MINIMUM_COLUMN_LENGTH)
            % sWork
        << std::endl
        ;
}

std::string cDataDefault::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szColumnName='%s', iColumnId=%u)"
        R"(, iDetaDefault=%u, szDetaDefault='%s')"
    ) % szOwner % szTableName % szColumnName % iColumnId
        % iDetaDefault % szDetaDefault
    ;
    return oss.str();
}

/**
 * @struct cDataDefault::cRetriever
 */
class cDataDefault::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cDataDefault::oList_.
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

const uint32_t cDataDefault::cRetriever::iBulkSize_ = 100;

cDataDefault::tAttributes cDataDefault::cRetriever::rTable_[cDataDefault::cRetriever::iBulkSize_];

cDataDefault::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szDataDefault
        , "Reading default value of columns", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szDetaDefault, &rTable_->nDetaDefaultInd, &rTable_->iDetaDefault)
{
    std::string predicate;
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szColumnName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iColumnId, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cDataDefault::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cDataDefault::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cDataDefault::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cDataDefault::szDataDefault[] = {
"select tc.owner "
", tc.table_name "
", tc.column_name "
", tc.column_id "
", tc.data_default "
"from all_tab_columns tc "
"where owner in %s "
"and data_default is not null "
};

cDataDefault::cDataDefault()
{
    // nothing to do
}

cDataDefault::~cDataDefault()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cDataDefault::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cDataDefault::vPrintAllItems(
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
        , &cDataDefault::tAttributes::iKeyMatched
        , &cDataDefault::tAttributes::vPrinter
        , &cDataDefault::vPrintPreRows
        , &cDataDefault::vPrintPostRows
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

void cDataDefault::vPrintPreRows(std::ostream& os, const tAttributes& rKey) const
{
    os << boost::format("alter table %s modify")
            % ps::lib::sMakeEnclosedName(rKey.szTableName)
        << std::endl
        ;
}

void cDataDefault::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os << std::string(");") << std::endl
        ;
}

std::string cDataDefault::sGetVirtualColumnClause(const cDataDefault::tKeyTuple& oKey)
{
    ps::lib::cList<tIter> oArr;
    const std::string str = sConcatMatched(
        *this
        , tAttributes(oKey)
        , { oList_.begin(), oList_.end() }
        , [](const tAttributes& lhs, const tAttributes& rhs)
            { return ps::lib::composite_equal(
                lhs.szOwner, rhs.szOwner
                , lhs.szTableName, rhs.szTableName
                , lhs.szColumnName, rhs.szColumnName
            );}
        , [](const tAttributes& rCur, const size_t&, cDataDefault&){ return std::string(rCur.szDetaDefault); }
        , [](cDataDefault&){ return std::string("generated always as ("); }
        , [](cDataDefault&){ return std::string(") virtual "); }
        , [](cDataDefault&){ return std::string(""); }
        , std::back_inserter(oArr)
    );
    boost::for_each(
        oArr, [this](tIter it){oList_.erase(it);}
    );
    return str;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
