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
#include <mpx.h>

int main(int argc, char *argv[], const char** envp)
{
    auto& env_ = ps::lib::cEnv::get_mutable_instance()(envp);
    auto& loc_ = ps::lib::cLocale::get_mutable_instance();
    auto& rc = ps::lib::cRtn::get_mutable_instance()(EXIT_SUCCESS);
    auto& conf = ps::lib::cConfigures::get_mutable_instance();
    auto& cout = ps::lib::cConsole::get_mutable_instance();
    auto& trc = ps::lib::cTracer::get_mutable_instance();
    auto& mos_ = ps::lib::cDistributor::get_mutable_instance()(cout)(trc);
    auto& stat_ = ps::lib::cStat::get_mutable_instance();
    loc_.szSetLocale(LC_CTYPE, PLATFORM_LC_TYPE_STR);
    conf.oAddConst("title", TITLE)
        .oAddConst("version", VERSION)
        .oAddConst("copyright", COPYRIGHT)
        ;
    try
    {
        // Hocking a new handler for Unix signal (e.g. SIGINT and SIGTERM).
        ps::lib::cSignal sig(std::bind(&ps::lib::cRtn::vBreak, &rc));

        // Does product home directory exists ?
        if (ps::lib::iCheckHomeOrMakeIt(conf.sGetPsHome()))
        {
            mos_ << std::string("Processing was interrupted.") << std::endl;
            return rc;
        }
        // Does xtru.conf on the product home directory exists ?
        if (ps::lib::iCheckConfOrMakeIt(conf.sGetPsHome(), PKG_BASE_DIR, PKG_DATA_DIR, PACKAGE_NAME))
        {
            mos_ << std::string("Processing was interrupted.") << std::endl;
            return rc;
        }
        ps::app::mpx::cAppConf oAppConf(conf);

        conf.oParse(argc, argv);

        const boost::filesystem::path sOutput(
            // A contains the delimiter '/' of the path, A is used as it is.
            // Otherwise, B/A is returned.
            ps::lib::sHasParentOrPrefixedPath(
                conf.as<boost::filesystem::path>("startup")   // -- A
                , conf.sGetPsHome()              // -- B
            )
        );
        boost::system::error_code ec;
        boost::filesystem::exists(sOutput, ec);
        ASSERT_OR_RAISE(
            !ec, std::runtime_error
            , boost::format(
                R"(%s %s: %s: Check the value of startup parameter "startup".)"
            ) % sClass(ps::lib::E) % ec.message() % sOutput
        );

        if (conf.is_value("filetrace") && conf.length("filetrace"))
        {
            trc.oRedirectTo(
                ps::lib::sHasParentOrPrefixedPath(
                    conf.as<std::string>("filetrace"), sOutput
                )
            );
        }
        if (conf.is_value("filelog") && conf.length("filelog"))
        {
            cout.oRedirectTo(
                ps::lib::sHasParentOrPrefixedPath(
                    conf.as<std::string>("filelog"), sOutput
                )
            );
        }
        if (conf.is_value("help") || argc < 2)
        {
            cout << boost::format("Usage:") << std::endl
                << boost::format("  %s [-h|--help] [-v|--version]") % PACKAGE_NAME << std::endl
                << std::string("      [-c|--config file_name]") << std::endl
                << std::string("      [-p|--parallelism integer_as_parallel_degree]") << std::endl
                << std::string("      [-n|--nice integer_as_nice_degree]") << std::endl
                << std::string("      [-m|--mpxlist m_exe1[,m_exe2]...]") << std::endl
                << std::string("      [-x|--ignlist i_exe1[,i_exe2]...]") << std::endl
                << std::string("      [-d|--startup startup_dir]") << std::endl
                << std::string("      [--csi string_for_send_to_child_proc_stdin]") << std::endl
                << std::string("      [--cso filename_for_recieve_from_child_proc_stdout]") << std::endl
                << std::string("      [--cse filename_for_recieve_from_child_proc_stderr]") << std::endl
                << std::string("      <-s|--cmd \"cmd_file_name [arg0[ arg1][ arg2]...]\"") << std::endl
                << std::string("           | \"cmd_file_name [arg0[ arg1][ arg2]...]\"") << std::endl
                << std::string("           >") << std::endl
                << std::endl
                << std::string("  [] encloses optional symbols.") << std::endl
                << std::string("  <> encloses mandatory symbols.") << std::endl
                << std::string("  '|' separates alternative symbols.") << std::endl
                << std::string("  '-' can be used instead of '/'.") << std::endl
                << std::string("  '...' means the symbol immediately before can be repeated once or more.") << std::endl
                << std::endl
                << std::string("  The argument specified for argN can be referred to in the") << std::endl
                << std::string("  placeholder written as $N in cmd_file_name (N=0,1,2,3,...,k)") << std::endl
                << std::endl
                << std::string("  In the cmd_file_name environment variable values can be") << std::endl
                << std::string("  referenced in a press folder labeled $<envname>.") << std::endl
                << std::endl
                << conf 
                << boost::format("Report bugs to <%s>.")
                    % PACKAGE_BUGREPORT
                << std::endl;
            return rc;
        }
        if (conf.is_value("version"))
        {
            cout << ps::lib::oNotice;
            return rc;
        }

        cout.vSwitchTagDateTime();
        trc.vSwitchTagDateTime();
        cout << ps::lib::oNotice;
        mos_ << std::string("Locale categories are ") << loc_ << "." << std::endl;
        mos_ << boost::format("Home directory is %s.") % conf.sGetPsHome() << std::endl;
        mos_ << boost::format("Startup parameters are defined by %s.") % conf.sGetConfFile() << std::endl;
        {
            const auto iPhy = conf.iGetNumPhisicalCores();
            const auto iLog = conf.iGetNumLogicalCores();
            mos_ << boost::format("This computer has %d physical %s and %d logical %s.")
                % iPhy % (iPhy > 1 ? "cores" : "core")
                % iLog % (iLog > 1 ? "cores" : "core")
                << std::endl;
        }
        // Print all startup parameters into the trace file.
        conf.vPrintAllKeyValuePairs(trc);
        // Print all environmental variables into the trace file.
        env_.vPrintAllEnvVariables(trc);

        std::unique_ptr<ps::app::mpx::cFeature> vFeature(
            ps::app::mpx::cFeature::oMakeInstance()
        );
        vFeature->vRun();
    }
    catch (const std::exception& ex)
    {
        mos_ << boost::format("Caught an exception: %s") % ex.what() << std::endl;
        rc.vOrValue(true);
    }
    {
        if (rc != EXIT_SUCCESS)
        {
            mos_ << std::string("Failed with an error.") << std::endl
                << boost::format("See a trace-file %s to diagnose this incident") % trc.sGetName();
        }
        else
        {
            mos_ << std::string(rc.iCotinue() ? "Completed successfully" : "Canceled by request");
        }
        const auto iMiSec = stat_.iDurationMilliSeconds();
        const auto iDataRate = stat_.iGetOutputBytes() * 1000 / iMiSec;
        mos_ << boost::format(". Took %.2f seconds. Data rate %siB/sec.")
            % ps::lib::sIntToa(iMiSec / 1000)
            % ps::lib::sBinIntToIntStr(iDataRate)
            << std::endl;
    }
    return rc;
}
