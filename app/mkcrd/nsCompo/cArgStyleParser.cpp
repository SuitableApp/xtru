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
 * @file cArgStyleParser.cpp
 * @brief Command Argument Analyzer Protocol
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
 * @brief
 *  This class checks whether the specified set contains the target data.
 */
template <class T>
class cFindPart
{
public:
    /**
     * @brief
     *  Search data setting.
     * @param[in] oPartStr
     *  Search target data.
     */
    cFindPart(const T& oPartStr) throw();
    
    /**
     * @brief
     *  Check if the corresponding data exists.
     * @param[in] oAllStr
     *  Aggregation of search source data.
     * @return true: A substring was found
     */
    int operator()(const T& oAllStr) const throw();
private:
    T oPartStr_; // Substring to be searched
};

/**
 * @details
 */
template <class T>
cFindPart<T>::cFindPart(const T& oPartStr) throw()
    : oPartStr_(oPartStr)
{}

/**
 * @details
 */
template <class T>
int cFindPart<T>::operator()(const T& oAllStr) const throw()
{
    typename T::size_type iPos = oAllStr.find(oPartStr_);
    return iPos != tstring::npos; // true: A substring was found
}

/**
 * @class cMsArgStyle
 * @brief 
 *  This class is factory of objects supporting Microsoft-style syntax.
 */
class cMsArgStyle : public cArgStyleParser
{
public:
    /**
     * @brief
     *  Convert to standard so that common implementation can be used<br/>
     * @param[in] oSynonyms
     *  A collection of valid parameters.
     * @param[in,out] oArgs
     *  A collection of specified parameters.
     */
    virtual void vNormalize(const tstrset& oSynonyms, tstrvector& oArgs) throw();
private:
    tstrset oSynonyms_;
    tstrvector oArgs_;
};

/**
 * @class cGnuArgStyle
 * @brief 
 *  This class is factory of objects supporting GNU resource-like syntax.
 */
class cGnuArgStyle : public cArgStyleParser
{
public:
    /**
     * @brief
     *  Convert to standard so that common implementation can be used<br/>
     * @param[in] oSynonyms
     *  A collection of valid parameters.
     * @param[in,out] oArgs
     *  A collection of specified parameters.
     */
    virtual void vNormalize(const tstrset& oSynonyms, tstrvector& oArgs) throw();
};

/**
 * @details
 */
void cMsArgStyle::vNormalize(const tstrset& oSynonyms, tstrvector& oArgs) throw()
{
    oSynonyms_ = oSynonyms;
    oArgs_ = oArgs;
}

/**
 * @details
 */
void cGnuArgStyle::vNormalize(const tstrset& oSynonyms, tstrvector& oArgs) throw()
{
    for (tstrset::const_iterator r = oSynonyms.begin(); r != oSynonyms.end(); ++r)
    {
        // For example, we assume that oArgs contains elements such as "-a = / a:" and want to match "-a" and "/a" included in oSynonyms.
        tstrvector::iterator i = find_if(
            oArgs.begin(), oArgs.end()
            , cFindPart<tstrset::const_iterator::value_type>(*r)
            );
        if (i != oArgs.end())
        {
            // Split what i is pointing into a part common to *r and a non-common part
            i = oArgs.insert(i, *r);
            i[1].erase(0, r->size());
            break;
        }
    }
}

/**
 * @details
 */
cArgStyleParser* cArgStyleParser::oMakeMsArgStyle() throw()
{
    return new cMsArgStyle;
}

/**
 * @details
 */
cArgStyleParser* cArgStyleParser::oMakeGnuArgStyle() throw()
{
    return new cGnuArgStyle;
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
