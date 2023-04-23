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

namespace ps
{

namespace lib
{

void vSplitUserid(
    const std::string& userid
    , std::string& username
    , std::string& password
    , std::string& db_alias
){
    boost::regex re_remote("(\\w+)/(.+)@(\\S+)");
    boost::regex re_local("(\\w+)/(.+)");
    boost::smatch result;
    if ( boost::regex_match(userid, result, re_remote))
    {
        // std::cout << "remote" << std::endl;
    }
    else if ( boost::regex_match(userid, result, re_local))
    {
        // std::cout << "local" << std::endl;
    }
    else
    {
        std::string msg("Invalid userid was passed. ");
        msg += userid;
        throw std::runtime_error(msg);
    }
    username = result[1];
    password = result[2];
    db_alias = (result.size() > 3) ? result[3] : std::string();
}

std::string trim(const std::string& string, const char* trimCharacterList)
{
    std::string trimed;
    std::string::size_type left = string.find_first_not_of(trimCharacterList);
    if (left != std::string::npos)
    {
        std::string::size_type right = string.find_last_not_of(trimCharacterList);
        trimed = string.substr(left, right - left + 1);
    }
    return trimed;
}

std::string trim(const std::string& string)
{
    // The first character is a white space. Therefore, it is not a mistake.
    return trim(string, " \t\v\r\n");
}

/**
 * @details
 */
boost::filesystem::path sHasParentOrPrefixedPath(
    const boost::filesystem::path& sOrgPath
    , const boost::filesystem::path& sPrefix
){
    return sOrgPath.has_parent_path() ? sOrgPath : sPrefix / sOrgPath;
}

/**
 * @details
 *
 * @param [in] csv
 *   contains one or more integers as well as two integers tied
 *   with a hiphen (ie integer ranges).
 * @param [in] iMinValue
 *   An integer to give a low value when left side of range is omitted.
 * @param [in] iMaxValue
 *   An integer to give a high value when right side of range is omitted.
 * @return ps::lib::cSet<uint32_t>
 *
 * @par Example way to be used:
 * @code
   using ps::lib::oPrintPagesParser;
   ps::lib::cSet<uint32_t> cont = oPrintPagesParser(expr, 0U, 10U)
   @endcode
 * @par Example of output for input:
   expr       | cont
   ---------- | -------------
   "3-8"      | [3, 4, 5, 6, 7, 8]
   "-2"       | [0, 1, 2]
   "8-"       | [8, 9, 10]
   "-"        | [0, 1, ... , 10]
   "10,3,8,2" | [2, 3, 8, 10]
 */
ps::lib::cSet<uint32_t> oPrintPagesParser(
    const std::string& csv
    , const uint32_t iMinValue
    , const uint32_t iMaxValue
){
    using boost::spirit::qi::rule;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::parse;
    using boost::spirit::qi::uint_;
    using boost::spirit::qi::char_;
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::space_type;
    using Iterator = std::string::const_iterator;
    // arr stores segments from values of the csv.
    ps::lib::cVector<std::string> arr;
    // Grammer to parse csv.
    rule<Iterator, std::string()> integer
        = +char_('0','9');
    rule<Iterator, std::string(), space_type> int_range
        = -integer >> char_("-") >> -integer;
    rule<Iterator, std::string(), space_type> segment
        = *(integer | int_range);
    rule<Iterator, ps::lib::cVector<std::string>(), space_type> start
        = segment % char_(",");
    auto it = csv.cbegin(), end = csv.cend();
    // Parsing.
    const auto ret = phrase_parse(it, end, start, space, arr);
    ASSERT_OR_RAISE(
        ret
        , std::runtime_error
        , boost::format("Faild to parse with [%s].") % csv
    );
    ASSERT_OR_RAISE(
        it == end
        , std::runtime_error
        , boost::format("Encounterd syntax error to around for [%.20s...] at segment-%d in [%s].")
            % std::string(it, end) % arr.size() % csv
    );
    ps::lib::cSet<uint32_t> res;
    for (auto val : arr)
    {
        const auto pos = val.find_first_of("-");
        if (pos != std::string::npos)
        {
            it = val.cbegin(), end = val.cend();
            auto min = 0U, max = 0U;
            // Parsing.
            const auto ret = parse(it, end, -uint_ >> '-' >> -uint_, min, max);
            ASSERT_OR_RAISE(
                ret
                , std::runtime_error
                , boost::format("Faild to parse with [%s].") % val
            );
            ASSERT_OR_RAISE(
                it == end
                , std::runtime_error
                , boost::format("Encounterd syntax error to around for [%.20s...] in [%s].")
                    % std::string(it, end) % val
            );
            // to correct when the left side hand is ommited or min is too low.
            if (0U == min || min < iMinValue) min = iMinValue;
            // to correct when the right side hand is ommited or max is too high.
            if (0U == max || max > iMaxValue) max = iMaxValue;
            // Decending is rejected.
            ASSERT_OR_RAISE(
                min <= max
                , std::range_error
                , boost::format("Decending value order with [%s] in [%s].")
                    % val % csv
            );
            for (auto i = min; i <= max; i++)
            {
                if (res.find(i) == res.end())
                {
                    res.insert(i);
                }
            }
        }
        else  // unary value.
        {
            const auto i = boost::lexical_cast<uint32_t>(val);
            // Too low or high value is discareded.
            if (i < iMinValue || i > iMaxValue) continue;
            // Only effective value is captured.
            if (res.find(i) == res.end())
            {
                res.insert(i);
            }
        }
    }
    return res;
}

const std::string sConvertDollar2Sharp(const std::string& str)
{
    return boost::regex_replace(str, boost::regex("\\$"), "#");
}

const std::string sConvertEolSequence(
    const std::string& target
    , const std::string& from
    , const std::string& to
){
    return boost::algorithm::replace_all_copy(target, from, to);
}

void vRmSpecCharsAtEofStr(
    std::string& sWork
    , const std::string& chars_specific
){
    auto rit = sWork.rbegin();
    while (chars_specific.find(*rit) != std::string::npos)
    {
        ++rit;
    }
    sWork.erase(rit.base(), sWork.end());
}

const std::string sConsolidatePolyBlanksIntoMonoLf(const std::string& str)
{
    return boost::regex_replace(str, boost::regex(R"((\s*\n)+)"), "\n");
}

const std::string sGetMinimumWidth(
    const std::string& sOrg
    , const int32_t& iLenToKeep
    , const int iCharToPadd
){
    const char *szOrg = sOrg.c_str();
    const size_t iOrg = sOrg.size();
    int iChars = 0;
    size_t iPos = 0;
    while (iPos < iOrg)
    {
        const int iNumBytes = std::mblen(&szOrg[iPos], iOrg - iPos);
        ASSERT_OR_RAISE(
            iNumBytes > 0
            , std::range_error
            , boost::format(
            "Failed to execute std::mblen. Despite using multibyte characters, "
            "category LC_CTYPE in the locale may not be initialized properly. "
            "Current value is %s."
            ) % std::setlocale(LC_CTYPE, 0)
        );
        iChars += (iNumBytes > 1 ? 2 : 1);
        iPos += iNumBytes;
    }
    int32_t iPadding = iLenToKeep - iChars;
    if (iPadding < 0) iPadding = 0;
    return sOrg + std::string(iPadding, iCharToPadd);
}

std::string sMakeEnclosedName(
    const std::string& sOrg
    , const int32_t& iLenToKeep
){
    const auto sRes = (boost::format(R"("%s")") % sOrg).str();
    return iLenToKeep ? sGetMinimumWidth(sRes, iLenToKeep) : sRes;
}

void vConvPlaceHolder(
    const ps::lib::str_vct& opts
    , std::string& sql
    , std::ostream& os
){
    const std::string ph("%s"); // Pattern of C-printf formatting style place-holder.
    if ( ! opts.empty())
    {
        os << boost::format("\"%s\" in the SQL-statement will be replaced with following values:" ) % ph
            << opts << std::endl;
    }
    auto itv = opts.cbegin();
    auto pos = sql.find(ph);
    BOOST_ASSERT(pos == std::string::npos || opts.size());
    // Are there C-printf style place-holders in the SQL Statement?
    while (pos != std::string::npos && itv != opts.cend())
    {
        ASSERT_OR_RAISE(itv != opts.end(), std::runtime_error
            , "Too few arguments given to placeholders in sql."
        );
        sql.replace(pos, ph.size(), *itv);
        pos = sql.find(ph, pos + itv->size());
        ++itv;
    }
    ASSERT_OR_RAISE(itv == opts.end(), std::runtime_error
        , "Too many arguments given to placeholders in sql."
    );
}

/**
 * @detail
 */
void vTrim(std::string& str, const char* iCharSet)
{
    std::string result;
    // Search from the left side for positions where non-iCharSet characters appear
    const auto left = str.find_first_not_of(iCharSet);
    if (left != std::string::npos)
    {
        // Search from the right side for positions where non-iCharSet characters appear
        const auto right = str.find_last_not_of(iCharSet);
        result = str.substr(left, right - left + 1);
    }
    str.swap(result);
}

} // ps::lib

} // ps

