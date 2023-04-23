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

namespace lib
{
/**
 * @class cConfigures
 * - Analyze the startup argument or <PACKAGE_NAME>.conf and hold
 *   the operation parameters of the program.
 * - An operation parameter is represented by key=value pair.
 * - For the same key, if both the startup argument and <PACKAGE_NAME>.conf
 *   are set, the startup argument takes precedence.
 * - Since it is implemented as a singleton, content consistency is kept
 *   regardless of where it is referenced / updated in the program.
 *
 * - <PACKAGE_NAME>.conf must be in one of the following places:
 *   -# Arguments passed to startup options, '-c' or '--config'.
 *      If this argument is only a directory, treat it as <PACKAGE_NAME>.conf
 *      exists under it.
 *      Otherwise, if you give a path containing a file name to this argument,
 *      you can handle an arbitrary name file.
 *   -# The same directory as executable file.
 *   -# Product-specific folder (the location pointed to by the variable
 *      SA_HOME or sa_home under the user home directory).
 * - The smaller the number of cases, the higher the priority.
 * - Even if <PACKAGE_NAME> .conf does not exist an error does not occur.
 */

class cConfigures
    : public boost::serialization::singleton< cConfigures >
{
    friend class boost::serialization::singleton< cConfigures >;
public:
    typedef ps::lib::cMap<std::string, boost::any> tDerivedValues;
    class cOptionExtender
    {
    private:
    protected:
        virtual ~cOptionExtender() =0;
    public:
        virtual int32_t iValidate(
            std::ostream& os
            , const boost::program_options::variables_map& vm_
        ) const =0;
    };
private:
    /**
     * @struct cSubstituter
     */
    struct cSubstituter
    {
        /// It can be matched for "$HOME" or "${HOME}".
        static const char szPlaceHolder[];
        /// regular expression for environment variable name.
        static const char szVariableName[];
        std::string operator()(const boost::cmatch& match);
    };
    ps::lib::cConsole& cout_;
    uint32_t iNumPhisicalCores_;
    uint32_t iNumLogicalCores_;
    /**
     * @brief
     * stores a location of configuretion file.
     * Assume that the configuration file is in one of the following places in order.
     * -#  A location was directly passed by start up parameter --config or -c,
     * -#  or it is placed in the same directory as the executable file and has a common name,
     *     but the extension has been changed to "conf".
     * -#  or it is placed in the directory pointed to by cConfigures::sPsHome_, the name
     *     is common to the executable format, but the extension has been changed to "conf".
     */
    boost::filesystem::path sConfFile_;
    /**
     * @brief
     * stores a value of environmental variable SA_HOME or HOME+"/sa_home".
     * If SA_HOME is set, it takes precedence.
     */
    boost::filesystem::path sPsHome_;
    /**
     * @brief
     * specifies the number of columns that should be reserved for the description text;
     * if the option text encroaches into this, then the description will start on the next line.
     */
    const unsigned line_length_;
    /**
     * @brief
     * Declare a group of options that will be
     * allowed only on command line
     */
    boost::program_options::options_description only_cmd_opts_;
    /**
     * @brief
     * Declare a group of options that will be
     * allowed both on command line and in config file
     */
    boost::program_options::options_description cmd_or_conf_opts_;
    /// @brief Concatenation of @ref only_cmd_opts_ and @ref cmd_or_conf_opts_.
    boost::program_options::options_description allowed_opts_;
    /// @brief Configures for backward compatibility.
    boost::program_options::positional_options_description pod_;
    boost::program_options::variables_map vm_;
    cOptionExtender* extdr_;
    ps::lib::cMap<std::string, std::string> oConstMap_;
    int iArgc_;
    char **oArgv_;
    /**
     * @breif
     *   Check the absolute path of argv[0] and store it
     */
    boost::filesystem::path sExecutablePath_;
    cConfigures(); // will call by ctor of "singleton_wrapper<cConfigures>".
    ~cConfigures();
    void vAttchToConf(
        const boost::filesystem::path& path
        , const int32_t iRecurse
    );
public:
    void vSetAuxConf(cOptionExtender* extdr);
    /**
     * @brief
     *   puts the display element of how to use the program into the stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const cConfigures& rhs);
    /**
     * @brief
     *
     * Print to the ostream all contents of cConfigures::vm_
     * in the form of key=value
     */
    void vPrintAllKeyValuePairs(std::ostream& os) const;
    bool is_value(const std::string& key) const;
    const std::string::size_type length(const std::string& key) const
    {
        BOOST_ASSERT(is_value(key));
        return vm_[key].as<std::string>().size();
    }
    template<typename U>
    U as(const std::string& key) const
    try
    {
        return is_value(key) ? vm_[key].as<U>() : boost::value_initialized<U>();
    }
    catch (const boost::bad_any_cast& e)
    {
        RAISE_EX_CONVERT(
            std::runtime_error
            , boost::format(R"(%s Check the code around using startup parameter "%s": %s)")
            % sClass(ps::lib::E) % key % e.what()
        );
        return boost::value_initialized<U>();
    }
    /**
     * @brief
     *   Analyze the startup argument and <PACKAGE_NAME>.conf and hold
     *   the operation parameter in the cConfigures::vm_.
     *
     */
    cConfigures& oParse(int32_t argc, char *argv[]);
    cConfigures& oAddConst(const std::string sKey, const std::string& sVal);
    const std::string& sGetConst(const std::string& sKey) const;
    int32_t iValidate(std::ostream& os) const;
    boost::program_options::options_description& oGetOptionsDesc()
    {
        return cmd_or_conf_opts_;
    }
    boost::program_options::positional_options_description& oGetPositionalDesc()
    {
        return pod_;
    }
    const boost::filesystem::path& sGetPsHome() const
    {
        BOOST_ASSERT(!sPsHome_.empty());
        return sPsHome_;
    }
    const boost::filesystem::path& sGetConfFile() const
    {
        BOOST_ASSERT(!sConfFile_.empty());
        return sConfFile_;
    }
    uint32_t iGetNumPhisicalCores() const
    {
        BOOST_ASSERT(iNumPhisicalCores_ > 0);
        return iNumPhisicalCores_;
    }
    uint32_t iGetNumLogicalCores() const
    {
        BOOST_ASSERT(iNumLogicalCores_ > 0);
        return iNumLogicalCores_;
    }
    /**
     * @brief
     *   returns a string that equivalent argv[0].
     *
     * @note
     *   Before calling this, oParse() must be called.
     */
    boost::filesystem::path sGetExecutablePath() const
    {
        BOOST_ASSERT(sExecutablePath_.size());
        return sExecutablePath_;
    }
};

int32_t iCconfiguresValidate(
    std::ostream& os
    , const boost::program_options::variables_map& vm_
    , const std::string& name
    , const int32_t& condition
    , const std::string& expression
);

#define PS_UTL_CCONFIGURES_VALIDATE(stream, map, name, condition) \
    ps::lib::iCconfiguresValidate(stream, map, name, condition, #condition)

} // ps::lib

} // ps

