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
 * @file cArgUtils.h
 * @brief Analyze the specified parameter
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CARGUTILS_H
#define CARGUTILS_H
#include "../define.h"
#include "cVectorScope.h"
#include "cSwitchUtils.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

class cArgUtilsImpl;
class cArgStyleParser;

/**
 * @class cArgUtils
 * @brief 
 *  This class is analyze the specified parameter.
 */
class cArgUtils
{
protected:
    /**
     * @brief 
     *  Parameter analysis initial processing
     * @param[in] iArgc Number of parameters
     * @param[in] szArgv parameters
     * @param[in] oAnyArgStyle Parameter analysis parser
     */
    explicit cArgUtils(
        const int iArgc
        , const _TCHAR ** szArgv
        , cArgStyleParser* oAnyArgStyle
    );

    /**
     * @brief 
     *  Specify and initialize an implementation class.
     * @param[in] oRhs Implementation class
     */
    cArgUtils(cArgUtils& oRhs);

    /**
     * @brief 
     *  Specify and initialize an implementation class.
     * @param[in] oRhs Implementation class
     */
    cArgUtils& operator=(cArgUtils& oRhs);

    /**
     * @brief 
     */
    ~cArgUtils();
    
    /**
     * @brief 
     *  Analyze and store parameters
     * @param[in] sTagOfList
     *  Tag for handling a set of synonyms together
     * @param[in] iExclusiveGrp
     *  Give the same index to sets of synonyms to be exclusively used
     * @param[in] iMinNumParams
     *  Specify the lower limit of the number of arguments that the switch is allowed to have
     * @param[in] iMaxNumParams
     *  Specify the maximum number of arguments that the switch is allowed to have
     * @param[in] oSu
     *  Information for managing valid switches
     * @param[out] oSw
     *  The location of the detected switch is set
     * @return false:Switch was not found. true:The position of the place where the switch was found returns.
     */
    const bool iSearchOpt(
        const tstring& sTagOfList
        , const int& iExclusiveGrp
        , const tstrvector::size_type iMinNumParams
        , const tstrvector::size_type iMaxNumParams
        , cSwitchUtils& oSu
        , cVectorScope& oSw
    );

    /**
     * @brief 
     *  Check if tag exists
     * @param[in] sTagOfList
     *  Tag for handling a set of synonyms together
     * @return false:There is no tag. true:Tag exists.
     */
    const bool iDoesTagExist(
        const tstring& sTagOfList
    ) const;
    
    /**
     * @brief 
     *  Check if there are incorrect parameters
     * @param[in] oSu
     *  Information for managing valid switches
     */
    void vSearchInvalidOpt(cSwitchUtils& oSu) const;
     
    /**
     * @brief 
     */
    tstrvector::const_iterator begin() const;

    /**
     * @brief 
     */
    tstrvector::const_iterator end() const;

    /**
     * @brief 
     */
    tstrvector::size_type size() const;

    /**
     * @brief 
     */
    const bool empty() const;

    /**
     * @brief 
     *  Get the number of runtime arguments
     * @return Number of run-time arguments (== argc)
     */
    const int iGetArgc() const;
private:
    std::unique_ptr<cArgUtilsImpl> Impl_;
};
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CARGUTILS_H */
