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
 * @struct cAnnotator
 * - Add annotations to successfully recognized token.
 */
template<typename Iterator>
class cAnnotator
{
public:
    cAnnotator(const Iterator& first)
        : first_(first)
    {}
    /**
     * @param [in,out] val
     *   A recognized token (a matched input sequence)
     * @param [in] f
     *   head of the input sequence
     * @param [in] l
     *   tail of the input sequence
     * @param [in,out] oDirective
     *   stores history of anotations
     */
    template<typename Val>
    void operator()(
        Val& val
        , const Iterator& f
        , const Iterator& l
        , tDirective& oDirective
    ) const
    {
        do_annotator(val, f, l);
        oDirective.push_back(val);
    }
private:
    const Iterator first_;
    /**
     * @param [in] li
     *    stores an object of any types derived from cLinePos.
     * @param [in] f
     *   head of the input sequence
     * @param [in] l
     *   tail of the input sequence
     */
    void do_annotator(
        cLinePos& li
        , const Iterator& f
        , const Iterator& l
    ) const
    {
        li.vSetMember(
            get_line(f)
            , get_column(first_, f)
            , std::distance(f, l)
            , std::string(f,l)
        );
    }
    void do_annotator(...) const
    {
#ifndef NDEBUG
        std::cerr << std::endl << "Not having cLinePos" << std::endl;
#endif
    }
};

} // ps::lib::sql::formatter

} // ps::lib::sql

} // ps::lib

} // ps
