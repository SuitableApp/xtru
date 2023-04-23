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

namespace sql
{

namespace formatter
{

/**
 * @class cDirector
 *   Perform indentation and line feed processing on character strings
 *   according to conversion instruction data included in tDirective.
 */
class cDirector
{
public:
    /**
     * @class cProxy
     *   The purpose of adapting the data processed by the member function
     *   included in the cDirector type to the argument of the output operator.
     */
    class cProxy
    {
        const std::string str_;
        friend std::ostream& operator<<(std::ostream& os, const cProxy rhs)
        {
            return os << rhs.str_;
        }
    public:
        cProxy(const std::string& str)
            : str_(str)
        {}
    };
    /**
     * @param [in] sIndentStr
     *   A string used for indenting.
     */
    cDirector(const std::string sIndentStr)
        : sIndentStr_(sIndentStr)
    {}
    /**
     * @param [in] first
     *   Head of a data to be converted.
     * @param [in] last
     *   Tail of a data to be converted.
     * @param [in] it
     * - points to the current element in the std::vector<cLinePos>,
     * - which is  necessary for controlling the line feed position
     *   and indentation amount is stored.
     * @param [in] ite
     * - points to tail of the std::vector<cLinePos>.
     */
    template <typename BaseIter>
    cProxy oConvert(
        const BaseIter first
        , const BaseIter last
        , tDirective::const_iterator it
        , const tDirective::const_iterator ite
    ) const
    {
        std::ostringstream oss_; ///< Temporary storage for converted string.
        int iLevel = 0;
        auto head = first;
        while (it != ite)
        {
            BaseIter tail = first;
            const auto next_NL = it->iGetColumn() + it->iGetNewLinePos();
            for (std::size_t i = 0; i < next_NL; ++i) ++tail;
            oss_ << std::string(head, tail) << std::endl;
            head = tail;
            iLevel += it->iGetIndent();
            for (int i = 0; i < iLevel; ++i)
            {
                oss_ << sIndentStr_;
            }
            ++it;
        }
        if (first != last)
        {
            oss_ << std::string(head, last);
        }
        return cProxy(oss_.str());
    }
private:
    const std::string sIndentStr_; ///< A padding to indent.
};

} // ps::lib::sql::formatter

} // ps::lib::sql

} // ps::lib

} // ps

