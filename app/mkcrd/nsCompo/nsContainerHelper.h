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
 * @file nsContainerHelper.h
 * @brief Helper function of the STL container
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef NSLISTCONTAINER_H
#define NSLISTCONTAINER_H

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @brief
 *  Insert the list-like data into the container<br/>
 *  You can specify the primitive specified by the third argument as a delimiter<br/>
 * @param[out]
 *  oContainer Target container
 * @param[in]
 *  oList A set of synonyms delimited by delimiter characters
 * @param[in]
 *  cSeparater Delimiter string
 */
template <class C> 
void vDelimitedStringToContainer(
    C& oContainer
    , const typename C::value_type& oList
    , const typename C::value_type::allocator_type::value_type& cSeparater
    )
{
    typename C::size_type p0 = 0, p1;
    typename C::value_type oList_(oList);
    oList_.push_back(cSeparater);
    while ((p1 = oList_.find(cSeparater, p0)) != C::value_type::npos)
    {
        oContainer.insert(oList_.substr(p0, p1 - p0));
        p0 = p1 + 1;
    }
}

/**
 * @brief
 *  Check whether the target item exists in the specified container
 * @param[in] oContainer
 *  Target container
 * @param[in] sItem
 *  Items to be checked
 * @return true:Target exists
 */
template <class C>
const bool iDoesExists(
    const C& oContainer
    , const typename C::key_type& sItem
    ) 
{
    return oContainer.find( sItem ) != oContainer.end();
}

/**
 * @brief
 *  "S type" is converted and taken out as the value of "D type"<br/>
 *  "D type" condition<br/>
 *   (1). Defining an input operator (operator >>)<br/>
 *   (2). Defining a default constructor<br/>
 * @param[in] oSource original
 * @param[out] oDestination After conversion
 * @param[in] oSeparator Delimiter string
 */
template <class S, class D>
void vScanValue(const S& oSource, D& oDestination, const D& oSeparator)
{
    tistringstream iss(oSource);
    D oBuff;
    D oConcatinated;
    int i = 0;
    while (iss >> oBuff)
    {
        if (i)
        {
            oConcatinated += oSeparator;
        }
        oConcatinated += oBuff;
        ++i;
    }
    oDestination = oConcatinated;
}

/**
 * @brief
 *  Split the specified value with a separator
 * @param[in] oSource original
 * @param[out] oDestination After conversion
 */
template <class S, class D>
void vScanValue(const S& oSource, D& oDestination)
{
    tistringstream iss(oSource);
    iss >> oDestination;
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* NSLISTCONTAINER_H */
