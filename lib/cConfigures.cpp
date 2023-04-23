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

#define EXCPT_ARGS_PARSE(ex) \
    catch (const ex& e) { \
        const std::string evt = (boost::format("thrown %s") % #ex).str(); \
        const ps::lib::cVector<const char *> vct(&argv[1], &argv[argc]); \
        ps::lib::vRaiseWithMsg<std::runtime_error>(evt.c_str() \
            , __PRETTY_FUNCTION__, __FILE__, __LINE__,  boost::format( \
            "%s: Command arguments are %s:") \
            % e.what() % vct \
        ); \
    }

#define EXCPT_CONF_PARSE(ex) \
    catch (const ex& e) { \
        const std::string evt = (boost::format("thrown %s") % #ex).str(); \
        ps::lib::vRaiseWithMsg<std::runtime_error>(evt.c_str() \
            , __PRETTY_FUNCTION__, __FILE__, __LINE__,  boost::format( \
            "%s: Check your conf=%s:") \
             % e.what() % sConfFile_ \
        ); \
    }

#define EXCPT_OTHER_HNDL(ex) \
    catch (const ex& e) { \
        const std::string evt = (boost::format("thrown %s") % #ex).str(); \
        ps::lib::vRaiseWithMsg<std::runtime_error>(evt.c_str() \
            , __PRETTY_FUNCTION__, __FILE__, __LINE__,  boost::format( \
            "%s:") \
             % e.what() \
        ); \
    }


namespace ps
{

namespace lib
{

namespace po = boost::program_options;

const char cConfigures::cSubstituter::szPlaceHolder[] = {R"(\$(\w+|\{([^\}]+)\}))"};
const char cConfigures::cSubstituter::szVariableName[] = {R"(\w+)"};

std::string cConfigures::cSubstituter::operator()(const boost::cmatch& match)
{
    const std::string sMatched(match.str());
    const boost::regex re(szVariableName);
    boost::cmatch var_name;
    boost::regex_search(sMatched.c_str(), var_name, re);
    const char* ptr = std::getenv(var_name.str().c_str());
    return nullptr == ptr ? sMatched : std::string(ptr);
}

int32_t cConfigures::iValidate(std::ostream& os) const
{
    BOOST_ASSERT(vm_.size() > 0);
    int32_t iErrors = 0;
    if (extdr_)
    {
        iErrors += extdr_->iValidate(os, vm_);
    }
    return iErrors;
}

cConfigures::cConfigures()
    // physical_concurrency() was supported by boost 1.55 or later.
    : cout_(ps::lib::cConsole::get_mutable_instance())
    , iNumPhisicalCores_(boost::thread::physical_concurrency())
    , iNumLogicalCores_(boost::thread::hardware_concurrency())
    , line_length_(100)
    , only_cmd_opts_("Only command options", line_length_)
    , cmd_or_conf_opts_("Cmmand or conf-file options", line_length_)
    , extdr_(nullptr)
    , iArgc_(0)
    , oArgv_(nullptr)
{
    try
    {
        // Declare a group of options that will be
        // allowed only in environmental variables.
        po::options_description only_env_opts;
        // Parsing environment variables.
        only_env_opts.add_options()
        ("SA_HOME"  // A home directory for SuitableApp's products.
             , po::value<boost::filesystem::path>()
            , "")
        ("HOME"
             , po::value<boost::filesystem::path>()
            , "")
        ;
        /*
         * From the set of the entire environment variable,
         * focus on the subset: only_env_opts and extract the result to vm_.
         */
        po::store(po::parse_environment(
            only_env_opts
            , [only_env_opts] (const std::string& name)->std::string
            {
                return
                    only_env_opts.find_nothrow(name, false)
                    ? name : ""
                ;
            }
        ), vm_);

        po::notify(vm_);
        if (vm_.count("SA_HOME"))
        {
            sPsHome_ = vm_["SA_HOME"].as<boost::filesystem::path>();
        }
        else
        {
            sPsHome_ = vm_["HOME"].as<boost::filesystem::path>();
            sPsHome_ /= "sa_home";
        }

        cmd_or_conf_opts_.add_options()
        ("filelog"
             , po::value<std::string>()
             , "")
        ("filetrace"
             , po::value<std::string>()
             , "")
        ("include"
             , po::value<std::string>()
             , "")
        ;
    }
    catch (const po::error& e)
    {
        RAISE_EX_CONVERT(std::runtime_error
            , boost::format("boost::program_options::error thrown. msg=%s:") % e.what()
        );
    }
}

cConfigures::~cConfigures()
{}

cConfigures::cOptionExtender::~cOptionExtender()
{}

void cConfigures::vSetAuxConf(cOptionExtender* extdr)
{
    BOOST_ASSERT(extdr);
    extdr_ = extdr;
}

std::ostream& operator<<(std::ostream& os, const cConfigures& rhs)
{
    // Incredible. Content registered one by one with add_options
    // is just dumped all at once with this.
    return os << rhs.allowed_opts_ << std::endl;
}

void cConfigures::vPrintAllKeyValuePairs(std::ostream& os) const
{
    BOOST_ASSERT(vm_.size());
    using ps::lib::sHumanReadable;
    ps::lib::cMap<std::string, std::string> ordered_by_key;
    for (const auto& item : vm_)
    {
        ordered_by_key.insert({item.first, sHumanReadable(item.second.value())});
    }
    os << "Startup parameters:" << std::endl;
    for (const auto& item : ordered_by_key)
    {
        os << "  " << item.first << std::string("=") << item.second << std::endl;
    }
    os << boost::format("Total %d parameters exist.") % ordered_by_key.size() << std::endl;
}

bool cConfigures::is_value(const std::string& key) const
{
    return vm_.count(key) > 0;
}

void cConfigures::vAttchToConf(
    const boost::filesystem::path& sConfPath
    , const int32_t iRecurse
){
    if (exists(sConfPath))
    {
        try
        {
            boost::iostreams::filtering_istream is;
            boost::regex re(cSubstituter::szPlaceHolder);
            is.push(boost::iostreams::regex_filter(re, cSubstituter()));
            is.push(boost::iostreams::file_source(sConfPath.string()));
            po::store(po::parse_config_file(is, cmd_or_conf_opts_), vm_);
            po::notify(vm_);
        }
        EXCPT_CONF_PARSE(po::invalid_option_value)
        EXCPT_CONF_PARSE(po::validation_error)
        EXCPT_CONF_PARSE(po::invalid_command_line_syntax)
        EXCPT_CONF_PARSE(po::invalid_config_file_syntax)
        EXCPT_CONF_PARSE(po::invalid_syntax)
        EXCPT_CONF_PARSE(po::ambiguous_option)
        EXCPT_CONF_PARSE(po::unknown_option)
        EXCPT_CONF_PARSE(po::error_with_no_option_name)
        EXCPT_CONF_PARSE(po::required_option)
        EXCPT_CONF_PARSE(po::multiple_occurrences)
        EXCPT_CONF_PARSE(po::multiple_values)
        EXCPT_CONF_PARSE(po::error_with_option_name)
        EXCPT_CONF_PARSE(po::reading_file)
        EXCPT_CONF_PARSE(po::invalid_command_line_style)
        EXCPT_CONF_PARSE(po::too_many_positional_options_error)
        if (vm_.count("include") > 0 && iRecurse)
        {
            // Search and load another conf file.
            vAttchToConf(vm_["include"].as<std::string>(), iRecurse - 1);
        }
    }
    else
    {
        cout_ << boost::format("%s Configure file %s does not exists.")
                % sClass(ps::lib::W)
                % sConfPath
            << std::endl;
    }
}

cConfigures& cConfigures::oParse(int32_t argc, char *argv[])
{
    sExecutablePath_ = boost::filesystem::complete(
        boost::filesystem::path(argv[0])
    );
    const auto regular_conf =
        sExecutablePath_.replace_extension("conf");
    boost::filesystem::path sDefaultConf;

    if (exists(regular_conf))
    {
        sDefaultConf = regular_conf;
    }
    else
    {
        sDefaultConf = sPsHome_;
        sDefaultConf /= regular_conf.filename();
    }

    try
    {
        only_cmd_opts_.add_options()
        ("help,h", po::value<bool>()->implicit_value(true), "showes help.")
        ("version,v", "prints version string.")
        ("config,c"
             , po::value<boost::filesystem::path>(&sConfFile_)
                ->default_value(sDefaultConf)
                    ->value_name("file_name")
             , "specify the configure file name.")
        ;

        allowed_opts_
            .add(only_cmd_opts_)
            .add(cmd_or_conf_opts_)
        ;

        try
        {
            po::store(
                po::command_line_parser(iArgc_ = argc, oArgv_ = argv)
                    .options(allowed_opts_)
                    .positional(pod_)
                    .run()
                , vm_
            );
        }
        EXCPT_ARGS_PARSE(po::invalid_option_value)
        EXCPT_ARGS_PARSE(po::validation_error)
        EXCPT_ARGS_PARSE(po::invalid_command_line_syntax)
        EXCPT_ARGS_PARSE(po::invalid_config_file_syntax)
        EXCPT_ARGS_PARSE(po::invalid_syntax)
        EXCPT_ARGS_PARSE(po::ambiguous_option)
        EXCPT_ARGS_PARSE(po::unknown_option)
        EXCPT_ARGS_PARSE(po::error_with_no_option_name)
        EXCPT_ARGS_PARSE(po::required_option)
        EXCPT_ARGS_PARSE(po::multiple_occurrences)
        EXCPT_ARGS_PARSE(po::multiple_values)
        EXCPT_ARGS_PARSE(po::error_with_option_name)
        EXCPT_ARGS_PARSE(po::reading_file)
        EXCPT_ARGS_PARSE(po::invalid_command_line_style)
        EXCPT_ARGS_PARSE(po::too_many_positional_options_error)

        po::notify(vm_);

        if (is_value("help") || is_value("version"))
        {
            return *this;
        }

        if (is_directory(sConfFile_))
        {
            sConfFile_ /= regular_conf.filename();
        }

        // Inporting parameters from the conf file.
        vAttchToConf(sConfFile_, 1);
    }
    EXCPT_OTHER_HNDL(po::error)

    const int32_t iErrors = iValidate(cout_);
    ASSERT_OR_RAISE(iErrors == 0, std::runtime_error
        , boost::format("%s %d error(s) detected in %s")
            % sClass(ps::lib::E) % iErrors % sConfFile_
    );

    return *this;
}

cConfigures& cConfigures::oAddConst(const std::string sKey, const std::string& sVal)
{
    oConstMap_.insert(std::make_pair(sKey, sVal));
    return *this;
}

const std::string& cConfigures::sGetConst(const std::string& sKey) const
{
    return oConstMap_.at(sKey);
}

int32_t iCconfiguresValidate(
    std::ostream& os
    , const boost::program_options::variables_map& vm_
    , const std::string& name
    , const int32_t& condition
    , const std::string& expression
){
    int32_t iErrors = 0;
    if (vm_.count(name) && !(condition))
    {
        os
            << boost::format(
                R"(Assertion failed expr=[%s], key=[%s], value=[%s].)"
            )
                % expression
                % name
                % ps::lib::sHumanReadable(vm_[name].value())
            << std::endl
        ;
        iErrors++;
    }
    return iErrors;
}

} // ps::lib

} // ps

