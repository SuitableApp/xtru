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

namespace ps
{
namespace lib
{
namespace nsStreamLocator
{
namespace bio = boost::iostreams;

constexpr static char FN_LOG_STDOUT[] = "_stdout.log";
constexpr static char FN_LOG_STDERR[] = "_stderr.log";

/// @class Pipe
/// An inner class that creates and manages pipes.
class Pipe
{
public:
    /// @brief Direction of the pipe.
    enum Direction { Read = 0, Write = 1 };
private:
    enum { NOT_ASSIGNED = -1 };
    /// @brief Descriptors managed by this instance.
    int fd_[2];
    /**
     * @brief
     *   For getting a opposite Direction value.
     */
    Direction iGetAnti(const Direction& dir) const
    {
        return (dir == Read) ? Write : Read;
    }
    /**
     * @brief
     *   closes the descriptor in the specified direction.
     * @param[in] dir
     *   @copybrief Direction
     */
    void vClose(Direction dir)
    {
        auto& ref_fd = fd_[dir];
        if (ref_fd != NOT_ASSIGNED)
        {
            ::close(ref_fd);
            ref_fd = NOT_ASSIGNED;
        }
    }
public:
    /**
     * @brief
     *   Constructor.
     * @exception
     *   Failed to open pipe.
     */
    Pipe()
        : fd_{NOT_ASSIGNED, NOT_ASSIGNED}
    {
        ASSERT_OR_RAISE(::pipe(fd_) == 0
            , std::runtime_error
            , boost::format("ERROR - pipe : errno=%d, msg=%s")
            % errno % ::strerror(errno));
    }
    /// @brief Destructor.
    ~Pipe()
    {
        release();
    }
    /**
     * @brief
     *   This function connects between the parent process side descriptor <br />
     *   that faces the child process side descriptor chosen by "dir" indexer <br />
     *   and the "newFd" descriptor.
     * @param[in] dir
     *   @copybrief Direction
     * @param[in] newFd
     *   New descriptor.
     * @retval NOT_ASSIGNED
     *   Failed to run.
     * @retval other
     *   New descriptor.
     * @exception
     *   Failed to assign.
     */
    int attach(Direction dir, int newFd)
    {
        const Direction iAnti = iGetAnti(dir);
        vClose(iAnti);
        ::close(newFd);
        if ((newFd = ::dup(fd_[dir])) == NOT_ASSIGNED)
        {
            ::perror("dup");
        }
        else
        {
            vClose(iAnti);
            vClose(dir);
        }
        return newFd;
    }
    /**
     * @brief
     *   This function returns the parent process side descriptor <br />
     *   chosen by the "dir" indexer, and closes the opposite side descriptor <br />
     *   that had been connected to it.
     *
     * @param[in] dir
     *   @copybrief Direction
     * @return
     *   Descriptor in the specified direction of this pipe.
     */
    int detach(Direction dir)
    {
        const Direction iAnti = iGetAnti(dir);
        vClose(iAnti);
        const int fd = fd_[dir];
        fd_[dir] = NOT_ASSIGNED;
        return fd;
    }
    /**
     * @brief
     *   releases ( closes ) all descriptors not attached and not detached.
     */
    void release()
    {
        for (auto& ref_fd : fd_)
        {
            if (ref_fd != NOT_ASSIGNED)
            {
                ::close(ref_fd);
                ref_fd = NOT_ASSIGNED;
            }
        }
    }
};

class cLocalProcessImpl
    : public bio::stream_buffer<bio::file_descriptor_sink>
{
private:
    enum
    { STDIN = STDIN_FILENO
    , STDOUT = STDOUT_FILENO
    , STDERR = STDERR_FILENO
    };
    /// Arbitrate the race condition occurring in pipe resource acquisition.
    typedef ps::lib::cSpinLock<int64_t, std::micro> spinlock_t;
    static spinlock_t oPipeSafe_;
    /// to diagnose
    ps::lib::cTracer& trc_;
    /// Command string to be executed.
    std::string sCommand_;
    /// Process ID of the external process.
    pid_t pid_;
    /// A stream for recieving messages from standard output.
    bio::stream<bio::file_descriptor_source> out_;
    /// A stream for recieving messages from standard error output.
    bio::stream<bio::file_descriptor_source> err_;
    /// Redirector for standard output.
    cAsyncRedirector srOut_;
    /// Redirector for standard error output.
    cAsyncRedirector srErr_;
    void vPrintTaggedLocator(
        std::ostream& os
        , const std::string& sTagId
    ){
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        os << boost::format("%s [%s:%s:%s]") % sClass(ps::lib::I) % sTagId
            % boost::posix_time::to_simple_string(now) % sCommand_ << std::endl;
    }
    template<int FILENO>
    void vOutputContents(
        const char fname[]
        , const std::string& sMsg
    ){
        static std::mutex oMtx_;
        std::lock_guard<std::mutex> lk(oMtx_);
        static bool iAtOnce(true);
        std::ios_base::openmode mode
            = iAtOnce
            ? std::ios_base::out | std::ios_base::trunc
            : std::ios_base::app
            ;
        boost::filesystem::ofstream ofs(sOutput_ / fname, mode);
        vPrintTaggedLocator(ofs, "B");
        ofs << sMsg;
        vPrintTaggedLocator(ofs, "E");
        if (iAtOnce) iAtOnce = false;
    }
    void vWaitForChild();
public:
    cLocalProcessImpl(const std::string& sCommand);
    /**
     * @brief
     * Destruct this instance.
     *
     * Perform the following procedure in order.
     *   -# Complete the transmission of the content outputted
     *      for this instance to the external process.
     *   -# Wait for the termination of the external process.
     *   -# Output the contents of output from the process
     *      to the stream at once.
     */
    ~cLocalProcessImpl();
};

cLocalProcessImpl::spinlock_t cLocalProcessImpl::oPipeSafe_(std::chrono::microseconds(300));

cLocalProcessImpl::cLocalProcessImpl(
    const std::string& sCommand
)
    : trc_(ps::lib::cTracer::get_mutable_instance())
    , sCommand_(sCommand)
    , pid_(0)
{
    {
        /// Arbitrate the race condition occurring in pipe resource acquisition.
        std::lock_guard<spinlock_t> lock(oPipeSafe_);
        /// See http://keicode.com/note/lin07.php .
        /// This page is conveniently described about pipe working,
        /// so it helps to understand.
        Pipe pipes[3];
        ::pid_t pid = ::fork();
        if (pid < 0)
        {
            RAISE_EX_CONVERT(std::runtime_error
                , boost::format("ERROR - fork : errno=%d, msg=%s")
                % errno % ::strerror(errno));
        }
        else if (pid == 0)
        {
            if (
                   pipes[STDIN].attach(Pipe::Read, STDIN) != -1
                && pipes[STDOUT].attach(Pipe::Write, STDOUT) != -1
                && pipes[STDERR].attach(Pipe::Write, STDERR) != -1
            ){
                for (auto& p : pipes) p.release();
                if (::execl("/bin/sh", "sh", "-c", sCommand.c_str(), nullptr) == -1)
                {
                    ::perror("execl");
                    ::exit(EXIT_FAILURE);
                }
            }
        }
        pid_ = pid;
        trc_ << boost::format("PID=%d Launched \"%s\"") % pid_ % sCommand_ << std::endl;
        this->open(pipes[STDIN].detach(Pipe::Write), bio::close_handle);
        out_.open(pipes[STDOUT].detach(Pipe::Read), bio::close_handle);
        err_.open(pipes[STDERR].detach(Pipe::Read), bio::close_handle);
    }
    // Started to read asynchronously.
    srOut_.vReadAsync(out_->handle());
    srErr_.vReadAsync(err_->handle());
}

void cLocalProcessImpl::vWaitForChild()
{
    std::ostringstream oss;
    oss << boost::format("PID=%d ") % pid_;
    int status = 0;
    oPipeSafe_.unlock();                       /// UN-LOCK
    const pid_t rtn = ::waitpid(pid_, &status, 0);
    oPipeSafe_.lock();                         /// LOCK
    if (rtn == -1)
    {
        oss << boost::format("ERROR - waitpid : errno=%d, msg=%s ")
            % errno % ::strerror(errno);
    }
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status))
        {
            oss << boost::format("WARNING - Might have failed rc=%d ") % WEXITSTATUS(status);;
        }
        else
        {
            oss << boost::format("Exited with rc=%d ") % WEXITSTATUS(status);
        }
    }
    else if (WIFSIGNALED(status))
    {
        oss << boost::format("Terminated by signal %d ") % WTERMSIG(status);
    }
    else if (WIFSTOPPED(status))
    {
        oss << boost::format("Stopped by signal %d ") % WSTOPSIG(status);
    }
    else
    {
        oss << boost::format("Changed status by unknown reason ");
    }
    oss << boost::format("\"%s\"") % sCommand_;
    trc_ << oss.str() << std::endl;
}

cLocalProcessImpl::~cLocalProcessImpl()
try
{
    if (pid_ != 0)
    {
        std::lock_guard<spinlock_t> lock(oPipeSafe_);
        // Must this->close() be before waiting, otherwise external process
        // cannot be completed by recieving EOF and vWait waits it forever.
        if (this->is_open()) this->close();
        vWaitForChild();
        srOut_.vWait();
        srErr_.vWait();
        vOutputContents<STDOUT_FILENO>(FN_LOG_STDOUT, srOut_.sGetMsg());
        vOutputContents<STDERR_FILENO>(FN_LOG_STDERR, srErr_.sGetMsg());
        if (err_.is_open()) err_.close();
        if (out_.is_open()) out_.close();
        pid_ = 0;
    }
}
catch (...)
{
    // do nothing
}

/**
 * @brief
 *   works to mediate between the interface and the implementation.
 * @param[in] sCommand
 */
cLocalProcess::cLocalProcess(const std::string& sCommand)
    : oImpl_(new cLocalProcessImpl(sCommand)
    , vRegularDeleter<cLocalProcessImpl>)
{
    this->rdbuf(oImpl_.get());   // invokes std::ostream::rdbuf().
}

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
