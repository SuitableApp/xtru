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

namespace system
{

/**
 * @details
 */
int32_t iStartSubProc(const std::string cmd, std::string& output)
{
    // Starting sub process.
    FILE* fp = ::popen(cmd.c_str(), "r");
    ASSERT_OR_RAISE(0 != fp, std::runtime_error, boost::format(R"(::popen("%s","r"))") % cmd);
    // Taking stdout from sub process.
    boost::iostreams::stream_buffer<
        boost::iostreams::file_descriptor_source
    > sb(::fileno(fp), boost::iostreams::never_close_handle);

   std::ostringstream oss;
   oss << &sb;
   oss.str().swap(output);

   return ::pclose(fp);
}

/**
 * @details
 * @return current  process name.
 */
#if defined(__sun)
#include <unistd.h>
#include <procfs.h>
#include <fcntl.h>
#include <string.h>
#endif
std::string sGetCurrentProcessName(void)
{
#if defined(__APPLE__) || defined(__FreeBSD__)
    return getprogname();
#elif defined(_GNU_SOURCE)
    return program_invocation_short_name;
#elif defined(__sun)
    static char szFname[sizeof(psinfo_t::pr_fname)];
    char psfile[64];
    const pid_t pid = getpid();
    psinfo_t psinfo;
    sprintf(psfile, "/proc/%d/psinfo", pid);
    const int fd = open(psfile, O_RDONLY);
    if (fd >= 0)
    {
        if (read(fd, &psinfo, sizeof(psinfo_t)) != -1)
        {
            strcpy(szFname, psinfo.pr_fname);
        }
        else
        {
            szFname[0] = 0;
        }
    }
    else
    {
        szFname[0] = 0;
    }
    return szFname;
#else
    return "?";
#endif
}

/**
 * @detail
 */
std::vector<boost::filesystem::path> oGetPathsArray(void)
{
    const std::string sPaths(std::getenv("PATH"));
    std::vector<boost::filesystem::path> arr;
    size_t next = 0, last = next;
    while ((next = sPaths.find(':', last)) != std::string::npos)
    {
        arr.push_back(sPaths.substr(last, next-last));
        last = next + 1;
    }
    if (last > 0)
    {
        arr.push_back(sPaths.substr(last));
    }
    return arr;
}

} // ps::lib::system

} // ps::lib

} // ps

