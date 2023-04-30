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

int main(int argc, char *argv[])
{
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
        ps::app::xtru::cAppConf oAppConf(conf);

        conf.oParse(argc, argv);

        const boost::filesystem::path sOutput(
            // A contains the delimiter '/' of the path, A is used as it is.
            // Otherwise, B/A is returned.
            ps::lib::sHasParentOrPrefixedPath(
                conf.as<std::string>("output")   // -- A
                , conf.sGetPsHome()              // -- B
            )
        );
        boost::system::error_code ec;
        boost::filesystem::exists(sOutput, ec);
        ASSERT_OR_RAISE(
            !ec, std::runtime_error
            , boost::format(
                R"(%s %s: %s: Check the value of startup parameter "output".)"
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
        if (conf.is_value("help"))
        {
            cout << boost::format(
                "Usage: %s [OPTION]... \n\n%s\n\n%sReport bugs to <%s>.")
            % PACKAGE_NAME % conf.sGetConst("title") % conf % PACKAGE_BUGREPORT  << std::endl;
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
        conf.vPrintAllKeyValuePairs(trc);

        std::unique_ptr<ps::app::xtru::cFeature> vFeature(
            ps::app::xtru::cFeature::oMakeInstance(conf.as<std::string>("feature"))
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
