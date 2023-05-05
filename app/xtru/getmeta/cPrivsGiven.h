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

#pragma once

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cPrivsGiven
 * This class provides following roles.
 * -# Early inspection that privileges have been granted
 *    on database objects for USER.
 * -# It is controllable by value of iIgnoreInsufficient_,
 *    whether to assert (@b false) or tolerate (@b true)
 *    that the authority is sufficient.
 *
 * @par Example way to be used:
   @code
   using cPrivsGiven = ps::app::xtru::getmeta::cPrivsGiven;
   // Acquire reference to singleton.
   cPrivsGiven& oPrivsGiven_ = cPrivsGiven::get_mutable_instance();
   // true does not throw an exception even if privilege not granted.
   oPrivsGiven_.vSwitchTolerance(); // false -> true
   // Make DB available for reference.
   oPrivsGiven_.vSetService(*oSvc_);
   // Enumerate object privileges to be checked
   const auto iNumCheckedTabs = oPrivsGiven_
        // checks that given privilege is whether sufficient.
        ({"SYS", "DBA_TAB_PRIVS",             "SELECT"})
        ({"SYS", "DBA_OBJ_AUDIT_OPTS",        "SELECT"})
        ({"SYS", "DBA_SEGMENTS",              "SELECT"})
        ({"SYS", "DBA_INDEXES",               "SELECT"})
        ({"SYS", "DBA_CLU_COLUMNS",           "SELECT"})
        ({"SYS", "DBA_JOBS",                  "SELECT"})
        ({"SYS", "DBA_REFRESH",               "SELECT"})
        ({"SYS", "DBA_DB_LINKS",              "SELECT"})
        ({"SYS", "DBA_MVIEW_LOG_FILTER_COLS", "SELECT"})
        ({"SYS", "DBA_AUDIT_OBJECT",          "SELECT"})
        ({"SYS", "DBA_DEPENDENCIES",          "SELECT"})
        // true: Shortage is ignored.
        .iCheckPrivsGiven(true)
   ;
   // Confirm whether access is possible (repeatable).
   if (oPrivsGiven_.iAccesible({"SYS", "DBA_MVIEW_LOG_FILTER_COLS", "SELECT"}))
   {
       ...;
   }
   @endcode
 */
class cPrivsGiven
    : public boost::serialization::singleton< cPrivsGiven >
{
    friend class boost::serialization::singleton< cPrivsGiven >;
private:
    /**
     * @brief
     * This variable is stored a PL/SQL program.
     * -# It aquires a value of ":b_privs_given" from the data dictionary,
     *    that is indicated user is accessible for it.
     * -# :b_table_schema, :b_table_name and :b_privilege are
     *    referred to in order to specify the object.
     */
    static const std::string sPls_;
    ps::lib::cTracer& trc_;  ///< to log the detail of the activities.
    ps::lib::sql::occi::cSvc* oSvc_; ///< To use Oracle DB provided service.
    /**
     * @brief
     *   Immediately after construction it is initialized with @b false.
     *   It can be changed to @b true by cPrivsGiven::vSwitchTolerance.
     *
     * @details
     * - @b false: If the granted authority is insufficient,
     *   an exception is thrown when calling cPrivsGiven::operator()().
     * - @b true: cPrivsGiven::operator()() behaves generously without throwing an exception.
     *   Hence it accepts that it is unknown whether privilege has been granted
     *   until calling cPrivsGiven::iAccesible().
     */
    bool iIgnoreInsufficient_;
    /**
     * @class tKey
     * @brief
     *   It gives convenience for handling elements
     *   constituting object privileges collectively.
     */
    struct tKey
    {
        tKey(
            const std::string& sOwner
            , const std::string& sTableName
            , const std::string& sPrivilege
        );
        tKey(const tKey& rhs);
        tKey& operator=(const tKey& rhs);
        bool operator<(const tKey& rhs) const;
        friend std::ostream& operator<<(std::ostream& os, const tKey rhs)
        {
            return os
                << boost::format(R"(sOwner="%s", sTableName="%s", sPrivilege="%s")")
                % rhs.sOwner_ % rhs.sTableName_ % rhs.sPrivilege_
            ;
        }
        bool iCompareCandiates(const ps::lib::str_set candidate) const;
        std::string sOwner_;      ///< A name of owner
        std::string sTableName_;  ///< A name of bable
        std::string sPrivilege_;  ///< A name of object privilege
        int32_t iPrivsGiven_ = 0; ///< >0: privilege is given
        std::string sReason_;     ///< Rationale deemed to be accessible
        std::string sPrintIdentifier() const;
    };
    ps::lib::cSet<tKey> oSet_; ///< A collection of object privileges of concern.
public:
    /**
     * @brief
     *   makes the negation of the value of the current cPrivsGiven::iIgnoreInsufficient_
     *   a new value.
     */
    void vSwitchTolerance(void);
    /**
     * @brief
     *   makes DB connection referable.
     * @param[in] oSvc
     *   @copybrief cPrivsGiven::oSvc_
     */
    void vSetService(ps::lib::sql::occi::cSvc& oSvc);
    /**
     * @brief
     *   registers the object privelege of concern and checks if it is granted to USER.
     * @param[in] oKey
     *   @copybrief cPrivsGiven::tKey
     */
    cPrivsGiven& operator()(tKey oKey);
    /**
     * @brief
     *   checks the object privelege of concern is granted to USER.
     * @param[in] oKey
     *   @copybrief cPrivsGiven::tKey
     * @return true: Yes, it has been granted.
     */
    bool iAccesible(const tKey& oKey) const;
    /**
     * @brief
     * - You should call this giving false if you want to detect that
     *   the privileges are insufficient and abort the program early.
     * @param[in] iIgnoreShortage
     * - The meaning of the value is as follows, but it can be specified
     *   independently of the value stored in cPrivsGiven::iIgnoreInsufficient_.
     * - @b false: If the granted authority is insufficient, an exception is thrown.
     * - @b true: behaves generously without throwing an exception.
     *
     * @return
     *   A number of the object priveleges of concern.
     */
    int32_t iCheckPrivsGiven(const bool& iIgnoreShortage) const;
    /**
     * @brief
     *   cleanns cPrivsGiven::oSet_.
     */
    void vClear(void);
private:
    /**
     * @brief
     */
    cPrivsGiven();
    cPrivsGiven(const cPrivsGiven&) =delete;
    cPrivsGiven& operator=(const cPrivsGiven&) =delete;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
