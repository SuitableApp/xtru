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

#pragma once

namespace ps
{

namespace app
{

namespace xtru
{

namespace copydd
{

class cCopyDd
{
private:
    using tLite3Type = ps::lib::sql::lite3::cAttr::tLite3Type;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
    /** 
     * mediates to retrive startup parameters from xtru.conf.
     */
    const ps::lib::cConfigures& conf_;
    /**
     * A string which passed to constructor of tokens_ is tokenaized 
     * by the rule_. Assuming each tokens_ are separated with either
     *  a white space, a hard tab, or a comma. Optionally a token
     * can be enclosed by a pair of two double quotations, 
     * if the token has sparate characters as a part of itself.
     */
    ps::lib::tSep rule_;
    ps::lib::sql::occi::cSvc* oSvc_;       /// Environment for an Oracle database client.
    ps::lib::sql::lite3::cSqliteDb* oDb_;  /// Environment for a SQLite3.
    ps::app::xtru::cTableList oTableList_;
    cCopyDd(const cCopyDd&) =delete;  ///< Prohibited to copy itself.
    cCopyDd& operator=(const cCopyDd&) =delete; ///<  Prohibited to copy itself.
    /**
     * This function searches for a value of the specified key 
     * in the xtru.conf (= conf_).
     * Then, it reads token sequence from a file with the same name
     * of the value.
     * Finally, token sequence is appended to the vct as the first 
     * argument of this.
     */
    void vFillTokensIntoVctInKeySpecFile(
        ps::lib::str_vct& vct
        , const std::string& key
    );
    template <class T, class F>
    void vInitializeRepo(
        const std::string sTableName
        , T true_action
        , F false_action
    ){
        ps::lib::sql::lite3::cCheckErr oChecker(*oDb_);
        bool iNotExists = ps::lib::sql::lite3::nsHelper::iDoesTableNotExist(sTableName.c_str(), *oDb_);
        ASSERT_OR_RAISE_FNC(
            SQLITE_OK == (iNotExists ? true_action() : false_action())
            , std::runtime_error, oChecker
        );
        trc_ << boost::format("%s was %s.") % sTableName
            % std::string(iNotExists ? "created" : "deleted") << std::endl;
    }
public:
    cCopyDd();
    /**
     * @brief
     * This should be called before the cCopyDd::vCreateRepo() runnning.
     */
    void vAssociate(
        ps::lib::sql::occi::cSvc* oSvc
        , ps::lib::sql::lite3::cSqliteDb* oDb
    );
    /**
     * @brief
     * Cleaning up the repository. 
     * After this work, You can see the schema on the Sqlinte3.
     * Do the following if you want to check it.
     * /bin/sqlite3 $HOME/sa_home/xtru.db
     * .schem [TABLE_NAME]
     * .quit
     * This should be called before the cCopyDd::oRefresGetDataRepo() runnning.
     */
    void vCreateRepo();
    ~cCopyDd();
    /**
     * @brief
     * @param[in] oWildOwners
     *    A collection of candidates of schema name.
     *    Allowd to pass not only literals, but also identifiers
     *    containing SQL wildcard characters.
     * @return
     *    A collection of existing schema names in the DB (ALL_USERS).
     */
    ps::lib::str_vct oConvEffectiveOwnerName(const ps::lib::str_vct& oWildOwners);
    /**
     * @brief
     * This is used to make or refresh the repository.
     * Actually Repository is implemented as table of Sqlite3.
     * For example it includes table attributes, constraint attributes,
     * various of such information,
     * Repository is a subset of Oracle data dictionary.
     */
    void vRefresGetDataRepo(const ps::lib::str_vct& oOwners);
    const ps::app::xtru::cTableList& oGetTableList(void) const;
};

} // ps::app::xtru::copydd

} // ps::app::xtru

} // ps::app

} // ps

