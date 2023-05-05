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
 * @file cArgUtils.cpp
 * @brief Analyze the specified parameter
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
 * @class cArgUtils
 * @brief 
 *  This class is an implementation class that parses the specified parameters.
 */
class cArgUtilsImpl
{
public:
    /**
     * @brief 
     *  Parameter analysis initial processing
     * @param[in] iArgc
     *  Number of parameters
     * @param[in] szArgv
     *  parameters
     * @param[in] oAnyArgStyle
     *  Parameter analysis parser
     */
    cArgUtilsImpl(
        const int iArgc
        , const _TCHAR **szArgv
        , cArgStyleParser* oAnyArgStyle
    )
        : iArgc_(iArgc), oAnyArgStyle_(oAnyArgStyle)
    {
        for (int i = 0; i < iArgc ; i++)
        {
            if (i == 0)
            {
                sOperator_ = szArgv[i];
            }
            else
            {
                oOperands_.push_back(szArgv[i]);
            }
        }
    }
    
    /**
     * @brief 
     */
    ~cArgUtilsImpl()
    {}
    
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
    ) ;
    
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
    tstrvector::const_iterator begin() const { return oOperands_.begin(); }
    tstrvector::const_iterator end() const { return oOperands_.end(); }
    tstrvector::size_type size() const { return oOperands_.size(); }
    const bool empty() const { return oOperands_.empty(); }
    
    /**
     * @brief 
     *  Get the number of runtime arguments
     * @return Number of run-time arguments (== argc)
     */
    const int iGetArgc() const { return iArgc_; }
private:
    const int iArgc_;
    std::unique_ptr<cArgStyleParser> oAnyArgStyle_;
    tstring sOperator_;         ///< Program name
    tstrvector oOperands_;      ///< Immediately after construction, all program arguments (switches, their arguments, files, directories, etc.)
                                ///< are delivered. However, after executing "iSearchOpt", the switches and their arguments remain.
    std::map<int, tstrvector> oExclusiveSwitches_;
                                ///< Data to examine the usage status of switches that must be used exclusively
    std::map<tstring, cVectorScope> oAvailableSwitches_;
    tstrvector oSwtches_;       ///< Valid switches and their arguments. Effective elements are transferred from "oOperands_" each time "iSearchOpt" is executed
    
    /**
     * @brief 
     */
    cArgUtilsImpl(const cArgUtilsImpl&);

    /**
     * @brief 
     */
    cArgUtilsImpl& operator=(const cArgUtilsImpl&);
};

/**
 * @details
 */
const bool cArgUtilsImpl::iSearchOpt(
    const tstring& sTagOfList
    , const int& iExclusiveGrp
    , const tstrvector::size_type iMinNumParams
    , const tstrvector::size_type iMaxNumParams
    , cSwitchUtils& oSu
    , cVectorScope& oSw
    ) 
{
    // Register a valid switch and its synonym
    auto oSynonyms = oSu.oGetSwitches(sTagOfList);   // A set of switches having the same meaning
    
    // Separate switches and switch arguments
    if (iMinNumParams > 0)
    {
        oAnyArgStyle_->vNormalize(oSynonyms, oOperands_);
    }
    
    // Search whether the specified switch exists in the argument (including synonym)
    for (auto r = oSynonyms.cbegin(); r != oSynonyms.cend(); ++r)
    {
        tstrvector::iterator i, j;
        i = j = find(oOperands_.begin(), oOperands_.end(), *r);
        if (j < oOperands_.end())
        {
            // Sort the searched switch by exclusion index and add it to tstrvector
            // If the number of elements of this tstrvector is not less than 2, it can be regarded as an exclusion control violation
            tstrvector& vect = oExclusiveSwitches_[iExclusiveGrp];
            vect.push_back(*i);
            Assert(vect.size() < 2, cVectorScope(vect.begin(), vect.size()
                , TEXT("Unable to use it at the same time. ")));
            for (;;)
            {
                ++j;
                tstrvector::size_type iNumItems = j - i;    // Number of arguments given to the switch
                
                // Repeat until reaching the end of the command statement, finding another switch,
                // or reaching the maximum number of arguments accepted by the current switch
                if (j == oOperands_.end() || oSu.iIsSwitch(j) || iNumItems >= iMaxNumParams + 1)
                {
                    // Check that it does not fall below the number of arguments allowed for the switch
                    Assert(iNumItems >= iMinNumParams + 1, cVectorScope(i, iNumItems
                        , TEXT("Too few parameters. ")));
                        
                    // Sort the switch part included in "oOperands_" and move it to the end of the current "oSwtches_"
                    oSwtches_.insert(oSwtches_.end(), i, j);
                    j = oOperands_.erase(i, j);
                    
                    // Set the switch and the iterator pointing to its argument to the return value
                    // Store return value with tag string in "oAvailableSwitches_" so that it can be referenced by another function of this class
                    oAvailableSwitches_.insert(
                        std::map<tstring, cVectorScope>::value_type(
                            sTagOfList                      // Tag character string for classification
                            , oSw = cVectorScope(oSwtches_.end() - iNumItems, iNumItems) 
                                                            // 'OSw' actually points to the argument specified for the command
                        )
                    );
                    return true;                            // The position of the place where the switch was found returns
                }
            }
        }
    }
    return false;                                           // 0 is returned if the switch is not found
}

/**
 * @details
 */
const bool cArgUtilsImpl::iDoesTagExist(
    const tstring& sTagOfList
) const
{
    return oAvailableSwitches_.find(sTagOfList) != oAvailableSwitches_.end();
}

/**
 * @details
 */
void cArgUtilsImpl::vSearchInvalidOpt(cSwitchUtils& oSu) const
{
	oSu.vSearchInvalidOpt(oOperands_);
}

/**
 * @details
 */
cArgUtils::cArgUtils(
    const int iArgc
    , const  _TCHAR ** szArgv
    , cArgStyleParser* oAnyArgStyle
)
    : Impl_(new cArgUtilsImpl(iArgc, szArgv, oAnyArgStyle))
{}

/**
 * @details
 */
cArgUtils::cArgUtils(cArgUtils& oRhs)
    : Impl_(move(oRhs.Impl_))
{}

/**
 * @details
 */
cArgUtils& cArgUtils::operator=(cArgUtils& oRhs)
{
    Impl_ = move(oRhs.Impl_);
    return *this;
}

/**
 * @details
 */
cArgUtils::~cArgUtils()
{}

/**
 * @details
 */
const bool cArgUtils::iSearchOpt(
    const tstring& sTagOfList
    , const int& iExclusiveGrp
    , const tstrvector::size_type iMinNumParams
    , const tstrvector::size_type iMaxNumParams
    , cSwitchUtils& oSu
    , cVectorScope& oSw
    )
{
    return Impl_->iSearchOpt(sTagOfList, iExclusiveGrp, iMinNumParams, iMaxNumParams, oSu, oSw);
}

/**
 * @details
 */
const bool cArgUtils::iDoesTagExist(
    const tstring& sTagOfList
) const
{
    return Impl_->iDoesTagExist(sTagOfList);
}

/**
 * @details
 */
void cArgUtils::vSearchInvalidOpt(cSwitchUtils& oSu) const
{
    Impl_->vSearchInvalidOpt(oSu);
}

/**
 * @details
 */
tstrvector::const_iterator cArgUtils::begin() const
{
    return Impl_->begin();
}

/**
 * @details
 */
tstrvector::const_iterator cArgUtils::end() const
{
    return Impl_->end();
}

/**
 * @details
 */
tstrvector::size_type cArgUtils::size() const
{
    return Impl_->size();
}

/**
 * @details
 */
const bool cArgUtils::empty() const
{
    return Impl_->empty();
}

/**
 * @details
 */
const int cArgUtils::iGetArgc() const
{
    return Impl_->iGetArgc();
}

} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps
