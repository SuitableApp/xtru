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

namespace app
{

namespace xtru
{

namespace getmeta
{

/**
 * @class cFunctionalReplace
 * -# This class is useful for converting strings with variable parts
 *   such as dates to strings that conform to different formats.
 * -# Variable part can also change the order of places.
 * -# You can also convert just literals that do not contain
 *   variables to other literals.
 *
 * @par Example way to be used:
 * @code
    // Declare a variable without the argument.
    cFunctionalReplace oFunctionalReplace_;

    // Register convertion rules.
    // There is no limit on the number of rules that can be registered,
    // but the more it is, the more costly the conversion will be.
    oFunctionalReplace_
        ("DEFAULT", "default")
        ("MAXVALUE", "maxvalue")
        ("NULL", "null")
        (
            "TO_DATE(' %4s-%2s-%2s 00:00:00', 'S%4s-%2s-%2s HH24:MI:SS', "
            "'NLS_CALENDAR=GREGORIAN')", "to_date('{0}-{1}-{2}','{3}-{4}-{5}')")
    ;

    // Application example of functionl conversion.

    const auto s1 = oFunctionalReplace_.sConvertStringFormat("MAXVALUE");
    // s1 will be "maxvalue".

    const char* org = "TO_DATE(' 2005-04-01 00:00:00',
        'SYYYY-MM-DD HH24:MI:SS', 'NLS_CALENDAR=GREGORIAN')";
    const auto s2 = oFunctionalReplace_.sConvertStringFormat(org);
    // s2 will be "to_date('2005-04-01','YYYY-MM-DD')".
   @endcode
 */
class cFunctionalReplace
{
private:
    /**
     * @struct cRule
     * @brief
     * Define rules for conversion work.
     */
    struct cRule
    {
        /**
         * @brief
         * A pattern string containing a wildcard character string expressed as "%ns".
         * The number of characters of the wildcard character string is applied to n.
         */
        const char* szSrc;
        /**
         * @brief
         * Formatted string after conversion. The "n" in "{n}" is the occurrence
         * position of the wildcard character string ("%ns"). "n" originate zero.
         */
        const char* szDst;
    };
    /// Organized by cFunctionalReplace::cRule as an element.
    ps::lib::cVector<cRule> vct_;
    /**
     * @struct cTranslator
     */
    struct cTranslator;
public:
    /**
     * @brief
     */
    cFunctionalReplace();
    /**
     * @brief
     */
    ~cFunctionalReplace();
    /**
     * @brief
     * Add a item of conversion rule.
     *
     * @param [in] szSrc
     *   @copydoc cFunctionalReplace::cRule::szSrc
     * @param [in] szDst
     *   @copydoc cFunctionalReplace::cRule::szDst
     * @return reference to the self.
     */
    cFunctionalReplace& operator()(const char szSrc[], const char szDst[]);
    /**
     * @brief
     *
     * @param[in] szFormedString A string before conversion.
     * @param[in] iLength A length of the string.
     * @return A string after conversion. If no rules was found, return szFormedString as is.
     */
    std::string sConvertStringFormat(
        const char* szFormedString
        , const size_t& iLength
    ) const;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps
