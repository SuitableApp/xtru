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
#include "cPrivsGiven.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @details
 */
cPrivsGiven::tKey::tKey(
    const std::string& sOwner
    , const std::string& sTableName
    , const std::string& sPrivilege
)
    : sOwner_(sOwner)
    , sTableName_(sTableName)
    , sPrivilege_(sPrivilege)
    , iPrivsGiven_(0)
{}

/**
 * @details
 */
cPrivsGiven::tKey::tKey(const tKey& rhs)
    : sOwner_(rhs.sOwner_)
    , sTableName_(rhs.sTableName_)
    , sPrivilege_(rhs.sPrivilege_)
    , iPrivsGiven_(rhs.iPrivsGiven_)
    , sReason_(rhs.sReason_)
{}

/**
 * @details
 */
cPrivsGiven::tKey& cPrivsGiven::tKey::operator=(const tKey& rhs)
{
    sOwner_ = rhs.sOwner_;
    sTableName_ = rhs.sTableName_;
    sPrivilege_ = rhs.sPrivilege_;
    iPrivsGiven_ = rhs.iPrivsGiven_;
    sReason_ = rhs.sReason_;
    return *this;
}

/**
 * @details
 */
bool cPrivsGiven::tKey::operator<(const tKey& rhs) const
{
    return ps::lib::composite_less(
        sOwner_, rhs.sOwner_
        , sTableName_, rhs.sTableName_
        , sPrivilege_, rhs.sPrivilege_
    );
}

/**
 * @details
 */
std::string cPrivsGiven::tKey::sPrintIdentifier() const
{
    return (boost::format(R"("%s"."%s"."%s")")
        % sOwner_ % sTableName_ % sPrivilege_).str();
}

/**
 * @details
 */
bool cPrivsGiven::tKey::iCompareCandiates(const ps::lib::str_set candidate) const
{
    return candidate.find(sPrivilege_) != candidate.end();
}

/**
 * @details
 */
const std::string cPrivsGiven::sPls_ =
"begin "
    "if :b_table_schema = USER then "
        "select count(*) "
        "into :b_privs_given "
        "from USER_OBJECTS "
        "where OBJECT_NAME = :b_table_name; "
        ":b_reason := USER || ' is a owner of ' || :b_table_name;"
    "else "
        "select count(*) "
        "into :b_privs_given "
        "from ALL_TAB_PRIVS "
        "where TABLE_SCHEMA = :b_table_schema "
        "and TABLE_NAME = :b_table_name "
        "and PRIVILEGE = :b_privilege; "
        ":b_reason := USER || ' is granted an object privilege by ' || :b_table_schema;"
    "end if; "
    "if :b_privs_given = 0 then "
        // Current is neither an owner nor a grantee.
        // Checking for granted system privileges instead.
        "select count(*) "
        "into :b_privs_given "
        "from all_objects "
        "where OWNER = :b_table_schema "
        "and OWNER != 'SYS' "
        "and OBJECT_NAME = :b_table_name; "
        "if :b_privs_given > 0 then "
            "if :b_privilege in ('INSERT','SELECT','UPDATE','DELETE') then "
                "select count(*) "
                "into :b_privs_given "
                "from SESSION_PRIVS "
                "where PRIVILEGE = :b_privilege || ' ANY TABLE'; "
            "elsif :b_privilege = 'EXECUTE' then "
                "select count(*) "
                "into :b_privs_given "
                "from SESSION_PRIVS "
                "where PRIVILEGE = :b_privilege || ' ANY PROCEDURE'; "
            "end if; "
            ":b_reason := USER || ' is granted a system privilege ' || "
             ":b_privilege || ' ANY <TABLE | PROCEDURE> by ' || :b_table_schema;"
        "end if; "
    "end if; "
"end; "
;

/**
 * @details
 */
cPrivsGiven::cPrivsGiven(void)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , oSvc_(nullptr)
    , iIgnoreInsufficient_(false)
{}

void cPrivsGiven::vSwitchTolerance(void)
{
    iIgnoreInsufficient_ = !iIgnoreInsufficient_;
}

void cPrivsGiven::vSetService(
    ps::lib::sql::occi::cSvc& oSvc
){
    oSvc_ = &oSvc;
}

/**
 * @details
 */
cPrivsGiven& cPrivsGiven::operator()(tKey oKey){
    ASSERT_OR_RAISE(oSvc_, std::runtime_error
        , std::string("Call vSetService before operator() invoking."));
    auto result = oSet_.find(oKey);
    ASSERT_OR_RAISE(
        result == oSet_.end()  /// Assert no duplication.
        , std::runtime_error
        , boost::format("%s Violated key uniqueness. Given as [%s]") % sClass(ps::lib::E) % oKey
    );
    //using ps::lib::operator<<;
    const ps::lib::str_set candidate({"INSERT","SELECT","UPDATE","DELETE","EXECUTE"});
    ASSERT_OR_RAISE(
        oKey.iCompareCandiates(candidate)
        , std::runtime_error
        , boost::format(
            "%s Specified privilege '%s' is wrong. Should select any one in [%s]"
        ) % sClass(ps::lib::E) % oKey.sPrivilege_ % candidate
    );
    // Parameters for PL/SQL
    ps::lib::sql::occi::cBind oBind;
    char szReason[80];
    int16_t nReasonInd;
    uint16_t iReason = sizeof(szReason);
    ::memset(szReason, 0, iReason);

    // IN:  :b_table_schema, :b_table_name, :b_privilege
    // OUT: :b_privs_given, :b_reason
    oBind.vAddItem(":b_table_schema", oKey.sOwner_);
    oBind.vAddItem(":b_table_name", oKey.sTableName_);
    oBind.vAddItem(":b_privilege", oKey.sPrivilege_);
    oBind.vAddItem(":b_privs_given", &oKey.iPrivsGiven_);
    oBind.vAddItem(":b_reason", szReason, iReason, &nReasonInd, &iReason);
    const auto sTagStr = oKey.sPrintIdentifier();
    std::unique_ptr<ps::lib::sql::occi::cStmt> oOcci(
        new ps::lib::sql::occi::cStmt(*oSvc_, 1, sPls_, sTagStr, &oBind)
    );
    oOcci->vExecute();
    bool iResult = oKey.iPrivsGiven_ > 0;
    if (iResult)
    {
        oKey.sReason_.assign(szReason, iReason);
    }
    else
    {
        ASSERT_OR_RAISE(
            iIgnoreInsufficient_, std::runtime_error
            , boost::format("%s Not found or insufficent privilege %s")
                % sClass(ps::lib::E) % sTagStr
        );
        oKey.sReason_.assign(
            "The object could not be found or required privilege not assigned."
        );
    }
    trc_ << boost::format("%s %s accesible, becaouse %s")
        % sTagStr % (iResult ? "is" : "is not") % oKey.sReason_ << std::endl;
    oSet_.insert(oKey);

    return *this;
}

/**
 * @details
 */
bool cPrivsGiven::iAccesible(const tKey& oKey) const
{
    const auto it = oSet_.find(oKey);
    return it != oSet_.end() && it->iPrivsGiven_ > 0;
}

/**
 * @details
 */
int32_t cPrivsGiven::iCheckPrivsGiven(const bool& iIgnoreShortage) const
{
	int32_t iRet = 0;
    for (ps::lib::cSet<tKey>::const_reference oItem : oSet_)
    {
        ASSERT_OR_RAISE(
            iIgnoreShortage || oItem.iPrivsGiven_ > 0
            , std::runtime_error
            , boost::format("%s %s") % oItem.sReason_ % oItem
        );
        iRet += oItem.iPrivsGiven_;
    }
    return iRet;
}

/**
 * @details
 */
void cPrivsGiven::vClear(void)
{
    oSet_.clear();
    trc_ << boost::format("Cleared the collection of object privileges.");
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
