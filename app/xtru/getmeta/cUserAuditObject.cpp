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

#define TIMESTAMP_LEN     19+1
#define AUDIT_ACTION_LEN  28+1
#define PRIV_USED_LEN     48+1
#define OS_USER_NAME_LEN  28+1
#define USER_HOST_LEN     40+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

struct cUserAuditObject::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];        ///< Name of owner
	char szObjectName[OBJECT_NAME_LEN];   ///< Name of object
	ps::lib::sql::ind_t nObjectNameInd;
	int32_t iTypeIdx;                     ///< Number of object type.
	char szOName[OBJECT_NAME_LEN];        ///< Name of object to be audited.
	ps::lib::sql::ind_t nONameInd;
	char szTimestamp[TIMESTAMP_LEN];      ///< Timestamp on created.
	ub4 iSessionId;                       ///< ID of audit session.
	ub4 iEntryId;                         ///< The order of audit actions performed
                                          ///< During one session.
	char szActionName[AUDIT_ACTION_LEN];  ///< Name of action.
	int32_t iReturnCode;                  ///< 0: operation is succeeded.
	char szUserName[OBJECT_NAME_LEN];     ///< The Oracle user who performed the action.
	char szPrivUsed[PRIV_USED_LEN];       ///< Privilege exercised.
	ps::lib::sql::ind_t nPrivUsedInd;
	char szOsUserName[OS_USER_NAME_LEN];  ///< Name of OS acount.
	ps::lib::sql::ind_t nOsUserNameInd;
	char szUserHost[USER_HOST_LEN];       ///< ID of terminal.
	ps::lib::sql::ind_t nUserHostInd;
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : nObjectNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iTypeIdx(0)
        , nONameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , iSessionId(0)
        , iEntryId(0)
        , iReturnCode(0)
        , nPrivUsedInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nOsUserNameInd(ps::lib::sql::ind_t::VAL_IS_NULL)
        , nUserHostInd(ps::lib::sql::ind_t::VAL_IS_NULL)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szObjectName, 0, sizeof(szObjectName));
        ::memset(szOName, 0, sizeof(szOName));
        ::memset(szTimestamp, 0, sizeof(szTimestamp));
        ::memset(szActionName, 0, sizeof(szActionName));
        ::memset(szUserName, 0, sizeof(szUserName));
        ::memset(szPrivUsed, 0, sizeof(szPrivUsed));
        ::memset(szOsUserName, 0, sizeof(szOsUserName));
        ::memset(szUserHost, 0, sizeof(szUserHost));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szObjectName, std::get<1>(oKey));
    }
    /// @copydoc cAllSequences::tAttributes::tAttributes()
    tAttributes()
        : tAttributes(std::make_tuple("", ""))
    {}
    /// @copydoc cAllSequences::tAttributes::operator<()
    bool operator<(const tAttributes& rhs) const
    {
        return ps::lib::composite_less(
            szOwner, rhs.szOwner
            , szObjectName, rhs.szObjectName
            , szTimestamp, rhs.szTimestamp
            , iTypeIdx, rhs.iTypeIdx
            , szOName, rhs.szOName
            , iSessionId, rhs.iSessionId
            , iEntryId, rhs.iEntryId
        );
    }
    /// @copydoc cAllSequences::tAttributes::iKeyMatched()
    bool iKeyMatched(const tAttributes& rhs) const
    {
        return ps::lib::composite_equal(
            szOwner, rhs.szOwner
            , szObjectName, rhs.szObjectName
        );
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserAuditObject& oCtx) const
    {
        os << boost::format(
                R"(%s, %u, %u, %s, %s, %s, %s, %s, %d, %s)"
            ) % szTimestamp % iSessionId % iEntryId
            % szActionName % szObjectName % szOsUserName
            % szUserHost % szUserName % iReturnCode % szPrivUsed
            << std::endl
        ; 
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cUserAuditObject::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szObjectName='%s'(%d))"
            R"(, iTypeIdx=%d, szOName='%s'(%d), szTimestamp='%s')"
            R"(, iSessionId=%u, iEntryId=%u, szActionName='%s')"
            R"(, iReturnCode=%d, szUserName='%s')"
            R"(, szPrivUsed='%s'(%d), szOsUserName='%s'(%d))"
            R"(, szUserHost='%s'(%d))"
        ) % szOwner % szObjectName % nObjectNameInd 
            % iTypeIdx % szOName % nONameInd % szTimestamp
            % iSessionId % iEntryId % szActionName
            % iReturnCode % szUserName
            % szPrivUsed % nPrivUsedInd
            % szOsUserName % nOsUserNameInd
            % szUserHost % nUserHostInd
        ;
    return oss.str();
}

/**
 * @struct cUserObjAuditOpts::cRetriever
 */
class cUserAuditObject::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserAuditObject::oList_.
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
        , const int32_t& iNumdays
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
        const auto& oPrivsGiven(cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_AUDIT_OBJECT", "SELECT"});
    }
};

const uint32_t cUserAuditObject::cRetriever::iBulkSize_ = 1000;

cUserAuditObject::tAttributes cUserAuditObject::cRetriever::rTable_[cUserAuditObject::cRetriever::iBulkSize_];

cUserAuditObject::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const int32_t& iNumdays
)
    : cDescriber(oSvc, iBulkSize_
        , szUserAuditObject
        , "Reading Object audit trails", &oBind_
        , (0 > iNumdays)  // If true is passed, executing and fetching will be canceled.
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        boost::lexical_cast<std::string>(OS_USER_NAME_LEN - 1)
        , boost::lexical_cast<std::string>(USER_HOST_LEN - 1)
        , (iAccesible() ? "dba" : "user")
        , sGetSqlInList(oOwners)
    });
    // Outbounding data to Oracle.
    oBind_.vAddItem(":b_numdays", &iNumdays);
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, &rTable_->nObjectNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iTypeIdx, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szOName, SQLT_STR, &rTable_->nONameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szTimestamp, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iSessionId, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iEntryId, SQLT_UIN, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szActionName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->iReturnCode, SQLT_INT, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUserName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szPrivUsed, SQLT_STR, &rTable_->nPrivUsedInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szOsUserName, SQLT_STR, &rTable_->nOsUserNameInd, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUserHost, SQLT_STR, &rTable_->nUserHostInd, NULL, NULL, iSkip_);
}

void cUserAuditObject::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserAuditObject::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserAuditObject::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cUserAuditObject::szUserAuditObject[] = {
"select "
"t1.owner "
", t1.obj_name "
", decode(substr(t1.action_name, instr(t1.action_name,' ')+1) "
", 'DATABASE LINK', 1 "
", 'SEQUENCE', 2 "
", 'CLUSTER', 3 "
", 'TABLE', 4 "
", 'INDEX', 4 "
", 'TRIGGER', 4 "
", 'MATERIALIZED VIEW', 6 "
", 'VIEW', 7 "
", 'TYPE', 8 "
", 'PACKAGE', 8 "
", 'FUNCTION', 9 "
", 'PROCEDURE', 9 "
", 'TYPE BODY', 10 "
", 'PACKAGE BODY', 10 "
", 'SYNONYM', 15 "
", 20 "
") as type_idx "
/* When create index is executed, table name is stored in new_name */
", nvl(t1.new_name, t1.obj_name) as oname "
", to_char(t1.timestamp, 'yyyy_mm_dd_hh24_mi_ss') as timestamp "
", t1.sessionid "
", t1.entryid "
", t1.action_name "
", t1.returncode "
", t1.username "
", t1.priv_used "
", substrb(t1.os_username,1,%s) as os_username "
", substrb(nvl(t1.userhost,t1.terminal),1,%s) as userhost "
"from %s_audit_object t1 "
"where t1.owner in %s "
"and t1.timestamp > trunc(sysdate) - :b_numdays "
};

cUserAuditObject::cUserAuditObject()
{
    // nothing to do
}

cUserAuditObject::~cUserAuditObject()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserAuditObject::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
    , const int32_t& iNumdays
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
        , iNumdays
    );
}

void cUserAuditObject::vPrintAllItems(
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
        , &cUserAuditObject::tAttributes::iKeyMatched
        , &cUserAuditObject::tAttributes::vPrinter
        , &cUserAuditObject::vPrintPreRows
        , &cUserAuditObject::vPrintPostRows
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

void cUserAuditObject::vPrintPreRows(std::ostream& os, const tAttributes&) const
{
    os  << std::string("/* The audit trails of the target actions.") << std::endl
        << std::string("Dtm, Ses, Ent, Act, Own, Obj, OS_acnt, Host, Exec, ORA-xxxxx, Priv") << std::endl
        ;
}

void cUserAuditObject::vPrintPostRows(std::ostream& os, const tAttributes&) const
{
    os  << std::string(R"(*/)")
        << std::endl
        ;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

