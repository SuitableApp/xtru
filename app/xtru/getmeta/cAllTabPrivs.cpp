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

constexpr size_t PRIVILEGE_NAME_LEN = 40+1; /* Max Length of privilege name */
constexpr size_t GRANTABLE_LEN = 3+1;

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cAllTabPrivs::tAttributes
 * See to study about [ALL_TAB_PRIVS]
 * (https://docs.oracle.com/database/121/REFRN/GUID-F3265681-6114-45C2-BE79-99C8D0DD517E.htm#REFRN20282)
 * (Oracle Database Online Documentation 12c Release 1 (12.1): Database Reference:).
 */
struct cAllTabPrivs::tAttributes
{
	int32_t iTypeIdx;                      ///< Index of the object type
	char szOwner[OBJECT_NAME_LEN];         ///< Name of object owner.
	char szTableName[OBJECT_NAME_LEN];     ///< Name of object
	char szPrivilege[PRIVILEGE_NAME_LEN];  ///< Granted privilege
	char szGrantee[OBJECT_NAME_LEN];       ///< Name of the user who is granted privileges.
	char szGrantable[GRANTABLE_LEN];       ///< Wheather WITH GRANT OPTION is given (YES or NO)
    /// @copydoc cAllSequences::tAttributes::tAttributes(const tKeyTuple&)
    explicit tAttributes(const tKeyTuple& oKey)
        : iTypeIdx(0)
    {
        ::memset(szOwner, 0, sizeof(szOwner));
        ::memset(szTableName, 0, sizeof(szTableName));
        ::memset(szPrivilege, 0, sizeof(szPrivilege));
        ::memset(szGrantee, 0, sizeof(szGrantee));
        ::memset(szGrantable, 0, sizeof(szGrantable));
        ::strcpy(szOwner, std::get<0>(oKey));
        ::strcpy(szTableName, std::get<1>(oKey));
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
            , szTableName, rhs.szTableName
            , szPrivilege, rhs.szPrivilege
            , szGrantee, rhs.szGrantee
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
    void vPrinter(std::ostream& os, const size_t& i, cAllTabPrivs& oCtx) const
    {
        os << boost::format(R"(grant %s on "%s" to "%s"%s;)")
                % szPrivilege
                % szTableName
                % szGrantee
                % (ps::lib::iCompBinary(strcmp, szGrantable,"YES") ? " with grant option" : "")
            << std::endl;
    }
    /**
     * @copydetails cAllSequences::tAttributes::sDebugPrint()
     */
    std::string sDebugPrint() const;
};

std::string cAllTabPrivs::tAttributes::sDebugPrint() const
{
    std::stringstream oss;
    oss << boost::format(
        R"(szOwner='%s', szTableName='%s', iTypeIdx=%d)"
        R"(, szPrivilege='%s', szGrantee='%s', szGrantable='%s')"
    ) % szOwner % szTableName % iTypeIdx
        % szPrivilege % szGrantee % szGrantable
    ;
    return oss.str();
}

/**
 * @struct cAllTabPrivs::cRetriever
 */
class cAllTabPrivs::cRetriever
    : public ps::app::xtru::getmeta::cDescriber
{
private:
    static const uint32_t iBulkSize_;  ///< @copybrief cAllSequences::cRetriever::iBulkSize_
    static tAttributes rTable_[];      ///< @copybrief cAllSequences::cRetriever::rTable_
    ps::lib::cList<tAttributes>& oList_; ///< is an alias of cAllTabPrivs::oList_.
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
        const auto& oPrivsGiven(cPrivsGiven::get_const_instance());
        return oPrivsGiven.iAccesible({"SYS", "DBA_TAB_PRIVS", "SELECT"});
    }
};

const uint32_t cAllTabPrivs::cRetriever::iBulkSize_ = 100;

cAllTabPrivs::tAttributes cAllTabPrivs::cRetriever::rTable_[cAllTabPrivs::cRetriever::iBulkSize_];

cAllTabPrivs::cRetriever::cRetriever(
    ps::lib::cList<tAttributes>& oList
    , ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
)
    : cDescriber(oSvc, iBulkSize_
        , (iAccesible() ? szDbaTabPrivs : szAllTabPrivs)
        , "Reading Object Privileges", &oBind_
    )
    , oList_(oList)
    , oDefine_(oGetDefine())  
    , iSkip_(sizeof(tAttributes))
{
    const auto later_12c = oSvc.iCompDbVersion("1201",4) >= 0; // true: equal or later.
    // Replacing "%s" in the SQL with string values.
    this->vConvPlaceHolder({ sGetSqlInList(oOwners), later_12c ? cAllTabPrivs::sz12cSpec : "" });
    // Inbounding data from Oracle.
    oDefine_.vAddItem(rTable_->iTypeIdx, SQLT_INT, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szOwner, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szTableName, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szPrivilege, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szGrantee, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
    oDefine_.vAddItem(rTable_->szGrantable, SQLT_STR, nullptr, nullptr, nullptr, iSkip_);
}

void cAllTabPrivs::cRetriever::vPreBulkAction(const uint32_t& iBulkSize)
{
    // nothing to do
}

void cAllTabPrivs::cRetriever::vPostBulkAction(const uint32_t& iNumIter)
{
    oList_.insert(oList_.end(), &rTable_[0], &rTable_[iNumIter]);
    vAddOutputBytes(iSkip_ * iNumIter);
    vAddOutputRows(iNumIter);
}

void cAllTabPrivs::cRetriever::vPostRepeatAction()
{
    oList_.sort();
}

const char cAllTabPrivs::szDbaTabPrivs[] = {
"select decode "
"( ao.object_type "
", 'SEQUENCE', 2 "
", 'TABLE', 4 "
", 'MATERIALIZED VIEW', 5 "
", 'VIEW', 6 "
", 'TYPE', 7 "
", 'PACKAGE', 8 "
", 'SYNONYM', 9 "
", 10 "
") as type_idx "
", atp.owner "
", atp.table_name "
", atp.privilege "
", atp.grantee "
", atp.grantable "
"from dba_tab_privs atp "
", all_objects ao "
"where atp.owner in %s "
"and ao.owner = atp.owner "
"and ao.object_name = atp.table_name "
"and ao.object_type not in "
"( 'PACKAGE BODY' "
", 'TYPE BODY' "
", 'TABLE PARTITION' "
", 'TABLE SUBPARTITION' "
", 'TRIGGER' "
") %s"
};

const char cAllTabPrivs::szAllTabPrivs[] = {
"select decode "
"( ao.object_type "
", 'SEQUENCE', 2 "
", 'TABLE', 4 "
", 'MATERIALIZED VIEW', 5 "
", 'VIEW', 6 "
", 'TYPE', 7 "
", 'PACKAGE', 8 "
", 'SYNONYM', 9 "
", 10 "
") as type_idx "
", atp.table_schema "
", atp.table_name "
", atp.privilege "
", atp.grantee "
", atp.grantable "
"from all_tab_privs atp "
", all_objects ao "
"where atp.table_schema in %s "
"and ao.owner = atp.table_schema "
"and ao.object_name = atp.table_name "
"and ao.object_type not in "
"( 'PACKAGE BODY' "
", 'TYPE BODY' "
", 'TABLE PARTITION' "
", 'TABLE SUBPARTITION' "
", 'TRIGGER' "
") %s"
};

const char cAllTabPrivs::sz12cSpec[] = { " and ao.oracle_maintained = 'N'" };

cAllTabPrivs::cAllTabPrivs()
{
    // nothing to do
}

cAllTabPrivs::~cAllTabPrivs()
{
    // nothing to do
}

ps::lib::sql::cFetchable* cAllTabPrivs::oSubmit(
    ps::lib::sql::occi::cSvc& oSvc
    , const ps::lib::str_vct& oOwners
){
    return new cRetriever(
        oList_
        , oSvc
        , oOwners
    );
}

void cAllTabPrivs::vPrintAllItems(
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
        , &cAllTabPrivs::tAttributes::iKeyMatched
        , &cAllTabPrivs::tAttributes::vPrinter
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
