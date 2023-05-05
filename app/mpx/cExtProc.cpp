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

#include <boost/process/extend.hpp>
namespace ps
{

namespace app
{

namespace mpx
{

/**
 * @class cExtProcImpl
 * @brief Implementation of the class cExtProc.
 *
 * @copydoc cExtProc
 *
 */
namespace bp = boost::process;
namespace bf = boost::filesystem;
namespace bt = boost::posix_time;

class cExtProcImpl
	: public cExtProc
{
public:
    /**
     * @brief
     *   An instance of cExtProcImpl type is created.
     *
     * @param[in] sExecutable
     * - A program name that is invoked.
     * - Not only the executable form but also scripts of various
     *   interpreters (i.e. such as sh, python, perl,...) to which
     *   execution authority is given.
     * @param[in] sParameters
     * - It is passed when sExecutable is accompanied by an argument.
     * @param[in] sProgress
     * - Display progress as a fraction.
     * @param[in] bIgnorable
     * - true: ignored even if non-ZERO is returned from external program.
     * - Generally non-ZERO indicates abnormal termination.
     * @param[in] iNice
     * - Pass the priority when executing the external program
     *   by a numerical value.
     * - -20 is the highest priority, +19 is the lowest priority.
     *   Only the root user can be specified below -1.
     * @param[in] sChildStdin
     * - A string sent to STDIN (standard input) of external program.
     * @param[in,out] oOut
     * - An output stream for redirecting messages received from
     *   STDOUT (standard output) of an external program.
     * @param[in,out] oErr
     * - An output stream for redirecting messages received from
     *   STDERR (standard outerr) of an external program.
     */
	cExtProcImpl(
		const std::string& sExecutable
		, const std::string& sParameters
		, const std::string& sProgress
		, const bool bIgnorable
        , const int32_t iNice
		, const std::string& sChildStdin
        , std::ostream& oOut
        , std::ostream& oErr
	);
private:
    ps::lib::cRtn& rtn_;
    ps::lib::cConsole& cout_;
    ps::lib::cTracer& trc_;
    ps::lib::cDistributor& mos_;
	const std::string sExecutable_;   ///< A name of executable.
	const std::string sParameters_;   ///< Startup arguments.
	const std::string sProgress_;     ///< indicates progress.
	const bool bIgnorable_;           ///< true: Ignore process return value.
    const int32_t iNice_;             ///< Execution priority. (between -20 and +19)
	const std::string& sChildStdin_;  ///< A string for STDIN of an external program.
    std::ostream& oOut_;              ///< A output stream for STDOUT of an external program.
    std::ostream& oErr_;              ///< A output stream for STDERR of an external program.
    /**
     * @brief
     *   Assemble notification message of begining.
     */
    std::string sShowStarting() const;
    /**
     * @brief
     *   Returns elapsed time in second.
     */
    double fGetElapsed(const bt::ptime& time_at_started) const;
    /**
     * @brief
     *   Returns the interval elapsed since time_at_started in seconds.
     */
    std::string sShowElapsed(const bt::ptime& time_at_started) const;
    /**
     * @brief
     *   Assemble notification message of execution result.
     */
    std::string sShowFinishing(
        const int32_t& iExitCode
        , const bt::ptime& time_at_started
    ) const;
    /**
     * @brief
     *   Control the process from the start to the end of the external program.
     *
     * @note
     *   Thread safe is guaranteed for any call.
     */
	virtual void vRun();
	virtual ~cExtProcImpl();
	cExtProcImpl(const cExtProcImpl&);
	cExtProcImpl& operator=(const cExtProcImpl&);
};

std::string cExtProcImpl::sShowStarting() const
{
    return (boost::format("[%s] %s %s")
        % sProgress_ % sExecutable_ % sParameters_
    ).str();
}

double cExtProcImpl::fGetElapsed(const bt::ptime& time_at_started) const
{
    const bt::time_period period(
        time_at_started
        , bt::microsec_clock::local_time()
    );
    const bt::time_duration duration = period.length();
    return duration.total_milliseconds() / 1000.0;
}

std::string cExtProcImpl::sShowElapsed(const bt::ptime& time_at_started) const
{
    return (boost::format("[%s] runing.   %8.2fs elapsed.")
        % sProgress_ % fGetElapsed(time_at_started)
    ).str();
}

std::string cExtProcImpl::sShowFinishing(
    const int& iExitCode
    , const bt::ptime& time_at_started
) const
{
    std::string sResult;
    if (0 == iExitCode)
    {
        sResult = (boost::format("completed.%8.2fs elapsed.")
            % fGetElapsed(time_at_started)).str();
    }
    else if (! rtn_.iCotinue())
    {
        sResult = "canceled.";
    }
    else
    {
        sResult = (boost::format("failed with rc=%d") % iExitCode).str();
        if (bIgnorable_)
        {
            sResult += ", but ignored.";
        }
        else
        {
            sResult += ".";
        }
    }
    return (boost::format("[%s] %s") % sProgress_ % sResult).str();
}

void cExtProcImpl::vRun()
{
    if (! rtn_.iCotinue()) return;
    // Keep timestamp when started
    const bt::ptime time_at_started(
        bt::microsec_clock::local_time()
    );
    // To protect critical sections from multiple access.
    static std::mutex mtx_;
    const auto sPrioSpec =
        (iNice_) ? (boost::format("nice -n %d ") % iNice_).str() : "";
    // Prepare candidates for places to find sExecutable_
    std::vector<bf::path> oPaths(
        // note: boost::this_process::path() could not be used
        // because of a segment fault in Solaris
        ps::lib::system::oGetPathsArray()
    );
    // To make the current directory the top priority, place it to the top.
    oPaths.emplace(oPaths.begin(), bf::current_path());
    bf::path sEffective
        = bf::path(sExecutable_).parent_path().empty()
        ? bp::search_path(sExecutable_,  oPaths)
        : sExecutable_ // When absolute or relative position is specified
        ;
    const std::string sCommandLine
        = (boost::format("%s%s %s")
            % sPrioSpec
            % sEffective.string()
            % sParameters_
        ).str();

    mos_ << sShowStarting() << std::endl;

    bp::ipstream ips_stdout, ips_stderr;

    // io_service is deprecated.
    // io_context should be used instead of io_service.
    boost::asio::io_context oIoCtx;
    bp::child oChild;
    std::error_code s_ec;
    // To show progress periodically.
    ps::lib::cIntervalTimer oIntervalTimer(
        oIoCtx
        , [&] { cout_ << sShowElapsed(time_at_started) << std::endl; }
    );
    auto sTransfer = [](std::istream& in)
    {
        std::string line;
        std::ostringstream oss;
        while (std::getline(in, line))
        {
            oss << line << std::endl;
        }
        return oss.str();
    };
    auto fut_stdout = std::async(sTransfer, std::ref(ips_stdout))
       , fut_stderr = std::async(sTransfer, std::ref(ips_stderr))
    ;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        /*
         * When ops_stdin is destructed, EOF is implicitly sent to std_in.
         * This must be done before calling oIoCtx.run(). To prevent deadlock.
         */
        bp::opstream ops_stdin;
        ops_stdin << sChildStdin_ << std::endl;
        /*
         * Boost.Process:
         *   To implement a portable wrapper.
         * https://www.boost.org/doc/libs/1_65_1/doc/html/process.html
         */
        oChild = bp::child(
            std::vector<std::string>{"/bin/sh", "-c", sCommandLine}
            , bp::std_in < ops_stdin
            , bp::std_out > ips_stdout
            , bp::std_err > ips_stderr
            , s_ec
            , oIoCtx
            // This function is called back within the context of
            // boost::asio::io_context::run().
            , bp::on_exit([&](int iExitCode, const std::error_code& s_ec_)
            {
                std::lock_guard<std::mutex> lk(mtx_);
                oIntervalTimer.vSuspend();
                const auto sFinish = sShowFinishing(iExitCode, time_at_started);
                oOut_
                    << sShowStarting() << std::endl
                    << fut_stdout.get()   // Net messages received.
                    << sFinish << std::endl
                ;
                oErr_
                    << sShowStarting() << std::endl
                    << fut_stderr.get()   // Net messages received.
                    << sFinish << std::endl
                ;
                mos_ << sFinish << std::endl;
                rtn_.vDelPid(oChild.id());
                if (rtn_.iCotinue() && ! bIgnorable_ && 0 != iExitCode)
                {
                    // notificates SIGTERM to brother process indirectly.
                    rtn_.vBreak();
                }
            })
        );
        rtn_.vAddPid(oChild.id());
    }
    /*
     * run() must be placed outside the lock guarded block.
     * To ensure asynchronicity.
     */
    oIoCtx.run();
}

cExtProcImpl::cExtProcImpl(
    const std::string& sExecutable
    , const std::string& sParameters
    , const std::string& sProgress
    , const bool bIgnorable
    , const int32_t iNice
    , const std::string& sChildStdin
    , std::ostream& oOut
    , std::ostream& oErr
)
    : rtn_(ps::lib::cRtn::get_mutable_instance())
    , cout_(ps::lib::cConsole::get_mutable_instance())
    , trc_(ps::lib::cTracer::get_mutable_instance())
    , mos_(ps::lib::cDistributor::get_mutable_instance())
	, sExecutable_(sExecutable)
	, sParameters_(sParameters)
	, sProgress_(sProgress)
	, bIgnorable_(bIgnorable)
    , iNice_(iNice)
	, sChildStdin_(sChildStdin)
    , oOut_(oOut)
    , oErr_(oErr)
{}

cExtProcImpl::~cExtProcImpl()
{}

cExtProc * cExtProc::oMakeExtProc(
    const std::string& sExecutable
    , const std::string& sParameters
    , const std::string& sProgress
    , const bool bIgnorable
    , const int32_t iNice
    , const std::string& sChildStdin
    , std::ostream& oOut
    , std::ostream& oErr
){
	return new cExtProcImpl(
		sExecutable, sParameters, sProgress, bIgnorable
        , iNice, sChildStdin, oOut, oErr
	);
}

cExtProc::~cExtProc()
{}

} // ps::app::mpx

} // ps::app

} // ps

