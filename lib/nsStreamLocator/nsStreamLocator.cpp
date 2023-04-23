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

namespace ps
{
namespace lib
{
namespace nsStreamLocator
{

// External identifiers.
static const std::string sDefaultStreamLocator_ = "file://{O}/{C}.{X}";
static std::string sSpecifiedStreamLocator_;
static int32_t iStdout_;
boost::filesystem::path sOutput_;
std::string sConnectTo_;
tExts oExts_;
tEnvMap oEnvMap_;
bool iSuppressCtrlf_;

const boost::regex regLocationExpr(R"(\A(?<scheme>[[:alpha:]_][\w]*):(//)?(?<location>.*)\z)");
const boost::regex regMacroSymbolExpr(R"(\{(?<var>[\u])(=(?<opt>.*?))?\})");

namespace
{

constexpr int MaxDateTimeLength = 100;
constexpr int MaxEnvValueLength = 8192;

std::string sFormatDateTime(
    const ps::lib::cMap<std::string, std::string>& cmap
    , const boost::regex& re
    , const std::string& pattern
){
    typedef const ps::lib::cMap<std::string, std::string> Map;
    static const std::pair <const char*
        , std::function<void(Map&, const std::string&, std::ostream&) >>
        funcs[] = {
        {
            "c"
            , [](Map& cmap, const std::string& s, std::ostream & out)
            {
                try
                {
                    out << cmap.at(s);
                }
                catch (std::out_of_range&)
                {
                    out << s;
                }
            }
        }
        , {
            "l"
            , [](Map& cmap, const std::string& s, std::ostream & out)
            {
                out << (s[0] != '\0' ? s : "'");
            }
        }
        , {
            "o"
            , [](Map& cmap, const std::string& s, std::ostream & out)
            {
                out << s;
            }
        }
    };

    std::stringstream fmt;
    boost::sregex_iterator it1(pattern.begin(), pattern.end(), re);
    boost::sregex_iterator it2;
    for (; it1 != it2; it1++)
    {
        auto& m = *it1;
        for (auto& func : funcs)
        {
            auto& s = m[func.first];
            if (s.matched)
            {
                func.second(cmap, s, fmt);
            }
        }
    }
    char buf[MaxDateTimeLength];
    ::time_t t = std::time(nullptr);
    ::tm tm;
    auto r = ::strftime(
        buf
        , sizeof (buf)
        , fmt.str().c_str()
        , ::localtime_r(&t, &tm));
    return r != 0 ? std::string(buf) : std::string();
}

/**
 * @brief
 * This function retrieves key-value formed pairs from the environment variables.
 * Then it organizes and returns a map of these pairs.
 */
tEnvMap oGetEnvValues()
{
    tEnvMap oMap_;
    /// @brief Object for trace output.
    auto& trc_ = ps::lib::cTracer::get_mutable_instance();
    static const boost::regex regExpr(
        R"((?<key>[^=]*)=((?<value>.*))?)"
    );
    FILE * fp_ = ::popen("printenv", "r");
    int eno = errno;
    BOOST_SCOPE_EXIT(&fp_)
    {
        if (fp_) ::pclose(fp_);
        fp_ = nullptr;
    }
    BOOST_SCOPE_EXIT_END;
    if (fp_)
    {
        boost::iostreams::stream<boost::iostreams::file_descriptor_source>
            is(::fileno(fp_), boost::iostreams::never_close_handle);
        trc_ << boost::format("Listing environment variables...") << std::endl;
        std::string line;
        boost::smatch tuple; // A Key-value pair extracted from the line.
        while (std::getline(is, line))
        {
            if (line[0] == ' ' || line[0] == '\t') continue;
            if (boost::regex_match(line, tuple, regExpr))
            {
                oMap_.insert(std::make_pair(tuple["key"], tuple["value"]));
                trc_ << boost::format("  %s=%s") % tuple["key"] % tuple["value"] << std::endl;
            }
            else
            {
                trc_ << boost::format("Failed to parse a environment value. line=%s") % line << std::endl;
            }
        }
    }
    else
    {
        RAISE_EX_CONVERT(std::runtime_error
            , boost::format("Failed to execute printenv. reason:%s") % ::strerror(eno));
    }
    return oMap_;
}

}

const ps::lib::cMap<std::string, tOstreamGenerator>
    oSchemeMap_ = {
    {
        "file"
        , [](const std::string& sPathToFile) { return new cFileSystem(sPathToFile); }
    }
    , {
        "ipc_pipe"
        , [](const std::string& sToExecuteDmdLine) { return new cLocalProcess(sToExecuteDmdLine); }
    }
    , {
        "named_pipe"
        , [](const std::string& sPathToNamedPipe) { return new cNamedPipe(sPathToNamedPipe); }
    }
};

const ps::lib::cMap<std::string, tMacroAction>
    oMacroMap_ = {
    {
        "O"
        , [](const tInitParams&, const std::string&, const tExtType&, const std::string&)
        {
            return sOutput_.string();
        }
    }
    , {
        "T"
        , [](const tInitParams& rInitParams, const std::string&, const tExtType&, const std::string&)
        {
            return rInitParams.sTableName;
        }
    }
    , {
        "P"
        , [](const tInitParams& rInitParams, const std::string&, const tExtType&, const std::string&)
        {
            return rInitParams.sPartitionName.size() ? rInitParams.sPartitionName : std::string("np");
        }
    }
    , {
        "C"
        , [](const tInitParams& rInitParams, const std::string&, const tExtType&, const std::string&)
        {
            return rInitParams.sPartitionName.size()
                ? rInitParams.sTableName + '_' + rInitParams.sPartitionName
                : rInitParams.sTableName
            ;
        }
    }
    , {
        "A"
        , [](const tInitParams&, const std::string&, const tExtType&, const std::string& sDataFileDir)
        {
            return sDataFileDir;
        }
    }
    , {
        "I"
        , [](const tInitParams& rInitParams, const std::string&, const tExtType&, const std::string&)
        {
            return rInitParams.sOwner;
        }
    }
    , {
        "X"
        , [](const tInitParams&, const std::string&, const tExtType& iExtType, const std::string&)
        {
            return oExts_[iExtType];
        }
    }
    , {
        "D"
        , [](const tInitParams&, const std::string& option, const tExtType&, const std::string&)
        {
            static const ps::lib::cMap<std::string, std::string> cmap = {
                {"d", "%-d"},
                {"dd", "%d"},
                {"ddd", "%a"},
                {"dddd", "%A"},
                {"M", "%-m"},
                {"MM", "%m"},
                {"MMM", "%b"},
                {"MMMM", "%B"},
                {"y", "%-y"},
                {"yy", "%y"},
                {"yyyy", "%Y"},
                {"gg", "%EC"},
                {"%", "%%"},
            };
            static const boost::regex re(
                R"((?<c>d+|M+|y+|g+|%)|'(?<l>.*?)'|(?<o>[^dMyg%']+|.))"
                );
            return sFormatDateTime(cmap, re, option);
        }
    }
    , {
        "W"
        , [](const tInitParams&, const std::string& option, const tExtType&, const std::string&)
        {
            static const ps::lib::cMap<std::string, std::string> cmap = {
                {"h", "%-I"},
                {"hh", "%I"},
                {"H", "%-H"},
                {"HH", "%H"},
                {"m", "%-M"},
                {"mm", "%M"},
                {"s", "%-S"},
                {"ss", "%S"},
                {"t", "%p"},
                {"tt", "%p"},
                {"%", "%%"},
            };
            static const boost::regex re(
                R"((?<c>h+|H+|m+|s+|t+|%)|'(?<l>.*?)'|(?<o>[^hHmst%']+|.))"
                );
            return sFormatDateTime(cmap, re, option);
        }
    }
    , {
        "E"
        , [](const tInitParams&, const std::string& option, const tExtType&, const std::string&)
        {
            const auto it = oEnvMap_.find(option);
            if (it != oEnvMap_.end())
            {
                ASSERT_OR_RAISE(it->second.size() < MaxEnvValueLength
                    , std::runtime_error
                    , "Environment variable value too long");
                return it->second;
            }
            else
            {
                return std::string("");
            }
        }
    }
};

/**
 * @brief
 * Checkng wheather initializing has done.
 */
bool iHasBeenInitialized(void)
{
    return sSpecifiedStreamLocator_.size() > 0;
}

/**
 * @brief
 * reset static member value.
 */
void vInitialize(
    const std::string& sStreamLocator
    , const int32_t iStdout
    , const boost::filesystem::path sOutput
    , const tExts oExts
    , const bool iSuppressCtrlf
){
    /*
     * Check and assign "sStreamLocator".
     */
    ASSERT_OR_RAISE(!sStreamLocator.empty()
        , std::runtime_error
        , "STREAM LOCATOR must not be empty. Must locator be "
          "previously initialized using vInitialize().");
    boost::smatch m;
    ASSERT_OR_RAISE(boost::regex_match(sStreamLocator, m, regLocationExpr)
        , std::runtime_error
        , boost::format(R"(Lexical error in sStreamLocator. "%s" does not match for regex:"%s".)")
          % sStreamLocator % regLocationExpr.str());
    const auto& scm = m["scheme"];
    ASSERT_OR_RAISE(oSchemeMap_.find(scm) != oSchemeMap_.end()
        , std::runtime_error
        , boost::format(
            R"(Appeared scheme "%s" in stream_locator:"%s" can not use. Choose one from %s.)"
            ) % scm % sStreamLocator % ps::lib::sGetKeyListOfMap(oSchemeMap_, {'\"','\"'})
        );
    const auto& loc = m["location"];
    ASSERT_OR_RAISE(!loc.str().empty()
        , std::runtime_error
        , "A location has not been specified");
    boost::sregex_iterator it1(loc.begin(), loc.end(), regMacroSymbolExpr);
    boost::sregex_iterator it2; // end of matching.
    while (it1 != it2)
    {
        ASSERT_OR_RAISE(oMacroMap_.find((*it1)["var"]) != oMacroMap_.end()
            , std::runtime_error
            , boost::format(
                R"(Appeared macro {%s} in stream_locator:"%s" can not use. Choose one from %s.)"
                ) % (*it1)["var"] % sStreamLocator % ps::lib::sGetKeyListOfMap(oMacroMap_, {'[',']'})
            );
        it1++;
    }
    sSpecifiedStreamLocator_ = sStreamLocator;
    iStdout_ = iStdout;
    /*
     * Check and assign "sOutput".
     */
    ASSERT_OR_RAISE(boost::filesystem::exists(sOutput)
        , std::runtime_error
        , boost::format("Path %s does not exists.") % sOutput);
    ASSERT_OR_RAISE(boost::filesystem::is_directory(sOutput)
        , std::runtime_error
        , boost::format("Path %s is not a directory.") % sOutput);
    sOutput_ = sOutput;

    oEnvMap_ = oGetEnvValues();

    /*
     * Check and assign "oExts".
     */
    static const boost::regex regIdentifierExpr(R"(\A([[:alpha:]_][\w]*)\z)");
    for (auto i = 0; i < iNumExtType; i++)
    {
        ASSERT_OR_RAISE(boost::regex_match(oExts[i], regIdentifierExpr)
            , std::runtime_error
            , boost::format(R"(Lexical error in oExts[%d]. "%s" does not match for regex:"%s".)")
              % i % oExts[i] % regIdentifierExpr.str());
    }
    oExts_ = oExts;

    /*
     * Check and assign "iSuppressCtrlf".
     */
    iSuppressCtrlf_ = iSuppressCtrlf;
}

std::string sGetStreamLocator(const tExtType& iExtType)
{
    ASSERT_OR_RAISE(iHasBeenInitialized()
        , std::runtime_error
        , "vInitialize() must be previously called.");

    std::string sRet;
    switch (iExtType)
    {
    case iExtData:
        sRet = (iStdout_ & 0x1) ? sSpecifiedStreamLocator_ : sDefaultStreamLocator_;
        break;
    case iExtCtrl:
        sRet = (iStdout_ & 0x2) ? sSpecifiedStreamLocator_ : sDefaultStreamLocator_;
        break;
    case iExtClob: case iExtBlob:
        sRet = (iStdout_ & 0x3) ? sSpecifiedStreamLocator_ : sDefaultStreamLocator_;
        break;
    default:
        RAISE_EX_CONVERT(std::logic_error
            , boost::format("Unexpected extention-type specified. (value=%d)") % iExtType);
    }
    return sRet;
}

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
