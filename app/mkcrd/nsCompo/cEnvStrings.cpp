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
 * @file cEnvStrings.cpp
 * @brief Control environment variables
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include "nsCompo.h"
#include "cEnvStrings.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @class cEnvStringsImpl
 * @brief 
 *  This class is an implementation class for controls environment variables.
 */
class cEnvStringsImpl
{
public:
    /**
     * @brief 
     */
    cEnvStringsImpl()
    {}
    
    /**
     * @brief 
     */
    ~cEnvStringsImpl()
    {}

    /**
     * @brief 
     *  Perform initial setting of input processing.
     * @param[out] oMap
     *  Map containing environment variables.
     * @param[in] szEnviron
     *  Environment variable.
     */
    void vGetTable(tstrmap& oMap, char* szEnviron[])
    {
        char **szP = szEnviron;
        for(int lp=0; szP[lp]; lp++)
        {
            tstring sKey;
            sKey.push_back('%');
            char *pPtr = szP[lp];
            while (*pPtr != '=')
            {
                sKey.push_back(toupper(*pPtr));
                pPtr++;
            }
            sKey.push_back('%');
            pPtr++;
            tstring sVal;
            while (*pPtr)
            {
                sVal.push_back(*pPtr);
                pPtr++;
            }
            oMap.insert(tstrmap::value_type(sKey, sVal));
        }
    }
private:
    ///< The following operation is prohibited
    cEnvStringsImpl(const cEnvStringsImpl&);               ///< Copy
    cEnvStringsImpl& operator=(const cEnvStringsImpl&);    ///< Substitution
};

/**
 * @details
 */
cEnvStrings::cEnvStrings()
    : Impl_(new cEnvStringsImpl())
{}

/**
 * @details
 */
cEnvStrings::~cEnvStrings()
{}

/**
 * @details
 */
void cEnvStrings::vGetTable(tstrmap& oMap, const char *szEnvp[])
{
    Impl_->vGetTable(oMap, (char**)szEnvp);
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

