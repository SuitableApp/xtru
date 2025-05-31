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
#include "cFunctionalReplace.h"

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @struct cFunctionalReplace::cTranslator
 */
struct cFunctionalReplace::cTranslator
{
    /**
     * @enum tConstant
     */
    enum tConstant
        { MAX_NUM_CHARS = 8 + 1  ///< Maximum number of bytes of string that can be replaced with "%ns"
        , MAX_NUM_VARIABLES = 12 ///< Maximum number of "%ns" can be included
        , MAX_DIGITS = 2 + 1     ///< Maximum number of integer digits that can be placed between '%' and 's'
    };
    /**
     * @brief
     */
    char* szStrToMatch_;
    /**
     * @brief
     * - Partial character strings of cTranslator::szStrToMatch_
     *   extracted by matching "%ns" are stored in <b>order of appearance</b>.
     * - Complete after calling cTranslator::vTransferMarkupLocation.
     */
    char szTbl_[MAX_NUM_VARIABLES][MAX_NUM_CHARS];
    /**
     * @brief
     * @param[in] szStrToMatch
     *   String to be converted
     * @param[in] iLength
     */
    cTranslator(const char* szStrToMatch, const size_t& iLength);
    cTranslator(const std::string& sStrToMatch);
    cTranslator(const cTranslator& rhs);
    ~cTranslator();
    /**
     * @brief
     * Verify that there is a rule matching cTranslator::szStrToMatch_.
     * @return true: Discovered matched rules.
     */
    bool operator()(const cRule& oRule) const;
    /**
     * @brief
     * - finds %ns from cRule::szSrc, extracts the partial string of the corresponding position
     *   from cTranslator::szStrToMatch_ 'n' digits, and stores it in cTranslator::szTbl_ in order.
     * - Inevitably, the index of cTranslator::szTbl_ is the appearance order of %ns in cRule::szSrc.
     */
    void vTransferMarkupLocation(ps::lib::cVector<cRule>::const_iterator oRule);
    /**
     * @brief
     * Returns a string obtained by replacing "[n]" with data
     *   searched from szTbl_ for "n" as an order index orignated zero.
     */
    std::string sReplaceMarkupLocation(ps::lib::cVector<cRule>::const_iterator oRule) const;
};

/**
 * @details
 */
cFunctionalReplace::cTranslator::cTranslator(const char* szStrToMatch, const size_t& iLength)
    : szStrToMatch_(new char[iLength + 1])
{
    ::memcpy(szStrToMatch_, szStrToMatch, iLength);
    szStrToMatch_[iLength] = '\0';
    std::memset(szTbl_, 0, MAX_NUM_CHARS * MAX_NUM_VARIABLES);
}

/**
 * @details
 */
cFunctionalReplace::cTranslator::cTranslator(const std::string& sStrToMatch)
    : cTranslator(sStrToMatch.c_str(), sStrToMatch.size())
{}

/**
 * @details
 */
cFunctionalReplace::cTranslator::cTranslator(const cTranslator& rhs)
    : szStrToMatch_(new char[::strlen(rhs.szStrToMatch_) + 1])
{
    std::strncpy(szStrToMatch_, rhs.szStrToMatch_, ::strlen(rhs.szStrToMatch_));
    szStrToMatch_[::strlen(rhs.szStrToMatch_)] = '\0';
    for (int i = 0; i < MAX_NUM_VARIABLES; ++i)
    {
        ::memset(szTbl_[i], 0, MAX_NUM_CHARS);
        if (::strlen(rhs.szTbl_[i]) > 0)
        {
            std::strncpy(szTbl_[i], rhs.szTbl_[i], MAX_NUM_CHARS - 1);
            szTbl_[i][MAX_NUM_CHARS - 1] = '\0';
        }
    }
}

/**
 * @details
 */
cFunctionalReplace::cTranslator::~cTranslator()
{
    delete [] szStrToMatch_;
}

/**
 * @details
 */
bool cFunctionalReplace::cTranslator::operator()(const cRule& oRule) const
{
    auto iRet = true;
    auto p = szStrToMatch_;
    auto q = oRule.szSrc;
    while (strlen(p) > 0 && strlen(q) > 0)
    {
        if (*q == '%')
        {
            // Up to 10 decimal digits can be stored in string notation.
            char digit_[MAX_DIGITS];
            auto i = 0;
            ::memset(digit_, 0, sizeof(digit_));
            q++;
            while (*q != 's') digit_[i++] = *q++;
            q++;
            p += atoi(digit_);
        }
        else
        {
            iRet = *p == *q;
            if (!iRet)
            {
                // Even if one character is different, everything is different.
                break;
            }
            p++; q++;
        }
    }
    return iRet;
}

/**
 * @details
 */
void cFunctionalReplace::cTranslator::vTransferMarkupLocation(
    ps::lib::cVector<cRule>::const_iterator oRule
){
    const char *p = szStrToMatch_; // Original string be be converted.
    const char *q = oRule->szSrc;  // contains "%sn" as a place holder.
    int iOdr = 0;                  // order of "%ns" appearance on cRule::szSrc.
    while (*p)
    {
        if (*q == '%')
        {
            // Cut out the substring between '%' and 's',
            // create an integer string and copy it to digit_.
            char digit_[MAX_DIGITS];
            ::memset(digit_, 0, sizeof(digit_));
            q++;
            int i = 0;
            while (*q != 's') digit_[i++] = *q++;
            q++;
            ASSERT_OR_RAISE(iOdr <= MAX_NUM_VARIABLES, std::out_of_range, boost::format(
                R"(A subscript other than the valid range [%d-%d] was specified. Actual value:%d)"
            ) % 0 % (MAX_NUM_VARIABLES - 1) % iOdr);

            const int iNumDigit = atoi(digit_);
            ASSERT_OR_RAISE(iNumDigit <= MAX_NUM_CHARS, std::out_of_range, boost::format(
                R"(A subscript other than the valid range [%d-%d] was specified. Actual value:%d)"
            ) % 0 % (MAX_NUM_CHARS - 1) % iNumDigit);
            ::memset(szTbl_[iOdr], 0, MAX_NUM_CHARS);
            ::strncpy(szTbl_[iOdr], p, iNumDigit);
            p += iNumDigit;
            iOdr++;
        }
        else
        {
            p++; q++;
        }
    }
}

/**
 * @details
 */
std::string cFunctionalReplace::cTranslator::sReplaceMarkupLocation(
    ps::lib::cVector<cRule>::const_iterator oRule
) const {
    char szBuf[MAX_NUM_CHARS * MAX_NUM_VARIABLES];
    ::memset(szBuf, 0, sizeof(szBuf));
    char *p = szBuf;
    const char *q = oRule->szDst;
    while (*q)
    {
        if (*q == '{')
        {
            // Up to 10 decimal digits can be stored in string notation.
            char digit_[MAX_DIGITS];
            ::memset(digit_, 0, sizeof(digit_));
            q++;
            int i = 0;
            while (*q != '}') digit_[i++] = *q++;
            q++;
            strcpy(p, szTbl_[atoi(digit_)]);
            p += strlen(szTbl_[atoi(digit_)]);
            const size_t iWritten = p - szBuf;
            ASSERT_OR_RAISE(iWritten < sizeof(szBuf), std::out_of_range, boost::format(
                R"(The size of the written data exceeded the area size (%d) of the internal buffer.)"
            ) % sizeof(szBuf));
        }
        else
        {
            *p++ = *q++;
        }
    }
    return std::string(szBuf);
}

/**
 * @details
 */
cFunctionalReplace::cFunctionalReplace()
{
    // Nothing to do.
}

/**
 * @details
 */
cFunctionalReplace::~cFunctionalReplace()
{
    // Nothing to do.
}

/**
 * @details
 */
cFunctionalReplace& cFunctionalReplace::operator()(const char szSrc[], const char szDst[])
{
    vct_.push_back({szSrc, szDst});
    return *this;
}

/**
 * @details
 */
std::string cFunctionalReplace::sConvertStringFormat(
    const char* szFormedString
    , const size_t& iLength
) const {
    BOOST_ASSERT(iLength > 0);
    BOOST_ASSERT(vct_.size() > 0);

    cTranslator oMatch(szFormedString, iLength);
    ps::lib::cVector<cRule>::const_iterator oCur
        = std::find_if(vct_.begin(), vct_.end(), oMatch);
    if (oCur == vct_.end())
    {
        // Could not find the corresponding rule.
        return std::string(szFormedString);
    }
    oMatch.vTransferMarkupLocation(oCur);
    return oMatch.sReplaceMarkupLocation(oCur);
}

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
