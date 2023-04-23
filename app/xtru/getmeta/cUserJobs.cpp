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

#define NEXT_DATE_LEN  24+1
#define BROKEN_LEN      1+1
#define INTERVAL_LEN  200+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/** 
 * @struct cUserJobs::tAttributes
 * See to study about [DBA_JOBS and DBA_REFRESH]
 * (https://docs.oracle.com/database/121/REFRN/GUID-278CEBB4-5095-4FEA-AE52-1254F7EE361F.htm#REFRN23105)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cUserJobs::tAttributes
{
	char szSchemaUser[OBJECT_NAME_LEN];    ///< Default schema used to parse the job
	char szObjectName[OBJECT_NAME_LEN];    ///< Identifier of job
	char szNextDate[NEXT_DATE_LEN];        ///< Date that this job will next be executed
	char szBroken[BROKEN_LEN];             ///< Y: no attempt is made to run this job
	char szInterval[INTERVAL_LEN];         ///< A date function, evaluated at the start of execution
	char *szWhat;                          ///< Body of the anonymous PL/SQL block that the job executes
	ps::lib::sql::ind_t nWhatInd;          ///< ind_t::VAL_IS_NULL: value is NULL
	ub4 iWhat;                             ///< length of the comment body.
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : szWhat(nullptr)
        , nWhatInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iWhat(0)
    {
        ::memset(szSchemaUser, 0, sizeof(szSchemaUser));
        ::memset(szObjectName, 0, sizeof(szObjectName));
        ::memset(szNextDate, 0, sizeof(szNextDate));
        ::memset(szBroken, 0, sizeof(szBroken));
        ::memset(szInterval, 0, sizeof(szInterval));
        ::strcpy(szSchemaUser, std::get<0>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple(""))
    {}
    /**
     * @brief
     * Do not use the default copy constructor.
     * Since tAttributes::szWhat which is member is only
     * pointing to the area allocated by the new[] operator.
     */
    tAttributes(const tAttributes& rhs)
        : szWhat(new char[rhs.iWhat + 1])
        , nWhatInd(rhs.nWhatInd)
        , iWhat(rhs.iWhat)
    {
        ::memset(szSchemaUser, 0, sizeof(szSchemaUser));
        ::memset(szObjectName, 0, sizeof(szObjectName));
        ::memset(szNextDate, 0, sizeof(szNextDate));
        ::memset(szBroken, 0, sizeof(szBroken));
        ::memset(szInterval, 0, sizeof(szInterval));
        ::strcpy(szSchemaUser, rhs.szSchemaUser);
        ::strcpy(szObjectName, rhs.szObjectName);
        ::strcpy(szNextDate, rhs.szNextDate);
        ::strcpy(szBroken, rhs.szBroken);
        ::strcpy(szInterval, rhs.szInterval);
        ::strcpy(szWhat, rhs.szWhat);
    }
    ~tAttributes()
    {
        delete [] szWhat;
    }
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szSchemaUser, rhs.szSchemaUser
            , szObjectName, rhs.szObjectName
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szSchemaUser, rhs.szSchemaUser
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserJobs& oCtx) const;
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

void cUserJobs::tAttributes::vPrinter(std::ostream& os, const size_t& i, cUserJobs& oCtx) const
{
    const std::tuple<const char*, const char*> oKey(szSchemaUser, szObjectName);
    // Convert the end-of-line sequence used by Windows to xUNIX.
    std::string sWork =
        ps::lib::sConvertEolSequence(szWhat, "\r\n", "\n");
    // Remove any characters in specific if they are used at the end of the string.
    ps::lib::vRmSpecCharsAtEofStr(sWork, " \r\n/");
    oss_.str("");
    if (ps::lib::iCompBinary(strcmp, szInterval, "null"))
    {
        oss_ << std::string("null");
    }
    else
    {
        oss_ << boost::format("'%s'") % szInterval;
    }
    os << sPrintLabel("Jobs", i + 1, std::string(szObjectName))
        << std::string("declare") << std::endl
        << std::string("  job_no binary_integer;") << std::endl
        << std::string("begin") << std::endl
        << std::string("  dbms_job.submit") << std::endl
        << std::string("  (  job       => job_no") << std::endl
        << boost::format("  ,  what      => '%s'")
            % sWork
        << std::endl
        << boost::format("  ,  next_date => to_date('%s','%s')")
            % szNextDate % oCtx.sDateMask_
        << std::endl
        << boost::format("  ,  interval  => %s")
            % oss_.str()
        << std::endl
        << std::string("  );") << std::endl
        << std::string("end;") << std::endl
        << std::string("/") << std::endl
       ;
}

std::string cUserJobs::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szSchemaUser='%s', szObjectName='%s', szNextDate='%s')"
        R"(, szBroken='%s', szInterval='%s', iWhat=%u, szWhat='%s')"
    ) % szSchemaUser % szObjectName % szNextDate
        % szBroken % szInterval % iWhat % szWhat
    ;
    return oss.str();
}

/**
 * @struct cUserJobs::cRetriever
 */
class cUserJobs::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserJobs::oList_.
    ps::lib::sql::occi::cBind oBind_;  ///< @copybrief cAllSequences::cRetriever::oBind_
    /// @copybrief cAllSequences::cRetriever::oDefine_
    ps::lib::sql::occi::cDefine& oDefine_;
    /// @copybrief cAllSequences::cRetriever::iSkip_
    const size_t iSkip_;
    size_t& iNumRows_;
    ps::lib::sql::occi::cPieceVct pv_;
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
    bool iAccesible() const
    {
        const auto& oPrivsGiven(ps::app::xtru::getmeta::cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_JOBS", "SELECT"})
            && oPrivsGiven.iAccesible({"SYS", "DBA_REFRESH", "SELECT"})
        ;
    }
};

const uint32_t cUserJobs::cRetriever::iBulkSize_ = 100;

cUserJobs::tAttributes cUserJobs::cRetriever::rTable_[cUserJobs::cRetriever::iBulkSize_];

cUserJobs::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , size_t& iNumRows
)
    : cDescriber(oSvc, iBulkSize_
        , iAccesible() ? szDbaJobs : szUserJobs
        , "Reading information of the job", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())
    , iSkip_(sizeof(tAttributes))
    , iNumRows_(iNumRows)
    , pv_(iSkip_, &rTable_->szWhat, &rTable_->nWhatInd, &rTable_->iWhat)
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners) });
    // Inbounding data from Oracle.
    oDefine_.vSetTiming(ps::lib::sql::occi::cDefine::tTiming::iOnce); // NOTE: default is iRepeat
    oDefine_.vAddItem(rTable_->szSchemaUser, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szNextDate, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szBroken, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInterval, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(SB4MAXVAL, SQLT_CHR, OCI_DYNAMIC_FETCH
        , ps::lib::sql::occi::cPieceVct::iCbkFunc, (void*) &pv_
    );
}

void cUserJobs::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserJobs::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    ps::lib::sql::occi::cPieceVct::vTerminateLatest(&pv_, iNumIter);
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserJobs::cRetriever::vPostRepeatAction()
{
    oList_.sort();
    iNumRows_ = oList_.size();
}

const char cUserJobs::szDbaJobs[] = {
"select schema_user "
", 'JOB_' || to_char(job, 'fm0000000000') as job "
", next_date "
", broken "
", replace(interval,'''','''''') as interval "
", replace(what,'''','''''') as what "
"from dba_jobs "
"where schema_user in %s "
  "and not exists ( "
  "select null "
  "from dba_refresh "
  "where job = dba_jobs.job "
  "and rowner = schema_user "
") "
};

const char cUserJobs::szUserJobs[] = {
"select schema_user "
", 'JOB_' || to_char(job, 'fm0000000000') as job "
", next_date "
", broken "
", replace(interval,'''','''''') as interval "
", replace(what,'''','''''') as what "
"from user_jobs "
"where schema_user in %s "
  "and not exists ( "
  "select null "
  "from user_refresh "
  "where job = user_jobs.job "
  "and rowner = schema_user "
") "
};

cUserJobs::cUserJobs(
    const std::string& sDateMask
)
    : iNumRows_(0)
    , sDateMask_(sDateMask)
{
    // nothing to do
}

cUserJobs::~cUserJobs()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserJobs::oSubmit(
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

void cUserJobs::vPrintAllItems(
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
        , &cUserJobs::tAttributes::iKeyMatched
        , &cUserJobs::tAttributes::vPrinter
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
