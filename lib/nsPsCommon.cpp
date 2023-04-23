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

/**
 * @details
 *   user defined message handler
 * @param[in] expr
 * @param[in] function
 * @param[in] file
 * @param[in] line
 *    Should not declare type as int32_t, becouse demanded by the Boost.
 */
void boost::assertion_failed(
    char const* expr
    , char const* function
    , char const* file
    , long line
){
     std::cout << boost::format("ASSERTION VIOLATED: '%s' in '%s' (%s-%d)")
         % expr % function % file % line
     << std::endl;
     std::abort();
}

/**
 * @details
 *   user defined message handler
 * @param[in] expr
 * @param[in] function
 * @param[in] file
 * @param[in] line
 *   Should not declare type as int32_t, becouse demanded by the Boost.
 */
void boost::assertion_failed_msg(
    char const * expr
    , char const * msg
    , char const * function
    , char const * file
    , long line
){
     std::cout << boost::format("ASSERTION VIOLATED: %s: '%s' in '%s' (%s-%d)")
         % msg % expr % function % file % line
     << std::endl;
     std::abort();
}

namespace ps
{

namespace lib
{

namespace bf = boost::filesystem;

/**
 * @details
 */
std::string sClass(const tMsgKey& iMsgKey)
{
    static const std::map<tMsgKey, const std::string> map =
        { {tMsgKey::I, "Information -"}
        , {tMsgKey::W, "Warning -"}
        , {tMsgKey::E, "Error -"}
        };
    return map.at(iMsgKey);
}

/**
 * @details
 */
std::ostream& oNotice(std::ostream& out)
{
    auto& conf = ps::lib::cConfigures::get_const_instance();
    return out
        << conf.sGetConst("title") << " version " << conf.sGetConst("version") << std::endl
        << conf.sGetConst("copyright") << std::endl
        << "This is free software; see the source for copying conditions.  There is NO" << std::endl
        << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl
    ;
}

/**
 * @details
 */
void vSleepSecs(const int32_t& iSecs)
{
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC_);
    xt.sec += iSecs;
    boost::thread::sleep(xt); // wait for until iSecs seconds later.
}
/**
 * @details
 */
int32_t vConfirmation(std::ostream& os)
{
    char ans;
    for (;;)
    {
        std::cin >> ans;
        if (ans == 'Y' || ans == 'y' || ans == 'N' || ans == 'n') break;
        os << boost::format("Unexpected '%c' was denied. Hit 'Y' or 'n'.") % ans << std::endl;
    }
    return toupper(ans);
}

/**
 * @details
 */
bool iCheckHomeOrMakeIt(const boost::filesystem::path& sPath)
{
    auto& cout = ps::lib::cConsole::get_mutable_instance();

    if ( ! bf::exists(sPath))
    {
        cout << boost::format(
            "******** WARNING ********\n"
            "A home directory does not exists for this application. \n"
            "It must be at %s.\n"
            "May you accept to create a new directory? [y/n]"
        ) % sPath << std::endl;
        if (ps::lib::vConfirmation(cout) == 'N') { return true; } // Cancel.

        bf::create_directory(sPath);
        cout << boost::format("A directory for this application was created.") << std::endl;
    }
    return false;
}
/**
 * @details
 */
bool iCheckConfOrMakeIt(
    const boost::filesystem::path& sPath
    , const boost::filesystem::path& sPreFixDir
    , const boost::filesystem::path& sPkgDataDir
    , const std::string& sPkgName
){
    auto& mos_ = ps::lib::cDistributor::get_mutable_instance();
    auto& trc = ps::lib::cTracer::get_mutable_instance();
    const bf::path sTarName(sPreFixDir / sPkgDataDir / "conf.tar");
    if ( ! bf::exists(sTarName))
    {
        mos_ <<  boost::format(
            "%s should have the default settings could not be found. "
            "Maybe %s is not installed.") % sTarName % sPkgName << std::endl;
        return true;
    }
    const bf::path config_file_((sPath / sPkgName) += ".conf");
    const bool conf_not_exists = ! bf::exists(config_file_);
    if (conf_not_exists)
    {
        std::ostringstream oss;
        std::string sProcMsg;
        const bf::path sCurrentDir = bf::current_path();
        bf::current_path(sPath);
        trc << boost::format("Current directory is switched to %s") % sPath << std::endl;
        oss << boost::format("tar xvf %s") % sTarName.string();
        int32_t rc = ps::lib::system::iStartSubProc(oss.str(), sProcMsg);
        mos_ << boost::format("%s\n%s%s")
            % sProcMsg % (0 != rc ? "Failed: " : "Completed: ") % oss.str()
            << std::endl << std::endl;
        if (0 == rc)
        {
            mos_ << boost::format(
                "WHAT YOU NEED TO DO NEXT BEFORE RUNNING:\n"
                "Please configure %s according to your usage.\n\n"
            ) % config_file_;
        }
        bf::current_path(sCurrentDir);
        trc << boost::format("Current directory is switched to %s") % sCurrentDir << std::endl;
    }
    return conf_not_exists;
}

/**
 * @details
 */
static bool wild_strmatch(
    const char *ptn
    , const char *str
){
    switch (*ptn)
    {
    case '\0':
        return '\0' == *str;
    case '*':
        return wild_strmatch( ptn+1, str )
            || (('\0' != *str) && wild_strmatch( ptn, str+1 ));
    case '?':
        return ('\0' != *str)
            && wild_strmatch( ptn+1, str+1 );
    default:
        return ((uint8_t)*ptn == (uint8_t)*str)
            && wild_strmatch( ptn+1, str+1 );
    }
}

/**
 * @details
 */
bool wild_strmatch(
    const std::basic_string<char>& ptn
    , const std::basic_string<char>& str
){
    return wild_strmatch(ptn.c_str(), str.c_str());
};

/**
 * @details
 */
bool composite_less(const char* lhs, const char* rhs)
{
    const auto i = ::strcmp(lhs, rhs);
    if (i) return i < 0;
    return false;
}

/**
 * @details
 */
bool composite_equal(const char* lhs, const char* rhs)
{
    return ::strcmp(lhs, rhs) == 0;
}

/**
 * @details
 */
bool iMatchedBackward(const char* a, const char* b)
{
    const size_t ia = strlen(a);
    const size_t ib = strlen(b);
    if (ia > ib)
    {
        return strcmp(a + (ia - ib), b) == 0;
    }
    else if (ia < ib)
    {
        return strcmp(b + (ib - ia), a) == 0;
    }
    else
    {
        return strcmp(a, b) == 0;
    }
}

/**
 * @details
 */
std::string sGetSqlInList(const str_vct& vct)
{
    str_vct oCopy(vct);
    std::ostringstream oss;
    oss << oCopy.oChange(cEnclosure::oSqlInList);
    return oss.str();
}

/**
 * @details
 */
tPtrFstream open_with_check(
    const boost::filesystem::path& name
    , const boost::filesystem::fstream::ios_base::openmode mode
){
    tPtrFstream ptr(new boost::filesystem::fstream);
    try
    {
        ptr->exceptions(boost::filesystem::fstream::failbit);
        ptr->open(name, mode);
    }
    catch (const boost::filesystem::fstream::failure&)
    {
        // Intentionally use ::strerror(errno), instead of boost::filesystem::fstream::failure::what().
        // Because what() is incompetent, it can only say " basic_ios::clear".
        // In contrast, ::strerror() speaks eloquently the situation of the problem.
        // For example, it speaks concretely like "No such file or directory".
        RAISE_EX_CONVERT(std::runtime_error
            , boost::format("%s Failed open a file %s: %s")
                % sClass(ps::lib::E) % name % ::strerror(errno)
        );
    }
    return ptr; // Hand over the ownership to the caller.
}

/**
 * Binary
 */
bool iCompBinary(
    int (*iBinary)(const char*, const char*)
    , const char* lhs, const char* rhs
){
     return (*iBinary)(lhs, rhs) == 0;
}

/**
 * Ternary
 */
bool iCompTernary(
    int (*iTernary)(const char*, const char*, size_t)
    , const char* lhs, const char* rhs, size_t len
){
     return (*iTernary)(lhs, rhs, len) == 0;
}

std::string sHumanReadable(const boost::any& val)
{
    std::string sRes;
    using boost::any_cast;
    using boost::filesystem::path;
    using boost::lexical_cast;
    const std::type_info& t = val.type();
    if      (t == typeid(std::string)) { sRes = "\"" + any_cast<std::string>(val) + "\"" ; }
    else if (t == typeid(path))        { sRes = any_cast<path>(val).string(); }
    else if (t == typeid(bool))        { sRes = lexical_cast<std::string>(any_cast<bool>(val)); }
    else if (t == typeid(int))         { sRes = lexical_cast<std::string>(any_cast<int>(val)); }
    else if (t == typeid(char))        { sRes = lexical_cast<std::string>(any_cast<char>(val)); }
    else if (t == typeid(int16_t))     { sRes = lexical_cast<std::string>(any_cast<int16_t>(val)); }
    else if (t == typeid(uint16_t))    { sRes = lexical_cast<std::string>(any_cast<uint16_t>(val)); }
    else if (t == typeid(int32_t))     { sRes = lexical_cast<std::string>(any_cast<int32_t>(val)); }
    else if (t == typeid(uint32_t))    { sRes = lexical_cast<std::string>(any_cast<uint32_t>(val)); }
    else if (t == typeid(int64_t))     { sRes = lexical_cast<std::string>(any_cast<int64_t>(val)); }
    else if (t == typeid(uint64_t))    { sRes = lexical_cast<std::string>(any_cast<uint64_t>(val)); }
    else if (t == typeid(float))       { sRes = lexical_cast<std::string>(any_cast<float>(val)); }
    else if (t == typeid(double))      { sRes = lexical_cast<std::string>(any_cast<double>(val)); }
    else                               { sRes = "<UNPRINTERBLE>"; }
    return sRes;
}

/**
 * @details
 */
const char * szBuildMessage(
    const ps::lib::tMsgKey iMsgKey
    , const boost::filesystem::path& fn
    , const std::string& msg
){
    static std::string sMsgBuf_;
    /*
     * In order to guarantee the lifetime of the object referenced by the pointer,
     * it is necessary to assign it to a variable of the same lifetime as the object.
     */
    sMsgBuf_ = (boost::format("%s %s: %s") % ps::lib::sClass(iMsgKey) % msg % fn).str();
    return sMsgBuf_.c_str();
}

} // ps::lib

} // ps

