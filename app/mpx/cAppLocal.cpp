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
#include <mpx.h>

namespace ps
{

namespace app
{

namespace mpx
{

namespace bp = boost::process;
namespace bf = boost::filesystem;

/**
 * @class cAppLocalImpl
 * @brief Implementation of the class cAppLocal.
 */
class cAppLocalImpl
    : private cStartValues
{
private:
    static bool iIsDecimal(const char& c);
    static bool iIsAlnum(const char& c);
    static const bool iDoesExists(
        const ps::lib::cSet<std::string>& container
        , const std::string& item
    );
    static const bool iDoesExists(
        const ps::lib::cVector<std::string>& container
        , const std::string& item
    );
public:
    cAppLocalImpl();
    ~cAppLocalImpl();
    void vRun();
};

bool cAppLocalImpl::iIsDecimal(const char& c)
{
    return (c >= '0' and c <= '9');
}

bool cAppLocalImpl::iIsAlnum(const char& c)
{
    return iIsDecimal(c)
        or (c >= 'A' and c <= 'Z')
        or (c >= 'a' and c <= 'z')
        or c == '_'
        or c == '@'
    ;
}

const bool cAppLocalImpl::iDoesExists(
    const ps::lib::cSet<std::string>& container
    , const std::string& item
){
    return container.find( item ) != container.end();
}

const bool cAppLocalImpl::iDoesExists(
    const ps::lib::cVector<std::string>& container
    , const std::string& item
){
    return std::find_if (container.begin(), container.end()
        , [&](const std::string& it)
        {
            return it == item.substr(0, it.size());
        }
    ) != container.end();
}

cAppLocalImpl::cAppLocalImpl()
{}

cAppLocalImpl::~cAppLocalImpl()
{}

void cAppLocalImpl::vRun()
{
    boost::system::error_code bs_ec;
	cout_.setf(std::ios::fixed);
    // Change current directory.
    bf::current_path(sStartup_, bs_ec);
    ASSERT_OR_RAISE(!bs_ec, std::runtime_error
        , boost::format("%s Failed to change the startup directory %s: %s")
            % sClass(ps::lib::E)
            % sStartup_
            % bs_ec.message()
    );
    // Check file existing.
    bf::exists(sCmdFile_, bs_ec);
    ASSERT_OR_RAISE(!bs_ec, std::runtime_error
        , boost::format("%s %s doesn't exist in the %s or the name is invalid: %s")
            % sClass(ps::lib::E)
            % sCmdFile_
            % sStartup_
            % bs_ec.message()
    );
    // first: program, second: arguments
    typedef std::pair<std::string, std::string> stmt;
    std::vector<stmt> stmts;
    std::ifstream file(sCmdFile_.c_str());
    std::stringstream ss;
    ASSERT_OR_RAISE(! file.fail(), std::runtime_error
        , boost::format("%s Could not open file %s")
            % sClass(ps::lib::E)
            % (sStartup_ / sCmdFile_)
    );
    // Store in absolute path.
    sStartup_ = bf::current_path();

    std::string line;
    while (std::getline(file, line))
    {
        // Remove contiguous white space before and after
        ps::lib::vTrim(line);
        // Replace placeholder $n (n=1,2,3,... INT_MAX) included in line
        // with actual character string
        std::string::size_type first = 0, last;
        // Repeat to respond to that placeholder can be used more than once per line
        while ((first = line.find('$', first)) != std::string::npos)
        {
            auto it = line.begin() + first + 1;
            if (iIsDecimal(line[first+1]))
            {
                // whether placeholder of argument $n (n=1,2,3,... INT_MAX)
                auto ite = std::find_if_not(it, line.end(), &iIsDecimal);
                last = std::distance(line.begin(), ite);
            }
            else
            {
                // or environmental variables $<NamedValue> ?
                auto ite = std::find_if_not(it, line.end(), &iIsAlnum);
                last = std::distance(line.begin(), ite);
                // Case convert.
                while (it != ite)
                {
                    *it = std::toupper(*it);
                    ++it;
                }
            }
            // Replace the placeholder to the actual value if placeholder is found.
            const auto& map_ci =
                oTableOfReplacement_.find( line.substr(first, last - first) );
            if (map_ci != oTableOfReplacement_.end())
            {
                line.replace(first, last - first, map_ci->second);
            }
            first = last + (map_ci->second.size() - (last - first)) + 1;
            //             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            //             Adjust the difference caused by replacement
        }
        // Separate the program name and arguments from 'line'
        // and store them in std::pair
        std::string::size_type pos = line.find(' ');
        stmts.emplace_back(
            line.substr(0, pos)
            , (pos == std::string::npos) ? std::string("") : line.substr(pos + 1)
        );
    }
    file.close();
    // A set that identifies a string indicating a comment
    std::string sChildStdin;   // String sent to standard input of child process
    if (sCsi_.size())
    {
        if (sCsi_[0] == '@')
        {
            // Assume that mkcrd exists in the same directory as mpx and look for it.
            const bf::path sPrgName("mkcrd");  // <-- Credential utility.
            const bf::path sPrgPath = conf_.sGetExecutablePath().parent_path() / sPrgName;
            const auto is_mkcrd = bf::exists(sPrgPath, bs_ec);
            ASSERT_OR_RAISE(is_mkcrd, std::runtime_error
                , boost::format("%s Credential utility is not found: %s: %s.")
                % sClass(ps::lib::E)
                % sPrgPath
                % bs_ec.message()
            );
            const std::string sCommandLine
                = (boost::format("%s /f %s /inv") % sPrgPath % std::string(sCsi_, 1)).str();
            trc_ << boost::format("Credential utility will be launched.") << std::endl;

            bp::ipstream ipps;
            bp::child oChild(sCommandLine, bp::std_out > ipps);

            std::string buf;
            while (ipps && std::getline(ipps, buf))
            {
                // forward phrase of the text to search.
                if ("Connect string is:" == buf) break;
            }
            // It is require the next line as the target.
            std::getline(ipps, sChildStdin);
            std::error_code s_ec;
            oChild.wait(s_ec);
            ASSERT_OR_RAISE(0 == oChild.exit_code(), std::runtime_error
                , boost::format("%s failed to read a credential data: %s.")
                % sClass(ps::lib::E)
                % s_ec.message()
            );
            trc_ << boost::format("Comlated to read a credential data.") << std::endl;
        }
        else
        {
            sChildStdin = sCsi_;
        }
    }
    auto& oOut = cSink<cStdOut>::get_mutable_instance().open(sCso_);
    auto& oErr = cSink<cStdErr>::get_mutable_instance().open(sCse_);
    /// A queue for the tasks of extracting the datas from RDBMS.
    ps::lib::tSequence<ps::app::mpx::cExtProc> oQueue_;
    const auto iConcurrency_ = conf_.as<int32_t>("parallelism");
    const auto iLineUpSize = stmts.size();
    ps::lib::cVector<std::string> str_for_remarks {"#",";"};
    for  (size_t i = 0; i < iLineUpSize; ++i)
    {
        // Progress status.
        const auto sProgress = (
            boost::format("%4d/%4d") % (i+1) % iLineUpSize
        ).str();
        // Skipping empty line.
        if (stmts[i].first.empty()) continue;
        // Skipping comment.
        if (iDoesExists(str_for_remarks, stmts[i].first)) continue;
        // Identify the name of the program that does not abort
        // even if the return value is an error
        const bool bIgnorable = iDoesExists(oIgnList_, stmts[i].first);
        oQueue_.push_back(cExtProc::oMakeExtProc(
            stmts[i].first, stmts[i].second, sProgress, bIgnorable
            , iNice_, sChildStdin, oOut, oErr
        ));
        // Launching immediately the program to be multiplexed
        if (! iDoesExists(oMaxList_, stmts[i].first))
        {
            oQueue_.pop_back()->vRun();
        }
        // Synchronize if process names are not concatenated
        else if (i < iLineUpSize - 1 && stmts[i].first != stmts[i + 1].first)
        {
            ps::lib::vSynchronize(iConcurrency_, oQueue_, &ps::app::mpx::cExtProc::vRun);
        }
    }
    /*
     * - This function executes asynchronously the task stacked in oQueue_.
     * - This thread blocks until all other threads have joined this thread.
     * - Elements of oQueue_ are consumed and released.
     */
    ps::lib::vSynchronize(iConcurrency_, oQueue_, &ps::app::mpx::cExtProc::vRun);
}

cAppLocal::cAppLocal()
    : oImpl_(new cAppLocalImpl)
{}

void cAppLocal::vRun()
{
    oImpl_->vRun();
}

} // ps::app::mpx

} // ps::app

} // ps

