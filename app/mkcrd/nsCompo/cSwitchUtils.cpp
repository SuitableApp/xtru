/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Generate Credential File(mkcrd).
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
/**
 * @file cSwitchUtils.cpp
 * @brief Perform switch parameter management
 * @author S.Sasaki
 * @date 2017/02/16
 */
#include "nsCompo.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @details
 */
cSwitchUtils::cSwitchUtils()
{}

/**
 * @details
 */
cSwitchUtils::~cSwitchUtils()
{}

/**
 * @details
 */
void cSwitchUtils::vAddOpt(const tstring& sSwitchesList, const tstring& sTagOfList)
{
    /// Available switch candidates.
    tstrset oSynonyms;
    vDelimitedStringToContainer(oSynonyms, sSwitchesList, ',');
    oSwitches_.insert(oSynonyms.begin(), oSynonyms.end());

    // Set of switches with the same meaning Add pairs of tag strings 
    // and synonyms that classify aggregates of synonyms
    oUsableSwitchesMap_.insert(
        std::map<tstring, tstrset>::value_type(
            sTagOfList                                      // Tag string for classification
            , tstrset( oSynonyms.begin(), oSynonyms.end() ) // A set of specifiable synonyms
        )
    );

}

/**
 * @details
 */
tstrset cSwitchUtils::oGetSwitches(const tstring& sTagOfList) const
{
	const auto itr = oUsableSwitchesMap_.find(sTagOfList);
	return itr->second;
}

/**
 * @details
 */
const bool cSwitchUtils::iIsSwitch(tstrvector::const_iterator c) const
{
    return oSwitches_.find(c->c_str()) != oSwitches_.end();
}

/**
 * @details
 */
void cSwitchUtils::vSearchInvalidOpt(const tstrvector& oOperands) const
{
    // Since vSearchInvalidOpt() is executed after all iSearchOpt() is called,
    // arguments in the form of some switches included in "oOperands" at this point can be regarded as invalid
    for (auto i = oOperands.cbegin(); i != oOperands.cend(); ++i)
    {
        Assert(iIsSwitch(i), cVectorScope(i, 1
            , TEXT("Command includes a not available switch. ")));
        // Confirm that only available switches are included
        for (auto j = oUsableSwitchesMap_.cbegin(); j != oUsableSwitchesMap_.cend(); ++j)
        {
            auto pos = j->second.find(*i);
            Assert(pos != j->second.cend(), cVectorScope(i, 1
                , TEXT("Command includes a not available switch. ")));
        }
    }
}

} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
