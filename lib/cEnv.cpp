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

cEnv::cEnv()
    : envp_(nullptr)
{}

cEnv& cEnv::operator()(const char** envp)
{
    static bool once = true;
    if (once)
    {
        ps::lib::tSep rule_("", "=", "\"");
        envp_ = envp;
        const char *ptr;
        while ((ptr = *envp++) != nullptr)
        {
            const char *eok = ::strchr(ptr, '='); // End of key.
            const size_t len = ::strlen(ptr);
            ASSERT_OR_RAISE(eok, std::runtime_error
                , boost::format("%s '=' is not found.") % sClass(ps::lib::E));
            this->emplace(
                std::string(ptr, eok)
                , std::string(eok + 1, ptr + len)
            );
        }
        once = false;
    }
    return *this;
}

void cEnv::vPrintAllEnvVariables(std::ostream& os)
{
    os << std::string("All environmental variables:") << std::endl;
    for (const auto& val : *this)
    {
        os << boost::format("  %s=%s")
                % val.first % val.second
            << std::endl;
    }
    os << boost::format("Total %lu environmental variables exist.")
            % size()
        << std::endl;  
}

} // ps::lib

} // ps

