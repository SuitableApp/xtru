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
#define NAX_NUM_ELEMENT_QUERIES 1000

namespace ps
{

namespace app
{

namespace xtru
{

namespace getdata
{

class cQueryImpl
    : private cStartValues
{
private:
    using cScriptMaker = ps::app::xtru::getmeta::cScriptMaker;
    using tWayToNext = ps::app::xtru::getmeta::tWayToNext;
    const std::string sTag_;
    const int32_t iRows_;  ///< A Number of rows at a time of loading.
    ps::lib::tPtrFstream st_make_sh_;
    ps::lib::sql::occi::cBind oBind_;
    void vPrintExecLoader(const std::string& fname)
    {
        const auto param_f(sGetParfName(true));
        *st_make_sh_ << boost::format("%s parfile=%s rows=%d control=%s.ctl")
                  % exec_load_.string()
                  % param_f.string()
                  % iRows_
                  % fname
            << std::endl
        ;
    }
    void vSubmitWithSqlStmt(
        const std::string& sOwner
        , const std::string& sSelect
        , const std::string& file_n
    ){
        if (partitioning_ & SCHEMA_BASED_PARTITIONING)
        {
            ps::lib::tSep sep("\\", ";", "");
            boost::tokenizer< ps::lib::tSep > tokens(sSelect, sep);
            ASSERT_OR_RAISE(tokens.begin() != tokens.end(), std::runtime_error
                , boost::format(
                  "At least one statement must be included in 'sql'. Actually zero."
                )
            );
            const auto dist = std::distance(tokens.begin(), tokens.end());
            ASSERT_OR_RAISE(dist < NAX_NUM_ELEMENT_QUERIES, std::runtime_error
                , boost::format(
                  "Too much element queries in sSelect. Actually %d, it must be less then %d."
                ) % dist % NAX_NUM_ELEMENT_QUERIES
            );
            auto i = 1;
            for (const auto& sql: tokens)
            {
                const auto trimed = boost::trim_copy(sql);
                if (ps::lib::trim(trimed).size() == 0U) return;
                unldrs_.push_back(new ps::lib::sql::occi::cUnloader(
                    *oSvc_
                    , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, file_n, (boost::format("%03d") % i).str())
                    , iBulkSize_, trimed, file_n, ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind_
                ));
                vPrintExecLoader((boost::format("%s_%03d") % file_n % i).str());
                ++i;
            }
        }
        else
        {
            unldrs_.push_back(new ps::lib::sql::occi::cUnloader(
                *oSvc_
                , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, file_n, "" /*sPartitionName*/)
                , iBulkSize_, sSelect, file_n, ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind_
            ));
            vPrintExecLoader(file_n);
        }
    }
    void vSubmitFromStream(
        const std::string& sOwner
        , const boost::filesystem::path& sQryFile
        , const std::string& sOutputStem
    ){
        boost::filesystem::ifstream ifs(sQryFile);
        // Reading all data.
        const auto sSelect = std::string(
            std::istreambuf_iterator<char>(ifs)
            , std::istreambuf_iterator<char>(/*EOF*/)
        );
        vSubmitWithSqlStmt(sOwner, sSelect, sOutputStem);
    }
    /**
     * @brief
     * @param[in] sOwner
     *   The name of the schema that exists in the database.
     * @param[in] rDataFmt
     *   FMT_VAR: Specification to extract as variable length data.
     *   FMT_FIX: Specification to extract as fixed length data.
     * @param[in] oObj
     *   It is assumed that one of the following three kinds of contents will be passed.
     *  -# The directory name where zero or more files containing arbitrary querie(s) are saved.<br/>
     *     You can specify an absolute path or a relative path visible from the current directory.
     *  -# The name of a file containing arbitrary query.
     *  -# One SQL-select statement body is passed. A semicolon is placed
     *     at the end to identify that it is not a path on the file system.
     */
    void vAddTaskItem(
        const std::string& sOwner
        , const ps::app::xtru::tFmtOfData& rDataFmt
        , const boost::filesystem::path& oObj
    ){
        if (boost::filesystem::exists(oObj))
        {
            mos_ << boost::format(R"(A value of parameter %s is %s)")
                % rDataFmt.sConfParam_ % oObj << std::endl;
            if (boost::filesystem::is_directory(oObj))
            {
                /*
                 * oObj is assumed as a name of the directory.
                 */
                mos_ << boost::format("Directory %s is found.") % oObj << std::endl;
                auto cnt = 0;
                boost::filesystem::directory_iterator it(oObj), end;
                for (;it != end; ++it)
                {
                    if ( ! boost::filesystem::is_regular_file(*it))
                    {
                        continue;
                    }
                    const auto stem = (*it).path().stem();
                    const auto ownr = stem.stem();
                    const auto extn = (*it).path().extension();
                    trc_ << boost::format("[stem,ownr,extn]=[%s,%s,%s]")
                            % stem % ownr % extn << std::endl;
                    if (
                        extn.empty() ||
                        ( stem != ownr && ownr.string() != sOwner) ||
                        !ps::lib::stricmp(extn.string(), "." + extnamesql_.string())
                    ){
                        mos_ << boost::format("  Excluded  %s. Because it is in the namespace of %s, not %s.")
                            % *it % ownr.string() % sOwner << std::endl;
                        continue;
                    }
                    // Excluded the extention from path = stem
                    const std::string sOutputStem = (stem == ownr ? sOwner + "." : "") + stem.string();
                    if (
                        !stem.empty() && !extn.empty() &&
                        ps::lib::wild_strmatch(queryfilename_, stem.string())
                    ){
                        mos_ << boost::format("  Importing a query from %s") % *it << std::endl;
                        vSubmitFromStream(sOwner, *it, sOutputStem);
                        ++cnt;
                    }
                }
                mos_ << boost::format("  %d files matches.") % cnt << std::endl;
            }
            else
            {
                /*
                 * oObj is a name of the file.
                 */
                mos_ << boost::format("  Importing a query from %s") % oObj << std::endl;
                vSubmitFromStream(sOwner, oObj, sOwner + "." + oObj.stem().string());
            }
        }
        else
        {
            /*
             * oObj regarded as a SQL-select statement.
             */
            // (?i:(A)) maches both "A" and "a".
            const boost::regex select_re_(R"(\s*(?<sql_stmt>(?i:(SELECT))\s+.+\s+(?i:(FROM))\s+.+;))");
            const boost::regex into_re_(R"((?<front_part>.*)(?i:(INTO))\s+(?<file_name>\w+)\s+(?<back_part>.+))");
            ASSERT_OR_RAISE(
                *(oObj.string().end()-1) == ';' // oObj must be ended with a semi-colon.
                , std::runtime_error
                , boost::format(
                    "If parameter '%s' is a query, it must be ended with a semi-colon."
                    " Check for specified value: %s"
                ) % rDataFmt.sConfParam_ % oObj.filename()
            );
            auto it = oObj.string().cbegin(), end = oObj.string().cend();
            boost::smatch result;  // Divided symbols according to grammar.
            ASSERT_OR_RAISE(
                boost::regex_search(it, end, result, select_re_)
                , std::runtime_error
                , boost::format(
                    "Parameter '%s' should be a query, but it has any problem"
                    " in grammar of SQL-statement. Check for specified value: %s"
                ) % rDataFmt.sConfParam_ % oObj.filename()
            );
            mos_ << boost::format("Query [%s] is passwd.") % result["sql_stmt"] << std::endl;
            boost::smatch stmt;
            std::string sSelect;
            std::string fn_stem;
            if (boost::regex_search(result["sql_stmt"].first, result["sql_stmt"].second, stmt, into_re_))
            {
                // Substring 'into table' is trimed from stmt.
                sSelect = stmt["front_part"] + stmt["back_part"];
                fn_stem = stmt["file_name"];

            }
            else
            {
                sSelect = result["sql_stmt"];
                fn_stem = rDataFmt.sDefaultFn_;
            }
            vSubmitWithSqlStmt(sOwner, sSelect, fn_stem);
        }
    }
    void vGenerateScripts();
public:
    cQueryImpl()
        : sTag_("Query")
        , iRows_(100000)
    {
        mos_ << boost::format("*** Started %s ***") % sTag_ << std::endl;
        mos_ << boost::format("Importing parameter value(s) from %s")
            %  filebind_ << std::endl;
        oBind_.vImportParameters(filebind_);
        mos_ << boost::format("  Provided parameter%s: ")
            % (oBind_.iSize() > 1 ? "s are" : " is") << oBind_.sPrintItems() << std::endl;
    }
    ~cQueryImpl()
    {
        mos_ << boost::format("*** Finished %s ***") % sTag_ << std::endl;
    }
    /// Executing queries.
    void vExpData()
    {
        st_make_sh_ = oGetStreamToMakeSh(output_, iSkipScr_);
        *st_make_sh_ << "#!/bin/sh -x" << std::endl;
        *st_make_sh_
            << boost::format("test -f %s && %s /nolog @%s")
                % disable_deps_.string() % exec_plus_.string() % disable_deps_.stem().string()
            << std::endl;
        // to issue 'alter session set current_schem'.
        ps::lib::sql::occi::cSetCurrentSchema oSchema(oSvc_);
        for (const auto sOwner : oOwners_)
        {
            mos_ << boost::format("Switching current_schema to %s") % sOwner << std::endl;
            oSchema.vAlterCurrent(sOwner);
            vAddTaskItem(sOwner, ps::app::xtru::tFmtOfData::FMT_VAR, queryvar_);
            vAddTaskItem(sOwner, ps::app::xtru::tFmtOfData::FMT_FIX, queryfix_);
            // All other threads are joined main thread here.
            ps::lib::vSynchronize(iConcurrency_, unldrs_, &ps::lib::sql::cFetchable::vExecuteAndFetch);
        }
        *st_make_sh_
            << boost::format("test -f %s && %s /nolog @%s")
                % enable_deps_.string() % exec_plus_.string() % enable_deps_.stem().string()
            << std::endl
            << boost::format(R"(%s "^%s" *.log > _errm.txt)")
                % sFindstrCmd_.string() % R"(ORA-)"
            << std::endl
            << boost::format(R"(%s "^%s" *.log >> _errm.txt)")
                % sFindstrCmd_.string() % R"(SQL\*Loader-)"
            << std::endl
        ;
        st_make_sh_.reset();  // To break bondage.
    }
    /// Making scripts (for to enable or disable constraint, database trigger).
    void vMakeScripts()
    {
        // vOutputScripts generates following files:
        // (1). _make.sh: shell script to execute SQL*Loader and SQL*Plus.
        // (2). _par0.txt, _par1.txt: Parameter file for initializing SQL*Loader.
        // (3). _conn.sql: This is used to connect to the Oracle Database via SQL*Plus.
        // (4). _wait_here.sh:
        if (rtn_.iCotinue() && !iSkipScr_)
        {
            cScriptMaker oScriptMaker_(output_);
            oScriptMaker_.vOutputScripts(
                *this
                , &cQueryImpl::vGenerateScripts
                , mode_
                , make_sh_
                , par0_txt_
                , par1_txt_
                , conn_sql_
                , wait_here_
            );
            mos_ << std::string("Finished generating scripts.") << std::endl;
        }
    }
};

void cQueryImpl::vGenerateScripts()
{
    // Nothig to do.
}

cQuery::cQuery()
    : oImpl_(new cQueryImpl)
{}

void cQuery::vRun()
{
    oImpl_->vExpData();
    oImpl_->vMakeScripts();
}

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps

