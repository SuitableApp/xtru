/*
 
Copyright (C) 2023 SuitableApp

This file is part of Extreme Unloader(XTRU).

Extreme Unloader(XTRU) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Extreme Unloader(XTRU) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Extreme Unloader(XTRU).  If not, see <http://www.gnu.org/licenses/>.

*/

#include <pslib.h>
#include "cAppConf.h"
#include "cAllTables.h"
#include "cAllSource.h"
#include "cAllViews.h"

static void vRoutine(void);

int main(int argc, char *argv[])
{
    ps::lib::cRtn& rtn_ = ps::lib::cRtn::get_mutable_instance()(EXIT_SUCCESS);
    ps::lib::cConfigures& conf_ = ps::lib::cConfigures::get_mutable_instance();
    ps::lib::cConsole& cout_ = ps::lib::cConsole::get_mutable_instance();
    ps::lib::cTracer& trc_ = ps::lib::cTracer::get_mutable_instance();
    ps::lib::cDistributor& mos_ = ps::lib::cDistributor::get_mutable_instance()(cout_)(trc_);
    conf_.oAddConst("title", TITLE)
        .oAddConst("version", VERSION)
        .oAddConst("copyright", COPYRIGHT)
        ;
    try
    {
        ps::lib::cSignal sig;   // Hocking a new handler for SIGINT.
        ps::demo::cAppConf oAppConf(conf_);
        conf_.oParse(argc, argv);
        if (conf_.is_value("filetrace") && conf_.length("filetrace"))
        {
            trc_.oRedirectTo(conf_.as<std::string>("filetrace"));
        }
        if (conf_.is_value("filelog") && conf_.length("filelog"))
        {
            cout_.oRedirectTo(conf_.as<std::string>("filelog"));
        }
        if (argc < 2 || conf_.is_value("help"))
        {
            cout_ << boost::format("Usage: %s [OPTION]... \n\n%s\n\n") % PACKAGE % TITLE
             << boost::format("%sReport bugs to <%s>.") % conf_ % PACKAGE_BUGREPORT << std::endl;
            return rtn_;
        }
        if (conf_.is_value("version"))
        {
            cout_ << ps::lib::oNotice;
            return rtn_;
        }

        cout_.vSwitchTagDateTime();
        cout_ << ps::lib::oNotice;

        vRoutine();
    }
    catch (const std::exception& ex)
    {
        mos_ << boost::format("Caught an exception: %s") % ex.what() << std::endl;
        rtn_.vOrValue(true);
    }
    catch (...)
    {
        mos_ << std::string("Caught an exception: Unknown")  << std::endl;
        rtn_.vOrValue(true);
    }
    {
        if (rtn_ != EXIT_SUCCESS)
        {
            mos_ << boost::format(
                "Failed with an error."
                " See a trace-file %s to diagnose this incident."
            ) % trc_.sGetName();
        }
        else
        {
            mos_ << std::string(rtn_.iCotinue() ? "Completed successfully." : "Canceled by request.");
        }
        mos_ << std::endl;
    }
    return rtn_;
} 

void vRoutine(void)
{
    const std::string sOwner = "SCOTT";
//  ps::lib::cRtn& rtn_ = ps::lib::cRtn::get_mutable_instance();
    ps::lib::cConfigures& conf_ = ps::lib::cConfigures::get_mutable_instance();
    ps::lib::cDistributor& mos_ = ps::lib::cDistributor::get_mutable_instance();
    {
        const auto iCores = conf_.iGetNumPhisicalCores();
        mos_ << boost::format("There %s %d physical %s in this system.")
            % (iCores > 1 ? "are" : "is")
            % iCores
            % (iCores > 1 ? "cores" : "core")
            << std::endl;
    }
    // Concurrency setting.
    const int32_t iConcurrency = conf_.as<int32_t>("parallelism");
    {
        mos_ << boost::format("Concurrency: %d") % iConcurrency << std::endl;
    }

    // Initializing STREM LOCATOR.
    ps::lib::nsStreamLocator::vInitialize(
        // destination for output.
        // Scheme name is placed front of "://" and defines kind of redirector.
        "file://{O}/{C}.{X}"
        , 0 // parameter: stdout
        , "/tmp/xtru_output"
//      , conf_.sGetPsHome() / "output"  /// to be assigned for {O} macro.
        // Types of target file generated when table is unloaded.
        // These values are able to refer as:
        // ps::lib::nsStreamLocator::cStreamLocator::
        , {{"dat", "ctl", "clo", "blo"}}
        , false // False means that the control file is outputted.
    );

    // Connecting to database.
    std::string username, password, db_alias;
    ps::lib::vSplitUserid(conf_.as<std::string>("userid"), username, password, db_alias);
    ps::lib::sql::occi::cSvc::tPtr oSvc(ps::lib::sql::occi::cSvc::oMakeSvc(
        ps::lib::sql::occi::cSvc::tTypeOfPool::iDedicated
        , username, password, db_alias, iConcurrency, 0
    ));
    oSvc->vShowServerVersion();

    const uint32_t iBulkSize = 100;
    // cBind can be bind host-variables value to place-holders.
    // Longevity of the host-variable must be longer than ps::lib::sql::cFetchable object.
    ps::lib::sql::occi::cBind oBind;
    std::string s_job_name = "SALESMAN";
    int32_t s_low_sal = 0;
    oBind.vAddItem(":B_JOB", s_job_name);
    oBind.vAddItem(":B_SAL", &s_low_sal);

    ps::lib::tSequence<ps::lib::sql::cFetchable> unldrs;
    // cUnloader is a data converter for generic variable-length formatting that like the CSV.

    unldrs.push_back(new ps::lib::sql::occi::cUnloader(
        *oSvc, new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "DEPT", "")
        , iBulkSize, "select d1.* from DEPT d1; select d2.* from DEPT d2;", "DEPT"
        , ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN
    ));
    unldrs.push_back(new ps::lib::sql::occi::cUnloader(
        *oSvc, new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "EMP", "")
        , iBulkSize, "select * from EMP where JOB = :B_JOB or SAL > :B_SAL", "EMP"
        , ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind
    ));
    unldrs.push_back(new ps::lib::sql::occi::cUnloader(
        *oSvc, new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "SALGRADE", "")
        , iBulkSize, "select * from SALGRADE", "SALGRADE"
        , ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN
    ));
    /*
    unldrs.push_back(new ps::lib::sql::occi::cUnloader(
        *oSvc
        , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, "HASH_PART_TABLE", "")
        , iBulkSize, "select * from HASH_PART_LARGE_TABLE PARTITION(P01);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P02);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P03);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P04);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P05);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P06);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P07);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P08);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P09);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P10);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P11);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P12);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P13);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P14);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P15);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P16);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P17);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P18);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P19);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P20);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P21);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P22);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P23);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P24);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P25);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P26);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P27);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P28);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P29);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P30);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P31);"
          "select * from HASH_PART_LARGE_TABLE PARTITION(P32);"
        , "HASH_PART_TABLE"
        , ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN)
    );
    */
    // Following this example demonstrates a method of directing from a stream.
    // If arguments (oBind) passed to cUnloader's ctor, It will be able to select rows dinamicaly.
    boost::filesystem::path fn("queries.csv");
    {
        std::ofstream ofs(fn.string());
        ofs << "\"ALL_TAB_COLUMNS\",\"select * from ALL_TAB_COLUMNS\"" << std::endl;
        ofs << "\"ALL_CONS_COLUMNS\",\"select * from ALL_CONS_COLUMNS\"" << std::endl;
        ofs << "\"ALL_CONSTRAINTS\",\"select * from ALL_CONSTRAINTS\"" << std::endl;
    }
    if (boost::filesystem::exists(fn))
    {
        boost::filesystem::ifstream ifs;
        std::string line;
        ps::lib::tSep rule;
//      ifs.open("queries.cs_");
        ifs.open(fn);
        while (getline(ifs, line))
        {
            if (line[0] == '#') continue;
            boost::tokenizer< ps::lib::tSep > tokens(line, rule);
            auto it = tokens.begin();
            std::string tag = *it++;
            std::string qry = *it++;
            unldrs.push_back(new ps::lib::sql::occi::cUnloader(
                *oSvc
                , new ps::lib::nsStreamLocator::cStreamLocator(sOwner, tag, "")
                , iBulkSize, qry, tag, ps::lib::sql::occi::cUnloader::NO_LONG_COLUMN, &oBind
            ));
        }
    }

    // cAllTables is a example which retrieves data from ALL_TABLES and converts it to the user preference.
    // This implementation may be easy to understand if you are familiar with programming in C language style.
    // Data of the cursor one line will be stored to an instance of the user-defined structure.
    // For data transport efficient, instance is prepared as an array. 
    // And fetch is performed in the number of rows equal to the number of elements in it.
    // It is convenient to export row in the fact table.
    unldrs.push_back(new ps::demo::cAllTables(sOwner, *oSvc));

    // In cAllTables, structure array instance is repeatedly used across the initialization fetch until all.
    // However, implementation shown by cAllSource is whenever necessary added an array fetch until all.
    // Useful if needing before making (eg, joining or aggregation) calculation of what
    // all the rows of the query has been allocated.
    unldrs.push_back(new ps::demo::cAllSource(sOwner, *oSvc));

    // In cAllViews, example to export a query that includes a LONG column is shown.
    // You cannot know the length unless to read all the data of the LONG.
    // Fortunately, the OCI, for it is not necessary to allocate memory corresponding to 
    // the maximum length of the LONG, a feature called piecewise fetch are provided.
    // The piecewise fetch is because it is concealed, nightmare to implement it directly can be avoided.
    // But care must be taken that there is some difference in the type argument of the function call,
    unldrs.push_back(new ps::demo::cAllViews(sOwner, *oSvc));
    ps::lib::vSynchronize(iConcurrency, unldrs, &ps::lib::sql::cFetchable::vExecuteAndFetch);
}

