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

enum tNumCnnst {iZeroItem = 0, iOneItem = 1};

enum tMsgKey
    { I  ///< Information
    , W  ///< Warning
    , E  ///< Error
    };

extern std::string sClass(const tMsgKey& iMsgKey);

typedef boost::escaped_list_separator<char> tSep;

template<class T>
struct cRegularDeleter
{
    void operator()(T* __ptr)
    {
        delete __ptr;
    }
};

template<class T>
void vRegularDeleter(T* __ptr)
{
    delete __ptr;
}

/**
 * @brief
 */
struct cFstreamDeleter
{
    void operator()(boost::filesystem::fstream* ptr) const
    {
        BOOST_ASSERT(ptr);
        ptr->flush();
        ptr->close();
        delete ptr;
    }
};
typedef std::unique_ptr<boost::filesystem::fstream, cFstreamDeleter> tPtrFstream;

} // ps::lib

} // ps

