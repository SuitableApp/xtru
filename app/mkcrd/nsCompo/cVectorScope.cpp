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
 * @file cVectorScope.cpp
 * @brief Class for making it possible to treat consecutive elements included in "std :: vector" as if seeing through the window
 * @author S.Sasaki
 * @date 2017/01/10
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
const tstring cVectorScope::sShowMsg(const tstring& sPrefix) const
{
    return sPrefix + sMsg_ + sGetSubItems(TEXT("Specified "));
}

/**
 * @details
 */
const tstring cVectorScope::sGetSubItems(const tstring& sLeading) const
{
    tstrvector::const_iterator iter = iBegin_;
    tostringstream tos;
    tstrvector::size_type i;
    for (i = 0; i < iNumItems_; ++i)
    {
        if (i == 0)
        {
            tos << (sLeading + TEXT("["));
        }
        else
        {
            tos << TEXT(", ");
        }
        tos << *iter++;
    }
    if (i > 0)
    {
        tos << TEXT("].");
    }
    return tos.str();
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
