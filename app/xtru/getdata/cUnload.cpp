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
#include "xtru.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getdata
{

class cUnloadImpl
    : private cStartValues
{
private:
    using cScriptMaker = ps::app::xtru::getmeta::cScriptMaker;
    using tWayToNext = ps::app::xtru::getmeta::tWayToNext;
    using cChmod = ps::lib::system::cChmod;
    using cPrivsGiven = ps::app::xtru::getmeta::cPrivsGiven;
    const std::string sTag_;
    const boost::filesystem::fstream::ios_base::openmode mode_;
    cPrivsGiven& oPrivsGiven_;
    const int32_t iRows_;  ///< A Number of rows at a time of loading.
    ps::lib::tPtrFstream st_make_sh_;
    const bool is_usualpath_;
    // sSelect will be stored one (or more) SQL-select statement(s).
    // When multiple items are stored, their are delimited with a semi-colon each other.
    ps::lib::sql::cFetchable * oSubmitWithSqlStmt(
        const std::string sSelect
        , const ps::app::xtru::tTabName& tbl
    ){
        const auto table_n(tbl.sGetConcatenatedName()); // Non-enclosing name will be return.
        const auto file_n = ps::lib::sConvertDollar2Sharp(table_n);
        return new ps::lib::sql::occi::cUnloader(
            *oSvc_
            , new ps::lib::nsStreamLocator::cStreamLocator(tbl.sOwner, file_n , "" /*sPartitionName*/)
           , iBulkSize_ , sSelect, table_n, tbl.iNumLongs
        );
    }
    void vPrintExecLoader(const ps::app::xtru::tTabName& tbl)
    {
        const auto param_f(sGetParfName(is_usualpath_ || tbl.iNumLongs));
        const auto fname = ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName());
        *st_make_sh_
            << (
              ("IOT" == tbl.sIotType || tbl.iNumLobs)
              ? boost::format("%s parfile=%s control=%s.ctl")
                  % exec_load_.string()
                  % param_f.string()
                  % fname
              : boost::format("%s parfile=%s rows=%d control=%s.ctl")
                  % exec_load_.string()
                  % param_f.string()
                  % iRows_
                  % fname
            )
            << std::endl
        ;
    }
    template<class T>
    void vTaskEntry(
        T& arr
        , const int expr
        , const ps::app::xtru::tTabName& tbl
    ){
        if (expr)
        {
            const auto param_f(sGetParfName(is_usualpath_ || tbl.iNumLongs));
            arr.vInsertUnloadTasks(tbl, unldrs_, param_f);
        }
        else
        {
            const auto sStmt = arr.sGenerateSql(tbl);
            unldrs_.push_back(oSubmitWithSqlStmt(sStmt, tbl));
            vPrintExecLoader(tbl);
        }
    }
    /**
     * Enqueue in the execution order of the queries.
     */
    void vSubmitUnloadSchedule(const ps::app::xtru::cTableList& oTableList)
    {
        // oScheme_ contains data for dividing the table for each partition.
        ps::app::xtru::getdata::cPartitionedByScheme oScheme_(oDb_, oSvc_, iBulkSize_, iRows_, st_make_sh_);
        // ORowid_ contains data for dividing the table into a plurality of chunks in the ROWID range.
        ps::app::xtru::getdata::cPartitionedByRowid oRowid_(oDb_, oSvc_, iBulkSize_, iRows_, st_make_sh_);
        for (const ps::app::xtru::tTabName& tbl : oTableList)
        {
            if (oScheme_.iFind(tbl))
            {
                vTaskEntry(oScheme_, partitioning_ & SCHEMA_BASED_PARTITIONING, tbl);
            }
            else if (oRowid_.iFind(tbl))
            {
                vTaskEntry(oRowid_, partitioning_ & ROWID_BASED_PARTITIONING, tbl);
            }
            else
            {
                // SQL for tables that are not split and extracted.
                const auto sStmt = "SELECT * FROM " + tbl.sGetConcatenatedName("\"");
                unldrs_.push_back(oSubmitWithSqlStmt(sStmt, tbl));
                vPrintExecLoader(tbl);
            }
        }
    }
    /*
     * @param[in] iWayToNext:
     * - iForward:  drop indexes, disable triggers and constraints.
     * - iBackward: create indexex, enable triggers and constraints.
     */
    void vGenerateScripts();
public:
    /// Constructor sequences ordering to unload.
    cUnloadImpl()
        : sTag_("Unload")
        , mode_(
            boost::filesystem::fstream::ios_base::out
            | boost::filesystem::fstream::ios_base::trunc
        )
        , oPrivsGiven_(cPrivsGiven::get_mutable_instance())
        , iRows_(100000)
        , is_usualpath_(conf_.as<bool>("usualpath"))
    {
        oPrivsGiven_.vSwitchTolerance();
        oPrivsGiven_.vSetService(*oSvc_);
        mos_ << boost::format("*** Started %s ***") % sTag_ << std::endl;
        if (oOwners_.size())
        {
            oPrivsGiven_
                // checks that given privilege is whether sufficient.
                ({"SYS","DBA_OBJECTS","SELECT"})
                ({"SYS","DBA_INDEXES","SELECT"})
            ;
        }
        oCopyDd_.vRefresGetDataRepo(oOwners_);
    }
    ~cUnloadImpl()
    {
        mos_ << boost::format("*** Finished %s ***") % sTag_ << std::endl;
    }
    /// Unloading tables.
    void vExpData()
    {
        const auto sDisableDeps = disable_deps_;
        // refreshes contents of the repository, and retrieves table name list.
        const auto& oTableList = oCopyDd_.oGetTableList();
        st_make_sh_ = oGetStreamToMakeSh(output_, iSkipScr_);
        *st_make_sh_ << "#!/bin/sh -x" << std::endl;
        *st_make_sh_ << boost::format("test -f %s && %s /nolog @%s")
            % sDisableDeps.string() % exec_plus_.string() % sDisableDeps.stem().string() << std::endl;
        vSubmitUnloadSchedule(oTableList);
        // All other threads are joined main thread here.
        ps::lib::vSynchronize(iConcurrency_, unldrs_, &ps::lib::sql::cFetchable::vExecuteAndFetch);
        for (const auto& tbl : oTableList)
        {
            const auto fname = ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName());
            *st_make_sh_
                << boost::format("%s /nolog @%s%s.sql")
                  % exec_plus_.string()
                  % fname
                  % sSuffixDeps_
                << std::endl
            ;
        }
        *st_make_sh_
            << boost::format("%s %s")
                % "sh" % wait_here_.string()
            << std::endl;
        for (const auto& tbl : oTableList)
        {
            const auto fname = ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName());
            *st_make_sh_
                << boost::format("%s /nolog @%s%s.sql")
                  % exec_plus_.string()
                  % fname
                  % sSuffixRefs_
                << std::endl
            ;
        }
        *st_make_sh_
            << boost::format(R"(%s "^%s" *.log > _errm.txt)")
                % sFindstrCmd_.string() % R"(ORA-)"
            << std::endl
            << boost::format(R"(%s "^%s" *.log >> _errm.txt)")
                % sFindstrCmd_.string() % R"(SQL\*Loader-)"
            << std::endl
        ;
        st_make_sh_.reset();  // To break bondage.
        // Apply u+w,g+w to the permissions of all control, data
        // and SQL script files under the output_.
        {
            cChmod obj(
                output_
                , boost::filesystem::add_perms
                | boost::filesystem::group_write
                );
            obj((boost::format(R"(.*\.(ctl|%s))") % dataext_).str());
        }
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
                , &cUnloadImpl::vGenerateScripts
                , mode_
                , make_sh_
                , par0_txt_
                , par1_txt_
                , conn_sql_
                , wait_here_
            );
            // Apply u+w,g+w to the permissions of all SQL script files under the output_.
            {
                cChmod obj(
                    output_
                    , boost::filesystem::add_perms
                    | boost::filesystem::group_write
                    );
                obj ((boost::format(R"(.*_(DEPS|REFS)\.sql)")).str())
                    (disable_deps_)
                ;
            }
            mos_ << std::string("Finished generating scripts.") << std::endl;
        }
    }
};

namespace /* anonymous */
{

/**
 * @class cStreamWrapper
 */
class cStreamWrapper
{
private:
    const boost::filesystem::path str;
    ps::lib::tPtrFstream oss_;
public:
    cStreamWrapper(
        const boost::filesystem::fstream::ios_base::openmode mode
        , const boost::filesystem::path& base
        , const boost::filesystem::path fname
    )
        : str(base / fname)
        , oss_(ps::lib::open_with_check(str, mode))
    {}
    cStreamWrapper(
        const boost::filesystem::fstream::ios_base::openmode mode
        , const boost::filesystem::path& base
        , const ps::app::xtru::tTabName& tbl
        , const std::string& sPostFix
    )
        : cStreamWrapper(
            mode, base
            , (boost::format("%s%s.sql")
                % ps::lib::sConvertDollar2Sharp(tbl.sGetConcatenatedName())
                % sPostFix
            ).str()
        )
    {}
    template<class T>
    std::ostream& operator<<(const T& rhs) const
    {
        return *oss_ << rhs;
    }
    boost::filesystem::path sGetFilename(void) const
    {
        return str;
    }
};

} /* anonymous */

void cUnloadImpl::vGenerateScripts()
{
    const auto iDependent_(conf_.as<bool>("dependent"));
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    oPrivsGiven_.vClear();  // Strage calass of oPrivsGiven_ is static.
    oPrivsGiven_
        // checks that given privilege is whether sufficient.
        ({"SYS","DBA_SEGMENTS","SELECT"})
        ({"SYS","DBA_INDEXES","SELECT"})
    ;
    // Register convertion rules.
    // cFunctionalReplace::operator()("As is", "To be")
    ps::app::xtru::getmeta::cFunctionalReplace oFunctionalReplace;
    oFunctionalReplace
        ("DEFAULT", "default")
        ("MAXVALUE", "maxvalue")
        ("NULL", "null")
        (
            "TO_DATE(' %4s-%2s-%2s 00:00:00', 'S%4s-%2s-%2s HH24:MI:SS', "
            "'NLS_CALENDAR=GREGORIAN')", "to_date('{0}-{1}-{2}','{3}-{4}-{5}')")
    ;
    // Constraints and Triggers.
    namespace GM = ps::app::xtru::getmeta; // xtru::GetMeta
    GM::cConsValidate::vSetMask(iMngScrOut_);
    GM::cConsValidate
        oC1024(GM::cConsValidate::tConsType::iC1024)
      , oC1025(GM::cConsValidate::tConsType::iC1025)
      , oC1026(GM::cConsValidate::tConsType::iC1026)
      , oC1027(GM::cConsValidate::tConsType::iC1027)
      , oC1028(GM::cConsValidate::tConsType::iC1028)
    ;
    // Query the dictionary and collect data.
    oC1024.vExecuteAndFetch(oDb_, oOwners_); // FK constraints
    oC1025.vExecuteAndFetch(oDb_, oOwners_); // check constraints
    oC1026.vExecuteAndFetch(oDb_, oOwners_); // PK or AK constraints
    oC1027.vExecuteAndFetch(oDb_, oOwners_); // Not NULL constraints
    oC1028.vExecuteAndFetch(oDb_, oOwners_); // triggers

    // Getting the attribute of segments.
    GM::cUserSegments oUserSegments;
    oQueue_.push_back(oUserSegments.oSubmit(*oSvc_, oOwners_, conf_.as<bool>("compress")));

    // Getting the columns forming key constraint.
    GM::cKeyColumns oKeyColumns;
    oQueue_.push_back(oKeyColumns.oSubmit(*oSvc_, oOwners_));

    // Getting expressions of function base index.
    GM::cAllIndExpressions oAllIndExpressions;
    oQueue_.push_back(oAllIndExpressions.oSubmit(*oSvc_, oOwners_));

    // Getting the subpartitioning key column name.
    GM::cAllSubPartKeyColumns oAllSubPartKeyColumns;
    oQueue_.push_back(oAllSubPartKeyColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the partitioning key column name.
    GM::cAllPartKeyColumns oAllPartKeyColumns;
    oQueue_.push_back(oAllPartKeyColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the default value.
    GM::cDataDefault oDataDefault;
    oQueue_.push_back(oDataDefault.oSubmit(*oSvc_, oOwners_));

    // Getting the column of the tables and views.
    GM::cAllTabColumns oAllTabColumns(
        iDependent_
        , conf_.as<int32_t>("longtransit")
        , oDataDefault
    );
    oQueue_.push_back(oAllTabColumns.oSubmit(*oSvc_, oOwners_, oKeyColumns));
    // Prepare the data by executing the query immediately (not asynchronously).
    {
        ps::lib::tSequence<ps::lib::sql::cFetchable>::auto_type unldr_(oQueue_.pop_back());
        unldr_ ->vExecuteAndFetch();
    }
    // Cloning oAllTabColumns.
    GM::cAllTabColumns oCloneForIndCols(oAllTabColumns)
                     , oCloneForIncluding(oAllTabColumns)
    ;

    // Getting the ind sub partition name.
    GM::cAllIndSubPartitions oAllIndSubPartitions;
    oQueue_.push_back(oAllIndSubPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting the table sub partition name.
    GM::cAllTabSubPartitions oAllTabSubPartitions;
    oQueue_.push_back(oAllTabSubPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting attribute of the sub partition templates.
    std::shared_ptr<GM::cAllSubPartitionTemplates> oAllSubPartitionTemplates(nullptr);
    if (oSvc_->iCompDbVersion("0902", 4) >= 0)
    {
        oAllSubPartitionTemplates.reset(
            new GM::cAllSubPartitionTemplates(oAllTabSubPartitions)
        );
        oQueue_.push_back(oAllSubPartitionTemplates->oSubmit(*oSvc_, oOwners_, oFunctionalReplace));
    }

    // Getting attributes of index column.
    GM::cAllIndColumns oAllIndColumns(oCloneForIndCols, oAllIndExpressions);
    oQueue_.push_back(oAllIndColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the ind partition name.
    GM::cAllIndPartitions oAllIndPartitions(oAllIndSubPartitions);
    oQueue_.push_back(oAllIndPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting the table partition name.
    GM::cAllTabPartitions oAllTabPartitions(oAllTabSubPartitions);
    oQueue_.push_back(oAllTabPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting attribute of partition index.
    GM::cAllPartIndexes oAllPartIndexes(
        oAllIndSubPartitions, oAllIndPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns
    );
    oQueue_.push_back(oAllPartIndexes.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of partition tables.
    GM::cAllPartTables oAllPartTables(
        oAllTabSubPartitions, oAllTabPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns, oAllSubPartitionTemplates
    );
    oQueue_.push_back(oAllPartTables.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of tables.
    GM::cAllTables oAllTables(oUserSegments, oAllPartTables);
    oQueue_.push_back(oAllTables.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of index.
    GM::cUserIndexes oUserIndexes(
        oAllIndColumns, oUserSegments
        , oAllTables, oCloneForIncluding, oAllPartIndexes
    );
    oQueue_.push_back(oUserIndexes.oSubmit(*oSvc_, oOwners_));

    /*
     * - This function executes asynchronously the task stacked in oQueue_.
     * - This thread blocks until all other threads have joined this thread.
     * - Elements of oQueue_ are consumed and released.
     */
    ps::lib::vSynchronize(iConcurrency_, oQueue_, &ps::lib::sql::cFetchable::vExecuteAndFetch);

    /*
     * Generating scripts for management.
     * These have SQL statement for following operation
     * drop index, disable constraints and triggers
     */
    GM::cConsValidate::tKeyTuple oKey; // szOwner + szTableName
    {
        const tWayToNext iWayToNext = tWayToNext::iForward;
        const auto& oTableList = oCopyDd_.oGetTableList();
        cStreamWrapper st_(mode_, output_, disable_deps_);
        /*
         * iForward: disable FK
         */
        st_
            << boost::format("spool %s.log")
                % disable_deps_.stem().string() << std::endl
            << std::string("timing start") << std::endl
            << boost::format("@@%s")
                % conn_sql_.stem().string() << std::endl
            ;
        size_t i = 0;
        for (const auto& tbl : oTableList)
        {
            oKey = std::make_tuple(tbl.sOwner.c_str(), tbl.sTable.c_str(), "");
            st_
                << GM::sPrintLabel("Refs", i + 1, tbl.sGetConcatenatedName())
                << GM::oPrintItems(oC1024, oKey, false, iWayToNext)
            ;
            ++i;
        }
        /*
         * iForward: disable trigger, disable AK, drop index
         */
        i = 0;
        for (const auto& tbl : oTableList)
        {
            oKey = std::make_tuple(tbl.sOwner.c_str(), tbl.sTable.c_str(), "");
            st_
                << GM::sPrintLabel("Deps", i + 1, tbl.sGetConcatenatedName())
                << GM::oPrintItems(oC1025, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1026, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1027, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1028, oKey, false, iWayToNext)
                << GM::oPrintItemsFN(
                    oUserIndexes
                    , &GM::cUserIndexes::vRptDrpCreUserIndexes
                    , oKey, false, iWayToNext
                )
            ;
            ++i;
        }
        st_
            << std::string("timing stop") << std::endl
            << std::string("spool off") << std::endl
            << std::string("exit") << std::endl
            ;
    }
    /*
     * In order to create and enable in the order opposite
     * to the order of drop and disable.
     */
    oC1024.vReverse();
    oC1025.vReverse();
    oC1026.vReverse();
    oC1027.vReverse();
    oC1028.vReverse();
    oUserIndexes.vReverse();
    /*
     * Generating scripts for management.
     * These have SQL statement for following operation
     * create index, enable constraints and triggers
     */
    {
        const tWayToNext iWayToNext = tWayToNext::iBackward;
        const auto& oTableList = oCopyDd_.oGetTableList();
        /*
         * iBackward: create index, enable AK, enable trigger
         */
        size_t i = 0;
        for (const auto& tbl : oTableList)
        {
            oKey = std::make_tuple(tbl.sOwner.c_str(), tbl.sTable.c_str(), "");
            cStreamWrapper st_(mode_, output_, tbl, sSuffixDeps_);
            st_
                << boost::format("spool %s.log")
                    % st_.sGetFilename().stem().string() << std::endl
                << std::string("timing start") << std::endl
                << boost::format("@@%s")
                    % conn_sql_.stem().string() << std::endl
                << GM::sPrintLabel("Deps", i + 1, tbl.sGetConcatenatedName())
                << GM::oPrintItemsFN(
                    oUserIndexes
                    , &GM::cUserIndexes::vRptDrpCreUserIndexes
                    , oKey, false, iWayToNext
                )
                << GM::oPrintItems(oC1028, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1027, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1026, oKey, false, iWayToNext)
                << GM::oPrintItems(oC1025, oKey, false, iWayToNext)
                << std::string("timing stop") << std::endl
                << std::string("spool off") << std::endl
                << std::string("exit") << std::endl
            ;
            ++i;
        }
        /*
         * iBackward: enable FK
         */
        i = 0;
        for (const auto& tbl : oTableList)
        {
            oKey = std::make_tuple(tbl.sOwner.c_str(), tbl.sTable.c_str(), "");
            cStreamWrapper st_(mode_, output_, tbl, sSuffixRefs_);
            st_
                << boost::format("spool %s.log")
                    % st_.sGetFilename().stem().string() << std::endl
                << std::string("timing start") << std::endl
                << boost::format("@@%s")
                    % conn_sql_.stem().string() << std::endl
                << GM::sPrintLabel("Refs", i + 1, tbl.sGetConcatenatedName())
                << GM::oPrintItems(oC1024, oKey, false, iWayToNext)
                << std::string("timing stop") << std::endl
                << std::string("spool off") << std::endl
                << std::string("exit") << std::endl
            ;
            ++i;
        }
    }
}

cUnload::cUnload()
    : oImpl_(new cUnloadImpl)
{}

void cUnload::vRun()
{
    oImpl_->vExpData();
    oImpl_->vMakeScripts();
}

} // ps::app::xtru::getdata

} // ps::app::xtru

} // ps::app

} // ps

