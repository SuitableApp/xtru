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

namespace bqi = boost::spirit::qi;
namespace phx = boost::phoenix;

/**
 * @class cLinePos
 * - An object of this class stores a position of the token
 *   which is matched for the rule.
 */
class cLinePos
{
public:
    typedef enum { iForward = -1 ///< New line is placed forward the token.
                , iBackward =  0 ///< New line is placed backward the token.
    } tNewLineEntry;
    friend std::ostream& operator<<(std::ostream& os, const cLinePos& rhs)
    {
        return rhs.sDebugPrint(os);
    }
    void vSetMember(
        const std::size_t& line
        , const std::size_t& column
        , const std::size_t& length
        , const std::string& input_token
    ){
        line_ = line;
        column_ = column;
        length_ = length;
        input_token_ = input_token;
        boost::format bfmt("Annotating [%-12s] pos:%4d, len:%2d, indent:%2d, pos of NL:%s");
#ifndef NDEBUG
        std::cerr
#else
        auto& trc = ps::lib::cTracer::get_mutable_instance();
        trc
#endif
            << bfmt % input_token % column % length % iIndent_ % (iNewLineEntry_ == iForward ? "F" : "B")
            << std::endl;
    }
    std::size_t iGetColumn(void) const
    {
        return column_;
    }
    /**
     * @return
     *   <0 : Decrease indentation.
     *   >0 : Increase indentation.
     */
    int iGetIndent(void) const
    {
        return iIndent_;
    }
    int iGetNewLinePos(void) const
    {
        return iNewLineEntry_;
    }
protected:
    cLinePos(
        const int& iIndent
        , const tNewLineEntry& iNewLineEntry
    )
        : iIndent_(iIndent)
        , iNewLineEntry_(iNewLineEntry)
        , line_(0), column_(0), length_(0)
    {}
private:
    std::ostream& sDebugPrint(std::ostream& os) const
    {
        boost::format bfmt("line=%d, column=%d, length=%d, iIndent=%d, iNewLineEntry=%d");
        return os << bfmt % line_ % column_ % length_ % iIndent_ % (iNewLineEntry_ == iForward ? "F" : "B");
    }
    const int iIndent_;  ///< amount of indent, positive means increasing and negative means decreasing
    const tNewLineEntry iNewLineEntry_;
    std::size_t line_    ///< number of line.
              , column_  ///< distance from the start of line.
              , length_  ///< size of matched token.
              ;
    std::string input_token_;
};

/**
 * @struct cKeyword
 * - Types of keywords that require prefix newlines.
 * - There is no increase or decrease of the indent amount.
 */
struct cKeyword
    : public cLinePos
{
    std::string str_;
    cKeyword()
        : cLinePos(0, tNewLineEntry::iForward)
    {}
};

/**
 * @struct cNormDelim
 * - Type of delimiter no increase or decrease indentation. (0)
 * - Immediately after that put a new line.
 */
struct cNormDelim
    : public cLinePos
{
    char ascii_;
    cNormDelim()
        : cLinePos(0, tNewLineEntry::iForward)
    {}
};

/**
 * @struct cIncrDelim
 * - Type of delimiter to increase indentation. (+1)
 * - Immediately after that put a new line.
 */
struct cIncrDelim
    : public cLinePos
{
    char ascii_;
    cIncrDelim()
        : cLinePos(+1, tNewLineEntry::iBackward)
    {}
};

/**
 * @struct cDecrDelim
 * - Type of delimiter to decrease indentation. (-1)
 * - Immediately after that put a new line.
 */
struct cDecrDelim
    : public cLinePos
{
    char ascii_;
    cDecrDelim()
        : cLinePos(-1, tNewLineEntry::iForward)
    {}
};

typedef std::vector<cLinePos> tDirective;

/**
 * @struct cErrorHandler
 * - Notify the location where the error occurred.
 */
struct cErrorHandler
{
    template<typename Iterator, typename Str>
    bqi::error_handler_result operator()(
        Iterator f
        , Iterator l
        , Iterator err_pos
        , Str const& what
    ) const
    {
#ifndef NDEBUG
        std::cerr
#else
        auto& trc = ps::lib::cTracer::get_mutable_instance();
        trc
#endif
            << boost::format("%s expecting %s in line %d: %s %d <---- here")
                % sClass(ps::lib::E)
                % what
                % get_line(err_pos)
                % std::string(f ,l)
                % std::setw(std::distance(f, err_pos))
            << std::endl;
        return bqi::fail;
    }
};

/**
 * @brief
 *   Parse the iterator pair based on the grammar defined for P
 *   and store the conversion directive data set into P.
 * @param [in] first
 *   head of string to parse.
 * @param [in] last
 *   tail of string to parse.
 * @param [in,out] iter
 * - When calling a function, give the same place as first.
 * - The place pointed as the parse place advances also moves.
 * - If parse is successful, the same place as last is returned.
 */
template<class P, class Iterator>
void vParse(
    P& parsar
    , const Iterator& first
    , const Iterator& last
    , Iterator& iter
){
    const bool status = phrase_parse(iter, last, parsar, bqi::space);
    if (!status || iter != last)
    {
        std::ostringstream oss;
        oss
            << std::string("ERROR: Parsing failed or not complete")
            << std::endl
            << boost::format("stopped at: line:%d, column:%d, offset:%d")
                % get_line(iter)
                % get_column(first, iter)
                % (std::distance(first, iter) + 1)
            << std::endl
            << boost::format("might be wrong here: '%s'")
                % std::string(iter, last)
            ;
        throw std::runtime_error(oss.str());
    }
}

} // ps::lib::sql::formatter

} // ps::lib::sql

} // ps::lib

} // ps

/// Cast modern magical spells.
BOOST_FUSION_ADAPT_STRUCT(ps::lib::sql::formatter::cKeyword, (std::string, str_))
BOOST_FUSION_ADAPT_STRUCT(ps::lib::sql::formatter::cNormDelim, (char, ascii_))
BOOST_FUSION_ADAPT_STRUCT(ps::lib::sql::formatter::cIncrDelim, (char, ascii_))
BOOST_FUSION_ADAPT_STRUCT(ps::lib::sql::formatter::cDecrDelim, (char, ascii_))

