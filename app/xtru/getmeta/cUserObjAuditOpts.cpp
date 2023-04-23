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

#define AUD_OPTS_LEN           3+1

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

struct cUserObjAuditOpts::tAttributes
{
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner
	char szObjectName[OBJECT_NAME_LEN];    ///< Name of object
	char szObjectType[OBJECT_TYPE_LEN];    ///< Name of object type
	char szAlt[AUD_OPTS_LEN];              ///< Setting of object audit about ALTER
	char szAud[AUD_OPTS_LEN];              ///< Similar to above
	char szCom[AUD_OPTS_LEN];              ///< Similar to above
	char szDel[AUD_OPTS_LEN];              ///< Similar to above
	char szGra[AUD_OPTS_LEN];              ///< Similar to above
	char szInd[AUD_OPTS_LEN];              ///< Similar to above
	char szIns[AUD_OPTS_LEN];              ///< Similar to above
	char szLoc[AUD_OPTS_LEN];              ///< Similar to above
	char szRen[AUD_OPTS_LEN];              ///< Similar to above
	char szSel[AUD_OPTS_LEN];              ///< Similar to above
	char szUpd[AUD_OPTS_LEN];              ///< Similar to above
	char szRef[AUD_OPTS_LEN];              ///< Similar to above
	char szExe[AUD_OPTS_LEN];              ///< Similar to above
	char szCre[AUD_OPTS_LEN];              ///< Similar to above
	char szRea[AUD_OPTS_LEN];              ///< Similar to above
	char szWri[AUD_OPTS_LEN];              ///< Similar to above (featured >=10gR1)
	char szFbk[AUD_OPTS_LEN];              ///< Similar to above (featured >=10gR1)
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szObjectName, 0, sizeof(szObjectName));
        ::memset(szObjectType, 0, sizeof(szObjectType));
        ::memset(szAlt, 0, sizeof(szAlt));
        ::memset(szAud, 0, sizeof(szAud));
        ::memset(szCom, 0, sizeof(szCom));
        ::memset(szDel, 0, sizeof(szDel));
        ::memset(szGra, 0, sizeof(szGra));
        ::memset(szInd, 0, sizeof(szInd));
        ::memset(szIns, 0, sizeof(szIns));
        ::memset(szLoc, 0, sizeof(szLoc));
        ::memset(szRen, 0, sizeof(szRen));
        ::memset(szSel, 0, sizeof(szSel));
        ::memset(szUpd, 0, sizeof(szUpd));
        ::memset(szRef, 0, sizeof(szRef));
        ::memset(szExe, 0, sizeof(szExe));
        ::memset(szCre, 0, sizeof(szCre));
        ::memset(szRea, 0, sizeof(szRea));
        ::memset(szWri, 0, sizeof(szWri));
        ::memset(szFbk, 0, sizeof(szFbk));
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
            , szObjectType, rhs.szObjectType
            , szAlt, rhs.szAlt
            , szAud, rhs.szAud
            , szCom, rhs.szCom
            , szDel, rhs.szDel
            , szGra, rhs.szGra
            , szInd, rhs.szInd
            , szIns, rhs.szIns
            , szLoc, rhs.szLoc
            , szRen, rhs.szRen
            , szSel, rhs.szSel
            , szUpd, rhs.szUpd
            , szRef, rhs.szRef
            , szExe, rhs.szExe
            , szCre, rhs.szCre
            , szRea, rhs.szRea
            , szWri, rhs.szWri
            , szFbk, rhs.szFbk
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
    void vPrtObjAuditStmt(std::ostream& os, const std::string& act_name, const std::string& act_als) const
    {
        const auto cls0 = act_als[0];
        const auto cls2 = act_als[2];
        if (cls0 != '-')
        {
            os << boost::format(R"(audit %s on "%s" %s whenever successful;)")
                % act_name % szObjectName % (cls0 == 'S' ? "by session" : "by access")
            << std::endl;
        }
        if (cls2 != '-')
        {
            os << boost::format(R"(audit %s on "%s" %s whenever not successful;)")
                % act_name % szObjectName % (cls2 == 'S' ? "by session" : "by access")
            << std::endl;
        }
    }
    /// @copydoc cAllSequences::tAttributes::vPrinter()
    void vPrinter(std::ostream& os, const size_t& i, cUserObjAuditOpts& oCtx) const
    {
        vPrtObjAuditStmt(os, "ALTER", szAlt);
        vPrtObjAuditStmt(os, "AUDIT", szAud);
        vPrtObjAuditStmt(os, "COMMENT", szCom);
        vPrtObjAuditStmt(os, "DELETE", szDel);
        vPrtObjAuditStmt(os, "GRANT", szGra);
        vPrtObjAuditStmt(os, "INDEX", szInd);
        vPrtObjAuditStmt(os, "INSERT", szIns);
        vPrtObjAuditStmt(os, "LOCK", szLoc);
        vPrtObjAuditStmt(os, "RENAME", szRen);
        vPrtObjAuditStmt(os, "SELECT", szSel);
        vPrtObjAuditStmt(os, "UPDATE", szUpd);
        vPrtObjAuditStmt(os, "REFERENCE", szRef);  /// Obsoleted
        vPrtObjAuditStmt(os, "EXECUTE", szExe);
        vPrtObjAuditStmt(os, "CREATE", szCre);
        vPrtObjAuditStmt(os, "READ", szRea);
        vPrtObjAuditStmt(os, "WRITE", szWri);
        vPrtObjAuditStmt(os, "FLASHBACK", szFbk);
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cUserObjAuditOpts::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
            R"(szOwner='%s', szObjectName='%s', szObjectType='%s')"
            R"(, szAlt='%s', szAud='%s', szCom='%s', szDel='%s')"
            R"(, szGra='%s', szInd='%s', szIns='%s', szLoc='%s')"
            R"(, szRen='%s', szSel='%s', szUpd='%s', szRef='%s')"
            R"(, szExe='%s', szCre='%s', szRea='%s', szWri='%s', szFbk='%s')"
        ) % szOwner % szObjectName % szObjectType
            % szAlt % szAud % szCom % szDel
            % szGra % szInd % szIns % szLoc
            % szRen % szSel % szUpd % szRef
            % szExe % szCre % szRea % szWri % szFbk
        ;
    return oss.str();
}

/**
 * @struct cUserObjAuditOpts::cRetriever
 */
class cUserObjAuditOpts::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cUserObjAuditOpts::oList_.
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
    bool iAccesible() const
    {
        const auto& oPrivsGiven(ps::app::xtru::getmeta::cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_OBJ_AUDIT_OPTS", "SELECT"});
    }
};

const uint32_t cUserObjAuditOpts::cRetriever::iBulkSize_ = 100;

cUserObjAuditOpts::tAttributes cUserObjAuditOpts::cRetriever::rTable_[cUserObjAuditOpts::cRetriever::iBulkSize_];

cUserObjAuditOpts::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , (iAccesible() ? szDbaObjAuditOpts : szUserObjAuditOpts)
        , "Reading Object audit options", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    const auto later_10g1 = oSvc.iCompDbVersion("1001",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({
        later_10g1 ? sz10g1Spec1 : ", 'n/a' as wri , 'n/a' as fbk"
        , sGetSqlInList(oOwners)
        , later_10g1 ? sz10g1Spec2 : ""
    });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectName, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szObjectType, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szAlt, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szAud, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCom, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szDel, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szGra, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szInd, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szIns, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szLoc, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRen, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szSel, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szUpd, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRef, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szExe, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szCre, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szRea, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szWri, SQLT_STR, NULL, NULL, NULL, iSkip_);
    oDefine_.vAddItem(rTable_->szFbk, SQLT_STR, NULL, NULL, NULL, iSkip_);
}

void cUserObjAuditOpts::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cUserObjAuditOpts::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cUserObjAuditOpts::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cUserObjAuditOpts::szDbaObjAuditOpts[] = {
"select owner, object_name, object_type "
", alt, aud, com, del "
", gra, ind, ins, loc "
", ren, sel, upd, ref "
", exe, cre, rea %s "
"from dba_obj_audit_opts "
"where owner in %s "
"and ("
	"alt != '-/-' or aud != '-/-' or com != '-/-' or del != '-/-' or "
	"gra != '-/-' or ind != '-/-' or ins != '-/-' or loc != '-/-' or "
	"ren != '-/-' or sel != '-/-' or upd != '-/-' or ref != '-/-' or "
	"exe != '-/-' or cre != '-/-' or rea != '-/-' %s "
")"
};

const char cUserObjAuditOpts::szUserObjAuditOpts[] = {
"select user, object_name, object_type "
", alt, aud, com, del "
", gra, ind, ins, loc "
", ren, sel, upd, ref "
", exe, cre, rea %s "
""
"from user_obj_audit_opts "
"where user in %s "
"and ("
	"alt != '-/-' or aud != '-/-' or com != '-/-' or del != '-/-' or "
	"gra != '-/-' or ind != '-/-' or ins != '-/-' or loc != '-/-' or "
	"ren != '-/-' or sel != '-/-' or upd != '-/-' or ref != '-/-' or "
	"exe != '-/-' or cre != '-/-' or rea != '-/-' %s "
")"
};

const char cUserObjAuditOpts::sz10g1Spec1[] = { ", wri, fbk " };
const char cUserObjAuditOpts::sz10g1Spec2[] = { " or wri != '-/-' or fbk != '-/-' " };

cUserObjAuditOpts::cUserObjAuditOpts()
{
    // nothing to do
}

cUserObjAuditOpts::~cUserObjAuditOpts()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cUserObjAuditOpts::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cUserObjAuditOpts::vPrintAllItems(
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
        , &cUserObjAuditOpts::tAttributes::iKeyMatched
        , &cUserObjAuditOpts::tAttributes::vPrinter
        , &cUserObjAuditOpts::vPrintPreRows
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

void cUserObjAuditOpts::vPrintPreRows(std::ostream& os, const tAttributes& rKey) const
{
    os << boost::format(R"(noaudit all on "%s";)") % rKey.szObjectName << std::endl;
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

