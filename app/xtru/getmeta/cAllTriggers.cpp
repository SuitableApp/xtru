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

#define DESCRIPTION_LEN           (4000+1)

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/** 
 * @struct cAllTriggers::tAttributes
 * See to study about [ALL_TRIGGERS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-0558FD5E-3EAD-4829-B9A5-4AA029C2105E.htm#REFRN20293)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTriggers::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Owner of the trigger
	char szTableName[OBJECT_NAME_LEN];     ///< indicates the table or view name on which the trigger is defined
	char szTriggerName[OBJECT_NAME_LEN];   ///< Name of the trigger
    /// - useful for re-creating a trigger creation statement
	char szDescription[DESCRIPTION_LEN];   ///< Trigger description
	char *szTriggerBody;                   ///< Statements executed by the trigger when it fires
	ps::lib::sql::ind_t nTriggerBodyInd;   ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iTriggerBody;                      ///< 
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szTriggerBody(nullptr)
        , nTriggerBodyInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iTriggerBody(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szTriggerName, 0, sizeof(szTriggerName));
        ::memset(szDescription, 0, sizeof(szDescription));
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
     * Since tAttributes::szTriggerBody which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szTriggerBody(new char[rhs.iTriggerBody + 1])
        , nTriggerBodyInd(rhs.nTriggerBodyInd)
        , iTriggerBody(rhs.iTriggerBody)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szTriggerName, 0, sizeof(szTriggerName));
        ::memset(szDescription, 0, sizeof(szDescription));
        ::strcpy(szOwner, rhs.szOwner);
        ::strcpy(szTableName, rhs.szTableName);
        ::strcpy(szTriggerName, rhs.szTriggerName);
        ::strcpy(szDescription, rhs.szDescription);
        ::strcpy(szTriggerBody, rhs.szTriggerBody);
    }
    ~tAttributes()
    {
        delete [] szTriggerBody;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szTableName, rhs.szTableName
            , szTriggerName, rhs.szTriggerName
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
    void vPrinter(std::ostream& os, const size_t& i, cAllTriggers& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
    /// A string format conditionaer.
    static std::string sConditioner(const char szWork[]);
};

std::string cAllTriggers::tAttributes::sConditioner(const char szWork[])
{
    // Convert the end-of-line sequence used by Windows to xUNIX.
    std::string sWork =
        ps::lib::sConvertEolSequence(szWork, "\r\n", "\n");
    // Remove any characters in specific if they are used at the end of the string.
    ps::lib::vRmSpecCharsAtEofStr(sWork, "\r\n/");
    // Consolidate multiple blanks and LFs into a single LF.
    return ps::lib::sConsolidatePolyBlanksIntoMonoLf(sWork);
}

void cAllTriggers::tAttributes::vPrinter(std::ostream& os, const size_t&, cAllTriggers&) const
{
    os << std::string("create or replace trigger ")
        << boost::format("%s") % sConditioner(szDescription)        << std::endl
        << boost::format("%s") % sConditioner(szTriggerBody)        << std::endl
        << std::string("/")                                         << std::endl
        << boost::format("show errors trigger %s;")
            % ps::lib::sMakeEnclosedName(szTriggerName)
        << std::endl
        ;
}

std::string cAllTriggers::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', szTriggerName='%s', szDescription='%s')"
        R"(, iTriggerBody=%u, szTriggerBody='%s')"
    ) % szOwner % szTableName % szTriggerName % szDescription
        % iTriggerBody % szTriggerBody
    ;
    return oss.str();
}

/**
 * @struct cAllTriggers::cRetriever
 */
class cAllTriggers::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTriggers::oList_.
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

const uint32_t cAllTriggers::cRetriever::iBulkSize_ = 10;

cAllTriggers::tAttributes cAllTriggers::cRetriever::rTable_[cAllTriggers::cRetriever::iBulkSize_];

cAllTriggers::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , szAllTriggers
        , "Reading trigger properties", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , pv_(iSkip_, &rTable_->szTriggerBody, &rTable_->nTriggerBodyInd, &rTable_->iTriggerBody)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTriggerName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDescription, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cAllTriggers::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTriggers::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllTriggers::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTriggers::szAllTriggers[] = {
"select owner "
", table_name "
", trigger_name "
", description "
", trigger_body "
"from all_triggers "
"where owner in %s "
"and table_owner = owner "
"and base_object_type in ('TABLE','VIEW') " /* Added v3.064 */
};

cAllTriggers::cAllTriggers()
{
    // nothing to do
}

cAllTriggers::~cAllTriggers()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTriggers::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllTriggers::vPrintAllItems(
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
        , &cAllTriggers::tAttributes::iKeyMatched
        , &cAllTriggers::tAttributes::vPrinter
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
