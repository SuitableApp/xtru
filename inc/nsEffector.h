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
 * @brief
 * - Between username and password is splited by a slash that is mandatory.
 * - Between passwrod and db_alias is splited by an at-mark that is optional.
 *   (if at-mark is ommited, db_alias length set to  zero.
 *   because destination is local,)
 *
 * @param [in]     userid
 * @param [in,out] username
 * @param [in,out] password
 * @param [in,out] db_alias
 *
 * @note Example way to be used:
 *  userid            |username|password|db_alias
 *  ------------------|--------|--------|--------
 * "SCOTT/TIGER@ORCL" |"SCOTT" |"TIGER" |"ORCL"
 * "SCOTT/TIGER"      |"SCOTT" |"TIGER" |""
 */
extern void vSplitUserid(
    const std::string& userid
    , std::string& username
    , std::string& password
    , std::string& db_alias
);

/**
 * @brief
 *   converts from an integer which is represented
 *   by string suffixed multiples symbol to a binary integer.
 *
 * @param [in]     sInt
 * -  is a number formed string.
 * -  The last symbol character is optional and qualifies the forward numerical value.
 *  - k : * 1024^1
 *  - M : * 1024^2
 *  - G : * 1024^3
 *  - T : * 1024^4
 *  - P : * 1024^5
 * @param [in,out] iValue
 * -  is a binary number of @b tRType type that receives the conversion result.
 *
 * @note Example way to be used:
 *   sInt   |iValue
 *   -------|--------------------------
 *   "123k" |125952 (=123*1024)
 *   "123M" |128974848 (=123*1024*1024)
 */
template<typename tRType>
void vIntStrToBinInt(
    const std::string& sInt
    , tRType& iValue
){
    BOOST_ASSERT(sInt.size());
    static ps::lib::cMap<char, int32_t> syms =
    {{'.',0}, {'K',1},{'M',2}, {'G',3}, {'T',4}, {'P',5}};
    const char *iUnit = strpbrk(sInt.c_str(), ".KkMmGgTtPp");
    if (!iUnit)
    {
        std::string::size_type pos = sInt.find_first_not_of("0123456789");
        ASSERT_OR_RAISE(
            pos == std::string::npos, std::out_of_range
            , std::string("Unexpected symbol '") + sInt[pos] + "' encountered in \"" + sInt + "\"."
        );
    }
    std::string sBuf(sInt.cbegin(), sInt.cend() - (iUnit ? 1 : 0));
    double fBuff = boost::lexical_cast<double>(sBuf);
    if (iUnit)
    {
        int32_t iUpper = toupper(*iUnit);
        fBuff *= pow(1024.0, syms.at(iUpper));
    }
    iValue = static_cast<tRType>(fBuff);
}

template<typename tRType>
tRType iIntStrToBinInt(
    const std::string& sInt
){
    tRType iValue;
    vIntStrToBinInt(sInt, iValue);
    return iValue;
}

template<typename tRType>
tRType iInvExp(
    tRType iMantissa
    , uint32_t iIndex
){
    if (iIndex > 0)
    {
        iIndex--;
        iMantissa = iInvExp(iMantissa, iIndex);
        iMantissa >>= 10;  // This computes 1/1024 as before.
    }
    return iMantissa;
}

/**
 * Converts from a binary integer to an integer
 * which is represented by string suffixed multiples symbol.
 * ex)
 *   from 125952 (=123*1024) to "123k"
 *   from 128974848 (=123*1024*1024) to "123M"
 */
template<typename tPType>
std::string sBinIntToIntStr(const tPType iInt, const std::string& sSpace=" ")
{
    static char sSyms[] = ".kMGTP";  // multiples symbols
    tPType iSign = iInt;    // identifies whether iInt is less than zero.
    tPType iMantissa = iInt;
    // computes a sign of iInt.
    iSign >>= sizeof(iInt) * 8 - 1;
    // computes an absolute number of iInt for comparison.
    if (iSign == (tPType) -1) iMantissa = (iMantissa ^ -1) + 1;
    // Finds an appropriate index.
    uint32_t iIndex = 0;
    while (iInvExp(iMantissa, iIndex) > 10000) iIndex++;
    return boost::lexical_cast<std::string>(iInt >> (iIndex * 10)) + sSpace + sSyms[iIndex];
}

/**
 * Converts from a binary integer to an integer
 * which is represented by string.
 * And returned value can be splitted by a comma for inberval three digits.
 * ex)
 *   from 543223552  to "543,223,552"
 */
template<typename tPType>
std::string sIntToa(
    tPType iInt
    , const int32_t iInterval =3
){
    tPType iSign = iInt;    // identifies whether iInt is less than zero.
    std::string sInv;       // inverted order entry.
    // computes a sign of iInt.
    iSign >>= sizeof(iInt) * 8 - 1;
    // computes an absolute number of iInt for comparison.
    if (iSign == (tPType) -1) iInt = (iInt ^ -1) + 1;
    int32_t i;
    for (i = 0;iInt > 0; i++)
    {
        if (iInterval > 0 && i > 0 && (i % iInterval) == 0)
        {
            sInv += ',';
        }
        sInv += (char)((iInt % 10) + '0');
        iInt = iInt / 10;
    }
    if (0 == i) sInv += '0';
    sInv += 0 == iSign ? ' ' : '-';
    return std::string(sInv.rbegin(), sInv.rend());
}

template<typename T>
void vSplitToFlagmentsImpl(const T&, const typename T::value_type&, const typename T::size_type)
{
    /// to stop recursive call.
}

template<typename T, typename... Rest>
void vSplitToFlagmentsImpl(const T& splittee, const typename T::value_type& splitter
        , const typename T::size_type pos, T& first, Rest&... args)
{
    const auto next = splittee.find(splitter, pos);
    if (next != std::string::npos)
    {
        vSplitToFlagmentsImpl(splittee, splitter, next + 1LU, args...);
    }
    else
    {
        vSplitToFlagmentsImpl(splittee + splitter + first, splitter, next + 1LU, args...);
    }
    first.append(splittee, pos, next);
}

template<typename T, typename... Args>
void vSplitToFlagments(const T& splittee, const typename T::value_type& splitter
        , Args&... args)
{
    vSplitToFlagmentsImpl(splittee, splitter, 0LU, args...);
}

std::string trim(const std::string& string, const char* trimCharacterList);

std::string trim(const std::string& string);

/**
 * @brief
 *
 * @param[in] sOrgPath
 *   Concatenation of optional directory name and required file name.
 * @param[in] sPrefix
 *   An alternative when sOrgPath does not contain a directory (option).
 *
 * @return
 *   if sOrgPath has path delimiter(s) ('/'), returns as it is
 *   else if sOrgPath is empty, returns always sPrefix
 *   else returns computed sPrefix / sOrgPath
 *
 * @note
 *   No validate whether path (sOrgPath, sPrefix or return value) is existing.
 *
 * @par Example way to be used:
 * @code
    const boost::filesystem::path a = "aaa"
                                , b = "/bbb"
                                , c = "ccc/"
                                , d = "../ddd/"
                                , e = ""
                                , prefix = "prefix"
                                ;
    std::cout
        << sHasParentOrPrefixedPath(a, prefix) << std::endl // "prefix/aaa"
        << sHasParentOrPrefixedPath(b, prefix) << std::endl // "/bbb"
        << sHasParentOrPrefixedPath(c, prefix) << std::endl // "ccc/"
        << sHasParentOrPrefixedPath(d, prefix) << std::endl // "../ddd/"
        << sHasParentOrPrefixedPath(e, prefix) << std::endl // "prefix"
        ;
   @endcode
 */
extern boost::filesystem::path sHasParentOrPrefixedPath(
    const boost::filesystem::path& sOrgPath
    , const boost::filesystem::path& sPrefix
);

/**
 * @brief
 *   parses string expressions that select one or more
 *   discrete or continuous range integers.
 *
 * Rule to parse.
 * -# One or both ends of the integer range can be omitted.
 *    @b iMinValue is refered when left hand side is omitted,
 *    and @b iMaxValue is refered when right hand side is omitted.
 *   - ex) '-' means between @b iMinValue and @b iMaxValue
 *   - ex) 'N-' means between N and @b iMaxValue
 *   - ex) '-M' means between @b iMinValue and M
 * -# An integer range of descending order will throw std::range_error.
 *    However, Comma separated values will not be rejected even
 *    in descending order from each other.
 *   - ex) '2,8-5' throwes std::range_error. But '12-,4,7-8,1' is not rejected.
 * -# Duplicate values are merged into one.
 * -# A negative value is ignored and does not throw exception.
 */
ps::lib::cSet<uint32_t> oPrintPagesParser(
    const std::string& csv
    , const uint32_t iMinValue
    , const uint32_t iMaxValue
);

/**
 * if str containes a (or more) dollar letter(s) it can be replaced each to the sharp.
 * because in *inux platform environment, the file name containing the dollar sign is
 * extremely difficult to handle.
 */
const std::string sConvertDollar2Sharp(const std::string& str);

/**
 * @brief
 *   Convert EOL sequence.
 * @param [in] target
 * @param [in] from  example: "\r\n"
 * @param [in] to    example: "\n"
 */
const std::string sConvertEolSequence(
    const std::string& target
    , const std::string& from
    , const std::string& to
);

/**
 * @brief
 *   Remove any characters in chars_specific if they are used at the end of the sWork.
 * @param [in,out] sWork
 *   - before:: "ABC DEF \r\n HIJ\r\n"
 *   - after::  "ABC DEF \r\n HIJ"
 * @param [in] chars_specific
 *   - Specify a set of the characters which should be removed.
 */
void vRmSpecCharsAtEofStr(
    std::string& sWork
    , const std::string& chars_specific
);

/**
 * @brief
 * If there is a sequence that a line-feed forward by zero or more blank characters
 *  (such as tabs or 0x30) repeated more than once, it is converted to single line-feed.
 * Example of functionality of converting.
 * - "ABC\n  \n\n\t \t \nDEF  \n \t \nGHI\n \n"
 * -    will be converted to  "ABC\nDEF\nGHI\n"
 * @param [in] str
 *   A original string.
 */
const std::string sConsolidatePolyBlanksIntoMonoLf(const std::string& str);

/**
 * @brief
 * - The display width is adjusted so that the display position is not disturbed
 *   even if sOrg contains multi-byte characters.
 * @param [in] sOrg
 *   A string which would like to process.
 * @param [in] iLenToKeep
 * - When the length of sOrg is less than the length specified
 *   in this argument, a character, passed to iCharToPadd is padded.
 * @param [in] iCharToPadd
 * - Characters used for padding.
 * @note
 *   If the length of sOrg exceeds iLenToKeep,
 *   the entire sOrg enclosed in quotation marks is returned.
 */
const std::string sGetMinimumWidth(
    const std::string& sOrg
    , const int32_t& iLenToKeep
    , const int iCharToPadd = ' '
);

/**
 * @brief
 * - Returns a string enclosed in double quotes.
 * - The display width is adjusted so that the display position is not disturbed
 *   even if sOrg contains multi-byte characters.
 * @param [in] sOrg
 *   A string which would like to be enclosed by the quotes.
 * @param [in] iLenToKeep
 * - When the length of sOrg is less than the length specified
 *   in this argument, halfwidth space is padded.
 * - If the argument is omitted, it is not padded.
 * @return
 * - An object name that is enclosed by the quotes.
 * - e.g.) (in) EMP -> (out) "EMP"
 */
std::string sMakeEnclosedName(
    const std::string& sOrg
    , const int32_t& iLenToKeep = 0
);

/**
 * Place-holders (=%s) in SQL-statement will be replaced
 * by given "opts" which is arraied string.
 *
 * @param[in] opts
 *   stores values for %s place-holder in sql_.
 * @param[in,out] sql
 *   A string to replace.
 * @param[in,out] os
 *   A streaming for diagnostic message.
 */
void vConvPlaceHolder(
    const ps::lib::str_vct& opts
    , std::string& sql
    , std::ostream& os
);

/**
 * @brief
 */
extern void vTrim(std::string& str, const char* iCharSet = " \t\v\r\n");

} // ps::lib

} // ps

