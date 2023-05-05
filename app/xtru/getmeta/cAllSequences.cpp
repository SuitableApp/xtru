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

#define MINMAX_VALUE_LEN 28+1
#define CYCLEFLG_LEN      1+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllSequences::tAttributes
 * The reason why the text type is brought into correspondence to the column
 * defined by the integer type of Oracle is because there is a possibility
 * that the number exceeding the range expressible by long can be returned.
 *
 * See to study about [ALL_SEQUENCES]
 * (https://docs.oracle.com/database/121/REFRN/GUID-5867804F-1339-4CB6-9A56-6B6DCECB61BB.htm#REFRN20244)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllSequences::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szSequenceName[OBJECT_NAME_LEN];  ///< Name of object
	char szMinValue[MINMAX_VALUE_LEN];     ///< Integer of minimum value.
	ps::lib::sql::ind_t nMinValueInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	char szMaxValue[MINMAX_VALUE_LEN];     ///< Integer of maximum value.
	ps::lib::sql::ind_t nMaxValueInd;      ///< ind_t::VAL_IS_NULL: value is NULL
	sb4 lIncrementBy;                      ///< Integer of increment value.
	char szCycleFlag[CYCLEFLG_LEN];        ///< Flag of cyclic numbering.
    ps::lib::sql::ind_t nCycleFlagInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	char szOrderFlag[CYCLEFLG_LEN];        ///< Flag of originate the number in the order requested.
    ps::lib::sql::ind_t nOrderFlagInd;     ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 lCacheSize;                        ///< Amount of numbers to be stored before supply.
	char szLastNumber[MINMAX_VALUE_LEN];   ///< Last number.
    /**
     * @brief
     * Call this before executing iKeyMatched. Implicit calls are dared to be permitted
     * @param [in] oKey
     */
    explicit tAttributes(const tKeyTuple& oKey)
        : nMinValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nMaxValueInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , lIncrementBy(0)
        , nCycleFlagInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nOrderFlagInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szSequenceName, 0, sizeof(szSequenceName));
        ::memset(szMinValue, 0, sizeof(szMinValue));
        ::memset(szMaxValue, 0, sizeof(szMaxValue));
        ::memset(szCycleFlag, 0, sizeof(szCycleFlag));
        ::memset(szOrderFlag, 0, sizeof(szOrderFlag));
        ::memset(szLastNumber, 0, sizeof(szLastNumber));
        ::strcpy(szOwner, std::get<0>(oKey));
    }
    /**
     * @brief
     * It is used when new tAttributes[] is called 
     * to generate raw array of tAttributes type.
     */
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /**
     * @brief
     * defines criteria when the collection of this structure is sorted.
     * @return true will be returned when lhs < rhs.
     * @param [in] rhs
     */
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szSequenceName, rhs.szSequenceName
        );
    }
    /**
     * @brief
     * Define the key for selecting the line.
     * @return true will be returned when lhs == rhs.
     * @param [in] rhs 
     *     must be initialized by the CTOR
     *     which takes tKeyTuple in advance.
     */
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
        );
    }
    /**
     * @brief
     * Assemble a part of the statement and flow it to the sink.
     * @param [in,out] os
     *     A sink of processed data.
     * @param [in] i
     *     Indicates the index of the elements in which the key matched.
     *     The first number is 0.
     * @param [in] oCtx
     *     Use this when you want to manipulate or refer to
     *     a member of a class that has @ref oList_.
     */
    void vPrinter(std::ostream& os, const size_t i, cAllSequences& oCtx) const
    {
        using ps::lib::iCascadeBinaryOr;
        using ps::lib::iCompBinary;
        using ps::lib::sMakeEnclosedName;
        std::string min_value_clause;
        std::string max_value_clause;
        std::string cache_size_clause;

        if (iCascadeBinaryOr(
            strcmp, szMinValue
            /* ascend */ , "1"
            /* descend */, "-99999999999999999999999999"
        )){
            min_value_clause = "nominvalue";
        }
        else
        {
            min_value_clause = (boost::format("minvalue %s") % szMinValue).str();
        }

        if (iCascadeBinaryOr(
            strcmp, szMaxValue
            /* ascend */ , "999999999999999999999999999"
            /* descend */,"-1"
        )){
            max_value_clause = "nomaxvalue";
        }
        else
        {
            max_value_clause = (boost::format("maxvalue %s") % szMaxValue).str();
        }

        if (lCacheSize > 0)
        {
            cache_size_clause = (boost::format("cache %d") % lCacheSize).str();
        }
        else
        {
            cache_size_clause = "nocache";
        }

        const std::string f_cycle = (iCompBinary(strcmp, szCycleFlag, "Y") ? "cycle" : "nocycle");
        const std::string f_order = (iCompBinary(strcmp, szOrderFlag, "Y") ? "order" : "noorder");
        os  << sPrintLabel("Seqs", i + 1, szSequenceName)
            << boost::format("drop sequence %s;") % sMakeEnclosedName(szSequenceName) << std::endl
            << boost::format("create sequence %s") % sMakeEnclosedName(szSequenceName) << std::endl
            << boost::format("  %s") % min_value_clause << std::endl
            << boost::format("  %s") % max_value_clause << std::endl
            << boost::format("  increment by %d") % lIncrementBy << std::endl
            << boost::format("  %s") % f_cycle << std::endl
            << boost::format("  %s") % f_order << std::endl
            << boost::format("  %s") % cache_size_clause << std::endl
            << boost::format("  start with %s") % szLastNumber << std::endl
            << std::string(";") << std::endl
            << oPrintItems(oCtx.oAllTabPrivs_,  std::make_tuple(szOwner, szSequenceName), true)
            << oPrintItems(oCtx.oUserObjAuditOpts_,  std::make_tuple(szOwner, szSequenceName), true)
            << oPrintItems(oCtx.oUserAuditObject_,  std::make_tuple(szOwner, szSequenceName), true)
            ;
    }
    /**
     * @details
     * - can be used to show the value of the member in the tAttributes
     *    into a human-readable format and return it as a string.
     * @par Example way to be used (on the console of the GNU gdb):
     * @code
        // usually following comannd is used when you want to dump the values of member.
        print *this;
        // Instead, try to use sDebugPrint() to dump it more pretty.
       call this->sDebugPrint();
       @endcode
     * @note To use this in gdb, don't define as inline it.
     */
    std::string sDebugPrint() const;
};

std::string cAllSequences::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szSequenceName='%s')"
            R"(, szMinValue='%s', szMaxValue='%s')"
            R"(, lIncrementBy=%d, szCycleFlag='%s', szOrderFlag='%s')"
            R"(, lCacheSize=%d, szLastNumber='%s')"
        ) % szOwner % szSequenceName
            % szMinValue % szMaxValue
            % lIncrementBy % szCycleFlag % szOrderFlag
            % lCacheSize % szLastNumber
        ;
    return oss.str();
}

/**
 * @struct cAllSequences::cRetriever
 */
class cAllSequences::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @brief Number of lines per fetch operation.
    /// @brief A raw array allocated the size of @ref iBulkSize_ * sizeof(@ref tAttributes).
    static tAttributes rTable_[];
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllSequences::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @brief is not used in this class.
    /**
     * @brief
     * is used to bind the address of each member variable of the first array element
     *  contained in @ref rTable_ to each column of the SQL select statement.
     */
    ps::lib::sql::occi::cDefine& oDefine_;
    const size_t iSkip_;               ///< @brief Alias of sizeof(@ref tAttributes).
    /// number of elements in oList_.
    size_t& iNumRows_;
public:
    cRetriever(
        ps::lib::cList<tAttributes>& oList
        , ps::lib::sql::occi::cSvc& oSvc
        , const ps::lib::str_vct& oOwners
        , size_t& iNumRows
    );
private:
    /**
     * @brief
     * Operations repeatedly executed befor each fetch.
     * - Clear the raw array (@ref rTable_) of tAttributes type with 0.
     */
    virtual void vPreBulkAction(const uint32_t& iBulkSize);
    /**
     * @brief
     * Operations repeatedly executed after each fetch.
     * - All elements of raw array (@ref rTable_) of type tAttributes adds to @ref oList_.
     * - Furthermore, the statistical counter related to the number of bytes
     *   and the number of lines of target data of the fetched row is updated.
     */
    virtual void vPostBulkAction(const uint32_t& iNumIter);
    /**
     * @brief
     * Operation to be performed after all fetches are completed.
     * - All elements of tAttributes type contained in the container are sorted.
     */
    virtual void vPostRepeatAction();
};

const uint32_t cAllSequences::cRetriever::iBulkSize_ = 100;

cAllSequences::tAttributes cAllSequences::cRetriever::rTable_[cAllSequences::cRetriever::iBulkSize_];

cAllSequences::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , szAllSequences
        , "Reading sequences", &oBind_
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
    oDefine_.vAddItem(rTable_->szSequenceName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szMinValue, SQLT_STR, &rTable_->nMinValueInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szMaxValue, SQLT_STR, &rTable_->nMaxValueInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->lIncrementBy, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCycleFlag, SQLT_STR, &rTable_->nCycleFlagInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szOrderFlag, SQLT_STR, &rTable_->nOrderFlagInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->lCacheSize, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLastNumber, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cAllSequences::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllSequences::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllSequences::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cAllSequences::szAllSequences[] = {
"select sequence_owner"
", sequence_name"
", min_value"
", max_value"
", increment_by"
", cycle_flag"
", order_flag"
", cache_size"
", last_number "
"from all_sequences "
"where sequence_owner in %s "
};

cAllSequences::cAllSequences(
    cAllTabPrivs& oAllTabPrivs
    , cUserObjAuditOpts& oUserObjAuditOpts
    , cUserAuditObject& oUserAuditObject
)
    : iNumRows_(0)
    , oAllTabPrivs_(oAllTabPrivs)
    , oUserObjAuditOpts_(oUserObjAuditOpts)
    , oUserAuditObject_(oUserAuditObject)
{
    // nothing to do
}

cAllSequences::~cAllSequences()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllSequences::oSubmit(
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

void cAllSequences::vPrintAllItems(
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
        , &cAllSequences::tAttributes::iKeyMatched
        , &cAllSequences::tAttributes::vPrinter
        , &cAllSequences::vPrintPreRows
        , &cAllSequences::vPrintPostRows
        , &cAllSequences::vPrintNotFound
        , std::back_inserter(oArr)
    );
    if (iDispose)
    {
        boost::for_each(
            oArr, [this](tIter it){oList_.erase(it);}
        );
    }
}

void cAllSequences::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    // nothing to do
}

void cAllSequences::vPrintPostRows(std::ostream&, const tAttributes&) const
{
    // nothing to do
}

void cAllSequences::vPrintNotFound(std::ostream&, const tAttributes&) const
{
    // nothing to do
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

