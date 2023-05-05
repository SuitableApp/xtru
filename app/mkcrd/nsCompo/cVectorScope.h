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
 * @file cVectorScope.h
 * @brief Class for making it possible to treat consecutive elements included in "std :: vector" as if seeing through the window
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include "../define.h"

#ifndef CVECTORSCOPE_H
#define CVECTORSCOPE_H

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @class cVectorScope
 * @brief 
 *  This class is a class for handling consecutive elements contained
 *  in "ps::lib::cVector" as if looking at the window.
 */
class cVectorScope
{
public:
    /**
     * @brief 
     */
    cVectorScope()
    {}
    
    /**
     * @brief 
     *  Perform initialization processing
     * @param[in] iBegin
     * @param[in] iNumItems
     * @param[in] sMsg
     */
    cVectorScope(
        const tstrvector::const_iterator iBegin
        , const tstrvector::size_type iNumItems
        , const tstring& sMsg = TEXT("")
        ) : iBegin_(iBegin), iNumItems_(iNumItems), sMsg_(sMsg)
    {}
    
    /**
     * @brief 
     *  Initialize with specified object
     * @param[in] oRhs
     *  Object used for initialization
     */
    cVectorScope(const cVectorScope& oRhs)
        : iBegin_(oRhs.iBegin_), iNumItems_(oRhs.iNumItems_), sMsg_(oRhs.sMsg_)
    {}
    
    /**
     * @brief 
     *  Initialize with specified object
     * @param[in] oRhs
     *  Object used for initialization
     * @return Your own class
     */
    cVectorScope& operator=(const cVectorScope& oRhs)
    {
        if (this == &oRhs)
        {
            return *this;
        }
        iBegin_ = oRhs.iBegin_;
        iNumItems_ = oRhs.iNumItems_;
        sMsg_ = oRhs.sMsg_;
        return *this;
    }
    
    /**
     * @brief 
     */
    virtual ~cVectorScope()
    {}
    
    /**
     * @brief 
     *  Format and retrieve the message stored in the specified prefix
     * @param[in] sPrefix
     *  Prefix for getting messages
     * @return Formatted message
     */
    virtual const tstring sShowMsg(const tstring& sPrefix) const;
    
    /*
     * @brief 
     *  Get number of peek window elements
     * @return Number of elements of peep window
     */
    virtual const tstrvector::size_type& iGetNumItems() const
    {
        return iNumItems_;
    }
    
    /**
     * @brief 
     *  Specify index and fetch command argument as character type
     * @param[in] i Index
     * @return command argument
     */
    virtual const tstring& operator[](const tstrvector::size_type i) const
    {
        return iBegin_[i];
    }
private:
    // Extract a specified number of argument elements from the specified position
    /**
     * @brief 
     *  Retrieve sub items
     * @param[in] sLeading Key items
     * @return String of subitem
     */
    virtual const tstring sGetSubItems(const tstring& sLeading) const;
    
    ///< Stores the position and the number of elements of the window in ps::lib::cVector
    tstrvector::const_iterator iBegin_;
    
    tstrvector::size_type iNumItems_;
    
    tstring sMsg_;
};
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CVECTORSCOPE_H */
