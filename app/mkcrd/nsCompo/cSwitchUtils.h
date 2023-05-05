/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Generate Credential File(mkcrd).
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
/**
 * @file cSwitchUtils.h
 * @brief Perform switch parameter management
 * @author S.Sasaki
 * @date 2017/02/16
 */
#ifndef CSWITCHUTILS_H
#define CSWITCHUTILS_H
#include "../define.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @class cSwitchUtils
 * @brief 
 *  This class is perform switch parameter management.
 */
class cSwitchUtils
{
public:
    /**
     * @brief 
     */
    cSwitchUtils();

    /**
     * @brief 
     */
    ~cSwitchUtils();

    /**
     * @brief 
     *  Register switch parameters
     * @param[in] sSwitchesList
     *  A set of synonyms delimited by delimiter characters
     * @param[in] sTagOfList
     *  Tag for handling a set of synonyms together
     */
    void vAddOpt(const tstring& sSwitchesList, const tstring& sTagOfList);

    /**
     * @brief 
     *  checkes whether it is an acceptable candidate as a switch.
     * @param[in] c
     *  Target iterator
     * @return false:Not a switch. true:It is a switch.
     */
    const bool iIsSwitch(tstrvector::const_iterator c) const;
    
    /**
     * @brief 
     *  retrives a set of switches which is matched with sTagOfList.
     * @param[in] sTagOfList
     *  Tag for finding
     * @return A set of switches
     */
    tstrset oGetSwitches(const tstring& sTagOfList) const;

    /**
     * @brief 
     *  Check if there are incorrect parameters
     * @param[in] oOperands
     *  Information for managing valid switches
     */
    void vSearchInvalidOpt(const tstrvector& oOperands) const;

private:
    /// saves all acceptable patterns as expressions of switches.
    tstrset oSwitches_;
    /// A set which contains switch strings is related with a tag string
    std::map<tstring, tstrset> oUsableSwitchesMap_;
};

} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CSWITCHUTILS_H */
