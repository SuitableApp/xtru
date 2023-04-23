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

#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

namespace ps
{

namespace lib
{

class cBackTraceImpl
{
private:
    cBackTraceImpl(const cBackTraceImpl&) =delete;
    cBackTraceImpl& operator=(const cBackTraceImpl&) =delete;
    int32_t line_;
    std::ostream& trc_;
    std::string cut_function_name_part(void* address) const
    {
        Dl_info info;
        char szHexStr[16+1];
        sprintf(szHexStr, "%p", address);
        std::string sAddr = szHexStr;
        sAddr += ": ";
        if (! ::dladdr(address, &info))
            return sAddr + "??? ";
        if (!info.dli_sname)
            // If constantly dli_sname is NULL, option "-rdynamic" might be insufficent to link.
            return sAddr + "??? in " + info.dli_fname; 
        if (!info.dli_saddr)
            return sAddr + info.dli_sname + " in " + info.dli_fname;
        // __cxa_demangle() is a library function for demangling.
        int32_t rc = 0; // Not zero means failure.
        char* demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, &rc);
        std::string sRet
            = sAddr + (rc ? info.dli_sname : demangled)
            + " + "  + boost::lexical_cast<std::string>((void *)((char *)address - (char *)info.dli_saddr))
            + " in "  + info.dli_fname;
        if (demangled) ::free(demangled);
        return sRet;
    }
public:
    cBackTraceImpl(std::ostringstream& oss, std::ostream& trc)
        : line_(0)
        , trc_(trc)
    {
        trc_ << oss.str() << std::endl << std::string("Backtrace:") << std::endl;
    }
    void operator()(void* address)
    {
        trc_ << boost::format("#%d %s") % line_
            % cut_function_name_part(address) << std::endl;
        line_++;
    }
};

cBackTrace::cBackTrace(std::ostringstream& oss)
{
    enum {MAX_TRACE = 64};
    auto& trc = ps::lib::cTracer::get_mutable_instance();
    // Preparing backtrace data.
    void* traces[MAX_TRACE];
    ::memset(&traces, 0, sizeof(traces));
    const int32_t tracesSize = ::backtrace(traces, MAX_TRACE);
    BOOST_ASSERT(tracesSize <= MAX_TRACE);
    // Printing symbols.
    cBackTraceImpl btrace(oss, trc);
    std::for_each(traces, traces + tracesSize, std::ref(btrace));
    {
        Dl_info info;
        ::dladdr(traces[0], &info);
        std::stringstream oss_;
        std::for_each(traces, traces + tracesSize, [&](void* address) {oss_ << address << " ";});
        trc << std::string("Following command could help you understanding faced problem:") << std::endl
            << boost::format("  addr2line -e %s -f -C %s") % info.dli_fname % oss_.str()<< std::endl;
    }
}

} // ps::lib

} // ps

