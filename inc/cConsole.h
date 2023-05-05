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

#pragma once

namespace ps
{
 
namespace lib
{
 
/**
 * @class cConsole
 * @brief
 * This class handles streaming the application messages to console.
 *
 */
class cConsole
    : public cOstream 
    , public boost::serialization::singleton< cConsole >
{
    friend class boost::serialization::singleton< cConsole >;
private:
    /// to restore original std::streambuf changed by @ref oRedirectTo() calling.
    std::streambuf *bkp_;
    /**
     * @brief
     *  If the message is instructed to be saved (@ref oRedirectTo())
     *  in the file pointed to by @ref ofs_ its valid path is indicated.
     */
    boost::filesystem::ofstream ofs_;
    cConsole();  // will call by ctor of "singleton_wrapper<cConsole >".
    ~cConsole();
public:
    /**
     * @brief
     *   generates a date-time tag and outputs to the std::ostream (os).
     * @param[in] os
     */
    virtual void vAddTag(std::ostream& os);
    /**
     * @brief
     *   changes the direction of the streaming
     *   from the console to a user-specified file.
     * @param[in] fn
     *   indicates the direction of the streaming.
     * @note
     * -  If the first character of fn is an
     *    addition symbol (='+'), it means additionally written.
     * -  If there is no addition symbol and there is already
     *    a file on fn, the contents are truncated first.
     */
    cConsole& oRedirectTo(const boost::filesystem::path& fn);
};

} // ps::lib

} // ps

