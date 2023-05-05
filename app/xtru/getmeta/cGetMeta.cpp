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

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cGetMetaImpl
 * @brief Implementation of the class cGetMeta.
 */
class cGetMetaImpl
    : private cStartValues
{
private:
    const std::string sTag_;
    /**
     * @brief
     * name refers to the filename when saving the output result of getmeta function.
     * When the directory name is omitted, it is saved in output.
     * Optionally a directory name can be prefixed to name.
     */
    boost::filesystem::path filemeta_;
    /**
     * @brief
     * The spool file is where the console redirects when @ref filemeta_ is executed.
     * Only the base is inherited, but the .log extension is given.
     */
    boost::filesystem::path spoolfn_;
    boost::posix_time::ptime oStartDtm_;
    const boost::filesystem::fstream::ios_base::openmode mode_;
    cPrivsGiven& oPrivsGiven_;
    const bool iMetaDiv_;
	const bool iDependent_;
    const std::string sDateMask_;
    const std::string sTimestampMask_;
    const std::string sTimestampTzMask_;
    ps::lib::tPtrFstream st_meta_;
    cNumObjs oNumObjs_;
    /// @copybrief cFunctionalReplace
    cFunctionalReplace oFunctionalReplace;
public:
    cGetMetaImpl();
    ~cGetMetaImpl();
    void vRun();
};

void cGetMetaImpl::vRun()
{
    /// A queue for the tasks of extracting the datas from RDBMS.
    ps::lib::tSequence<ps::lib::sql::cFetchable> oQueue_;
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    *st_meta_
        << boost::format("set feedback off linesize 250 trimspool on tab off echo on verify off;") << std::endl
        << boost::format("set concat off define on pagesize 1000;") << std::endl
		<< boost::format("column ERROR format a80") << std::endl
		<< boost::format("spool %s;") % spoolfn_.string() << std::endl
        << boost::format("/*") << std::endl
        << boost::format(" * Date and time                   = %s") % boost::posix_time::to_simple_string(oStartDtm_) << std::endl
        << boost::format(" * Net service name                = %-64s") % oSvc_->sGetNetServiceName() << std::endl
        << boost::format(" * Release identifier              = %s") % oSvc_->sGetServerVersion() << std::endl
        << boost::format(" * Target schema(s)                = %-30s") % oOwners_ << std::endl
        << boost::format(" * Produced by                     = %-30s") % oSvc_->sGetSchemaName() << std::endl
        << oNumObjs_
        << boost::format(" *") << std::endl
        << boost::format(" * This script file was described by %s version %s.")
            % conf_.sGetConst("title") % conf_.sGetConst("version") << std::endl
        << boost::format(" * %s") % conf_.sGetConst("copyright") << std::endl
        << boost::format(" *")  << std::endl
        << boost::format(" */") << std::endl
        << boost::format("timing start;") << std::endl
    ;
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
        .iCheckPrivsGiven(true) // true: Shortage is ignored.
    ;
    mos_ << boost::format(R"(Passed the privilege check. (N=%d))") % iNumCheckedTabs << std::endl;

    // Register convertion rules.
    // cFunctionalReplace::operator()("As is", "To be")
    oFunctionalReplace
        ("DEFAULT", "default")
        ("MAXVALUE", "maxvalue")
        ("NULL", "null")
        (
            "TO_DATE(' %4s-%2s-%2s 00:00:00', 'S%4s-%2s-%2s HH24:MI:SS', "
            "'NLS_CALENDAR=GREGORIAN')", "to_date('{0}-{1}-{2}','{3}-{4}-{5}')")
    ;

    // Getting the objets.
    cAllObjects oAllObjects;
    oQueue_.push_back(oAllObjects.oSubmit(*oSvc_, oOwners_));

    // Getting the object privileges.
    cAllTabPrivs oAllTabPrivs;
    oQueue_.push_back(oAllTabPrivs.oSubmit(*oSvc_, oOwners_));

    // Getting the option of object audit.
    cUserObjAuditOpts oUserObjAuditOpts;
    oQueue_.push_back(oUserObjAuditOpts.oSubmit(*oSvc_, oOwners_));

    // Getting the audit trails.
    cUserAuditObject oUserAuditObject;
    oQueue_.push_back(oUserAuditObject.oSubmit(*oSvc_, oOwners_
            , conf_.as<std::int32_t>("numdays_audit")));

    // Getting the database links.
    cUserDbLinks oUserDbLinks(
        oUserAuditObject
    );
    oQueue_.push_back(oUserDbLinks.oSubmit(*oSvc_, oOwners_));

    // Getting the sequences.
    cAllSequences oAllSequences(
        oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
    );
    oQueue_.push_back(oAllSequences.oSubmit(*oSvc_, oOwners_));

    // Getting the source codes.
    cAllSource oAllSource;
    oQueue_.push_back(oAllSource.oSubmit(*oSvc_, oOwners_));

    // Getting the table comments.
    cAllTabComments oAllTabComments;
    oQueue_.push_back(oAllTabComments.oSubmit(*oSvc_, oOwners_, conf_.as<int32_t>("cmntlvl")));

    // Getting the column comments.
    cAllColComments oAllColComments;
    oQueue_.push_back(oAllColComments.oSubmit(*oSvc_, oOwners_, conf_.as<int32_t>("cmntlvl")));

    // Getting the attribute of segments.
    cUserSegments oUserSegments;
    oQueue_.push_back(oUserSegments.oSubmit(*oSvc_, oOwners_, conf_.as<bool>("compress")));

    // Getting the constraints.
    cAllConstraints oConstraints;
    oQueue_.push_back(oConstraints.oSubmit(*oSvc_, oOwners_));

    // Getting the columns forming key constraint.
    cKeyColumns oKeyColumns;
    oQueue_.push_back(oKeyColumns.oSubmit(*oSvc_, oOwners_));
    // Load data instantly.
    {
        ps::lib::tSequence<ps::lib::sql::cFetchable>::auto_type unldr_(oQueue_.pop_back());
        unldr_ ->vExecuteAndFetch();
    }

    // Getting the default value.
    cDataDefault oDataDefault;
    oQueue_.push_back(oDataDefault.oSubmit(*oSvc_, oOwners_));

    // Getting the column of the tables and views.
    cAllTabColumns oAllTabColumns(
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
    cAllTabColumns oCloneForConsCols(oAllTabColumns)
                 , oCloneForIndCols(oAllTabColumns)
                 , oCloneForIncluding(oAllTabColumns)
                 , oCloneForMVLogFilter(oAllTabColumns)
    ;

    // Getting the subpartitioning key column name.
    cAllSubPartKeyColumns oAllSubPartKeyColumns;
    oQueue_.push_back(oAllSubPartKeyColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the partitioning key column name.
    cAllPartKeyColumns oAllPartKeyColumns;
    oQueue_.push_back(oAllPartKeyColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the table sub partition name.
    cAllTabSubPartitions oAllTabSubPartitions;
    oQueue_.push_back(oAllTabSubPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting attribute of the sub partition templates.
    std::shared_ptr<cAllSubPartitionTemplates> oAllSubPartitionTemplates(nullptr);
    if (oSvc_->iCompDbVersion("0902", 4) >= 0)
    {
        oAllSubPartitionTemplates.reset(
            new cAllSubPartitionTemplates(oAllTabSubPartitions)
        );
        oQueue_.push_back(oAllSubPartitionTemplates->oSubmit(*oSvc_, oOwners_, oFunctionalReplace));
    }

    // Getting the table partition name.
    cAllTabPartitions oAllTabPartitions(oAllTabSubPartitions);
    oQueue_.push_back(oAllTabPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting attribute of partition tables.
    cAllPartTables oAllPartTables(
        oAllTabSubPartitions, oAllTabPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns, oAllSubPartitionTemplates
    );
    oQueue_.push_back(oAllPartTables.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of tables.
    cAllTables oAllTables(oUserSegments, oAllPartTables);
    oQueue_.push_back(oAllTables.oSubmit(*oSvc_, oOwners_));

    // Getting expressions of function base index.
    cAllIndExpressions oAllIndExpressions;
    oQueue_.push_back(oAllIndExpressions.oSubmit(*oSvc_, oOwners_));

    // Getting attributes of index column.
    cAllIndColumns oAllIndColumns(oCloneForIndCols, oAllIndExpressions);
    oQueue_.push_back(oAllIndColumns.oSubmit(*oSvc_, oOwners_));

    // Getting the ind sub partition name.
    cAllIndSubPartitions oAllIndSubPartitions;
    oQueue_.push_back(oAllIndSubPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting the ind partition name.
    cAllIndPartitions oAllIndPartitions(oAllIndSubPartitions);
    oQueue_.push_back(oAllIndPartitions.oSubmit(*oSvc_, oOwners_, oFunctionalReplace));

    // Getting attribute of partition index.
    cAllPartIndexes oAllPartIndexes(
        oAllIndSubPartitions, oAllIndPartitions, oAllPartKeyColumns
        , oAllSubPartKeyColumns
    );
    oQueue_.push_back(oAllPartIndexes.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of index.
    cUserIndexes oUserIndexes(
        oAllIndColumns, oUserSegments
        , oAllTables, oCloneForIncluding, oAllPartIndexes
    );
    oQueue_.push_back(oUserIndexes.oSubmit(*oSvc_, oOwners_));

    // Getting hash function..
    cHashExpr oHashExpr;
    oQueue_.push_back(oHashExpr.oSubmit(*oSvc_, oOwners_));

    // Getting attribute of cluster.
    cAllClusters oAllClusters(
        oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
        , oUserSegments, oAllTabColumns, oHashExpr, oUserIndexes
    );
    oQueue_.push_back(oAllClusters.oSubmit(*oSvc_, oOwners_));

    // Getting keys for clusters.
    cUserCluColumns oUserCluColumns;
    oQueue_.push_back(oUserCluColumns.oSubmit(*oSvc_, oOwners_));

    // Getting effective constraints.
    cEffectiveKeyCons oEffectiveKeyCons;
    oQueue_.push_back(oEffectiveKeyCons.oSubmit(*oSvc_, oOwners_));

    // Getting Tables referenced by FK.
    cParentWithChild oParentWithChild;
    oQueue_.push_back(oParentWithChild.oSubmit(*oSvc_, oOwners_));

    // Getting column names that compose constraints (PK, AK, FK).
    cAllConsColumns oAllConsColumnsKey(true, oCloneForConsCols);;
    oQueue_.push_back(oAllConsColumnsKey.oSubmit(*oSvc_, oOwners_));

    // Getting Column names that constitutes the AK referenced by FK.
    cAllConsColumns oAllConsColumnsRef(false, oCloneForConsCols);
    oQueue_.push_back(oAllConsColumnsRef.oSubmit(*oSvc_, oOwners_));

    // Getting check constraints.
    cConsOfCheck oConsOfCheck;
    oQueue_.push_back(oConsOfCheck.oSubmit(*oSvc_, oOwners_));

    // Getting referensial constraints.
    cConsOfFKey oConsOfFKey(oAllConsColumnsKey, oAllConsColumnsRef);
    oQueue_.push_back(oConsOfFKey.oSubmit(*oSvc_, oOwners_));

    // Getting alternate key constraints.
    cConsOfAKey oConsOfAKey(oAllConsColumnsKey);
    oQueue_.push_back(oConsOfAKey.oSubmit(*oSvc_, oOwners_));

    // Getting lobs.
    cAllLobs oAllLobs(oUserSegments);
    oQueue_.push_back(oAllLobs.oSubmit(*oSvc_, oOwners_));

    // Getting triggers.
    cAllTriggers oAllTriggers;
    oQueue_.push_back(oAllTriggers.oSubmit(*oSvc_, oOwners_));

    // Getting mview log filter columns.
    std::shared_ptr<cDbaMviewLogFilterCols> oDbaMviewLogFilterCols(nullptr);
    if (oPrivsGiven_.iAccesible({"SYS", "DBA_MVIEW_LOG_FILTER_COLS", "SELECT"}))
    {
        oDbaMviewLogFilterCols.reset(
            new cDbaMviewLogFilterCols(oCloneForMVLogFilter)
        );
        oQueue_.push_back(oDbaMviewLogFilterCols->oSubmit(*oSvc_, oOwners_));
    }

    // Getting mview log properties.
    cAllMviewLogs oAllMviewLogs(
        oUserSegments, oAllTabPrivs, oUserObjAuditOpts
        , oUserAuditObject, oAllTabComments, oAllColComments
        , oDbaMviewLogFilterCols
    );
    oQueue_.push_back(oAllMviewLogs.oSubmit(*oSvc_, oOwners_));

    // Getting location of the external data.
    cAllExternalLocations oAllExternalLocations;
    oQueue_.push_back(oAllExternalLocations.oSubmit(*oSvc_, oOwners_));

    // Getting information of the external data.
    cAllExternalTables oAllExternalTables(oAllExternalLocations);
    oQueue_.push_back(oAllExternalTables.oSubmit(*oSvc_, oOwners_));

    // Getting supplimental log group columns.
    cAllLogGroupColumns oAllLogGroupColumns;
    oQueue_.push_back(oAllLogGroupColumns.oSubmit(*oSvc_, oOwners_));

    // Getting supplimental log group properties.
    cAllLogGroups oAllLogGroups(oAllLogGroupColumns);
    oQueue_.push_back(oAllLogGroups.oSubmit(*oSvc_, oOwners_));

    // Getting the order of outputting. (APLDDT02)
    cOutputOrderTbl oOutputOrderTbl(
        iDependent_
        , oAllTabPrivs
        , oUserObjAuditOpts
        , oUserAuditObject
        , oAllTabColumns
        , oAllTables
        , oAllTabComments
        , oAllColComments
        , oUserCluColumns
        , oEffectiveKeyCons
        , oParentWithChild
        , oConsOfCheck
        , oConsOfFKey
        , oConsOfAKey
        , oAllLobs
        , oDataDefault
        , oAllTriggers
        , oAllMviewLogs
        , oUserIndexes
        , oAllExternalTables
        , oAllLogGroups
    );
    oQueue_.push_back(oOutputOrderTbl.oSubmit(*oSvc_, oOwners_));

    // Getting the properties of the materialized view.
    cAllMviews oAllMviews(
		oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject, oConsOfAKey
		, oEffectiveKeyCons, oAllTables, oAllTriggers, oAllMviewLogs
		, oAllTabComments , oAllColComments, oUserCluColumns, oUserIndexes
    );
    oQueue_.push_back(oAllMviews.oSubmit(*oSvc_, oOwners_));

    // Getting tables referring to other tables.
    cTablesWithFk oTablesWithFk(iDependent_, oEffectiveKeyCons, oConsOfFKey);
    oQueue_.push_back(oTablesWithFk.oSubmit(*oSvc_, oOwners_));

    // Getting elements of the refresh group.
    cAllRefreshChildren oAllRefreshChildren;
    oQueue_.push_back(oAllRefreshChildren.oSubmit(*oSvc_, oOwners_));

    // Getting elements of the refresh group.
    cAllRefresh oAllRefresh(sDateMask_, oAllRefreshChildren);
    oQueue_.push_back(oAllRefresh.oSubmit(*oSvc_, oOwners_));

    // Getting information of the view.
    cAllViews oAllViews(
		oAllTabPrivs, oUserObjAuditOpts, oUserAuditObject
		, oAllTriggers, oAllTabComments, oAllColComments, oAllTabColumns
    );
    oQueue_.push_back(oAllViews.oSubmit(*oSvc_, oOwners_));

    // Getting information of the synonym.
    cAllSynonyms oAllSynonyms(oUserAuditObject);
    oQueue_.push_back(oAllSynonyms.oSubmit(*oSvc_, oOwners_));

    // Getting information of the job.
    cUserJobs oUserJobs(sDateMask_);
    oQueue_.push_back(oUserJobs.oSubmit(*oSvc_, oOwners_));

    /*
     * - This function executes asynchronously the task stacked in oQueue_.
     * - This thread blocks until all other threads have joined this thread.
     * - Elements of oQueue_ are consumed and released.
     */
    ps::lib::vSynchronize(iConcurrency_, oQueue_, &ps::lib::sql::cFetchable::vExecuteAndFetch);

    /*
     * Printing script for the schema definition.
     */
    for (const std::string& sOwner: oOwners_)
    {
        const auto szOwner = sOwner.c_str();
        // Instruction to the SQL*Plus.
        *st_meta_
            << boost::format(R"(alter session set current_schema = %s;)") % sOwner << std::endl
            << oPrintItems(oUserDbLinks, std::make_tuple(szOwner), true)
            << std::string("set define off;") << std::endl
            << oPrintItems(oAllSequences, std::make_tuple(szOwner), true)
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "TYPE"), "Type")
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "PACKAGE"), "Pckg")
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "FUNCTION"), "Func")
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "PROCEDURE"), "Proc")
            << oPrintItems(oAllClusters, std::make_tuple(szOwner), true)
            << oPrintItems(oOutputOrderTbl, std::make_tuple(szOwner), true)
            << oPrintItems(oAllMviews, std::make_tuple(szOwner), true)
            << oPrintItems(oTablesWithFk, std::make_tuple(szOwner), true)
            << oPrintItems(oAllRefresh, std::make_tuple(szOwner), true)
            << oPrintItems(oAllViews, std::make_tuple(szOwner), true)
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "TYPE BODY"), "Typb")
            << oAllObjects.oPrintItems(oAllSource, std::make_tuple(szOwner, "PACKAGE BODY"), "Pkgb")
            << oPrintItems(oAllSynonyms, std::make_tuple(szOwner), true)
            << oPrintItems(oUserJobs, std::make_tuple(szOwner), true)
            << std::string("set define on;") << std::endl
            ;
    }
    /*
     * Output statistical information on the number of lines before
     * oPrintItems function is called. Because this function consumes
     * the line after script output.
     */
    oNumObjs_.vSetValues(
        oUserDbLinks.iGetNumRows()           // database link
        , oAllSequences.iGetNumRows()        // sequence
        , oAllObjects.iGetNumRows("Type")    // types
        , oAllObjects.iGetNumRows("Pckg")    // packages
        , oAllObjects.iGetNumRows("Func")    // functions
        , oAllObjects.iGetNumRows("Proc")    // procedures
        , oAllClusters.iGetNumRows()         // cluster
        , oOutputOrderTbl.iGetNumRows()      // table
        , oAllMviews.iGetNumRows()           // materialized view
        , oTablesWithFk.iGetNumRows()        // table that included FK.
        , oAllRefresh.iGetNumRows()          // refresh group
        , oAllViews.iGetNumRows()            // view
        , oAllObjects.iGetNumRows("Typb")    // type bodies
        , oAllObjects.iGetNumRows("Pkgb")    // package bodies
        , oAllSynonyms.iGetNumRows()         // synonym
        , oUserJobs.iGetNumRows()            // job queue
    );
    /*
     * Dependent type.
     */
    *st_meta_
        << std::string("timing stop;") << std::endl
        << std::string("spool off;") << std::endl
        << std::string("exit;") << std::endl
        << oNumObjs_
        ;
    st_meta_.reset();
}

cGetMetaImpl::cGetMetaImpl()
    : sTag_("GetMeta")
    , filemeta_(ps::lib::sHasParentOrPrefixedPath(conf_.as<std::string>("filemeta"), output_))
    , spoolfn_( filemeta_.filename().replace_extension(".log")) // rid the directory name.
    , oStartDtm_(boost::posix_time::second_clock::local_time())
    , mode_(
        boost::filesystem::fstream::ios_base::out
        | boost::filesystem::fstream::ios_base::trunc
    )
    , oPrivsGiven_(cPrivsGiven::get_mutable_instance())
    , iMetaDiv_(conf_.as<bool>("metadiv"))
    , iDependent_(conf_.as<bool>("dependent"))
    , sDateMask_(conf_.as<std::string>("date_mask"))
    , sTimestampMask_(conf_.as<std::string>("timestamp_mask"))
    , sTimestampTzMask_(conf_.as<std::string>("timestamp_tz_mask"))
{
    oPrivsGiven_.vSwitchTolerance();
    oPrivsGiven_.vSetService(*oSvc_);
    mos_ << boost::format("*** Started %s ***") % sTag_ << std::endl;
    boost::system::error_code ec;
    boost::filesystem::exists(filemeta_.parent_path(), ec);
    ASSERT_OR_RAISE(
        !ec, std::runtime_error
        , boost::format("%s: %s") % filemeta_.parent_path() % ec.message()
    );
    st_meta_ = ps::lib::open_with_check(filemeta_, mode_);
//  *st_meta_ << std::string(BOM_OF_UTF8);
    mos_ << boost::format("Script will be stored in %s") % filemeta_ << std::endl;
}

cGetMetaImpl::~cGetMetaImpl()
{
    mos_ << boost::format("*** Finished %s ***") % sTag_ << std::endl;
}

cGetMeta::cGetMeta()
    : oImpl_(new cGetMetaImpl)
{}

void cGetMeta::vRun()
{
    oImpl_->vRun();
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
