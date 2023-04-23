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
 * @file cStatCounter.h
 * @brief OS statistics counter
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CSTATCOUNTER_H
#define CSTATCOUNTER_H

#include <iostream>
#include <iomanip>
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
 * @class cSimpleStats
 * @brief
 *  A simple implementation that counts only elapsed time
 */
class cSimpleStats
{
private:
    static struct timeval tLiFreq_;
    int iCount_;                            ///< Number of passes
    struct timeval tLiPrevTick_;            ///< Last tick
    double fLiTotalTick_;                   ///< Total sum

    /**
     * @brief
     *  Acquire the system time
     * @return Structure containing seconds and microseconds
     */
    const struct timeval tLiGetPerfCounter() const;
    
    /**
     * @brief
     *  Acquisition of elapsed time (sec)
     * @return elapsed time(sec)
     */
    const double fGetSecond() const
    {
        return (double)fLiTotalTick_ / (double)1000000.0;
    }
    
    /**
     * @brief
     */
    struct tSingleton
    {
        tSingleton()
        {
            gettimeofday(&tLiFreq_, nullptr);
        }
    };
public:
    friend struct tSingleton;               ///< Show tLiFreq_ to tSingleton

    /**
     * @brief
     *  Initialization of statistical information
     */
    cSimpleStats();
    
    /**
     * @brief
     *  Initialization of statistical information
     * @param[in] oRhs 
     */
    cSimpleStats(const cSimpleStats& oRhs)
        : iCount_(oRhs.iCount_)
    {
        tLiPrevTick_ = oRhs.tLiPrevTick_;
        fLiTotalTick_ = oRhs.fLiTotalTick_;
    }
    
    /**
     * @brief
     *  Operator operation
     * @param[in] oRhs 
     * @return A pointer to itself
     */
    cSimpleStats& operator=(const cSimpleStats& oRhs)
    {
        iCount_ = oRhs.iCount_;
        tLiPrevTick_ = oRhs.tLiPrevTick_;
        fLiTotalTick_ = oRhs.fLiTotalTick_;
        return *this;
    }
    
    /**
     * @brief
     *  Format the measured time and output it to the specified stream
     * @param[in] os Output stream
     * @param[in] oRhs 
     * @return Output stream
     */
    friend tostream& operator<<(tostream& os, const cSimpleStats& oRhs)
    {
        return os 
            << TEXT("took ")
            << oRhs.fGetSecond()
            << TEXT(" second(s) and repeat ")
            << oRhs.iCount_ << TEXT(" time(s).\n")
        ;
    }
    
    /**
     * @brief
     *  Start measurement
     */
    void vStartElapse()
    {
        tLiPrevTick_ = tLiGetPerfCounter();
    }
    
    /**
     * @brief
     *  End measurement
     */
    void vStopElapse()
    {
        struct timeval tLiCurTick = tLiGetPerfCounter();
        fLiTotalTick_ += (tLiCurTick.tv_sec - tLiPrevTick_.tv_sec)*1000000+(tLiCurTick.tv_usec - tLiPrevTick_.tv_usec);
        tLiPrevTick_ = tLiCurTick;
        ++iCount_;
    }
    
    /**
     * @brief
     *  Measure the time and return the result
     * @return Measured time (sec)
     */
    const double fGetElapsed()
    {
        vStopElapse();
        vStartElapse();
        return fGetSecond();
    }
};

/**
 * @class cStatCounter
 * @brief 
 *  This class is OS statistics counter.
 */
template
<
    class tKeyType      ///< The data type of the key that distinguishes each statistic snapshot
    , class tValType    ///< Data type of statistic snapshot
>
class cStatCounter
{
protected:
    /**
     * @brief 
     *  Perform initialization of time measurement
     * @param[in] iPrecision
     *  Number of decimal places to output
     * @param[in] bShowTotal
     *  Whether all measured time is displayed at the end
     */
    cStatCounter(const int& iPrecision = 1, const bool& bShowTotal = true)
        : iPrecision_(iPrecision), iShowTotal_(bShowTotal)
    {
        tcout << std::setprecision(iPrecision_);        // Number of decimal places of floating point number
    }
    
    /**
     * @brief 
     *  Perform initialization of time measurement
     * @param[in] oRhs 
     */
    cStatCounter(const cStatCounter& oRhs)
        : oMap_(oRhs.oMap_), iPrecision_(oRhs.iPrecision_)
    {}
    
    /**
     * @brief
     * @param[in] oRhs 
     * @return Your own class
     */
    cStatCounter& operator=(const cStatCounter& oRhs)
    {
        if (this == &oRhs)
        {
            return *this;
        }
        oMap_ = oRhs.oMap_;
        return *this;
    }
    
    /**
     * @brief
     *  At the time of termination, the processing time is output on the screen as necessary                 */
    ~cStatCounter();
    
    /**
     * @brief
     *  Start measurement
     * @param[in] Tag Classification tag
     */
    void vStartElapse(const tKeyType& Tag)
    {
        oMap_[Tag].vStartElapse();
    }
    
    /**
     * @brief
     *  End measurement
     * @param[in] Tag Classification tag
     */
    void vStopElapse(const tKeyType& Tag)
    {
        oMap_[Tag].vStopElapse();
    }
    
    /**
     * @brief
     *  Obtain measurement value of specified tag
     * @param[in] Tag Classification tag
     * @return Measured value
     */
    tValType& operator[](const tKeyType& Tag)
    {
        return oMap_[Tag];
    }
private:
    typedef std::pair<tKeyType, tValType> tPairType_;
    std::map<tKeyType, tValType> oMap_;
    int iPrecision_;                                ///< Displayed digits after the decimal point
    bool iShowTotal_;                               ///< true: True: Display all maps before demolition
};

/**
 * @details
 */
template
<
    class tKeyType 
    , class tValType 
>
cStatCounter<tKeyType, tValType>::~cStatCounter()
{
    typename std::map<tKeyType, tValType>::const_iterator ci;
    if (iShowTotal_)
    {
        tcout << std::setprecision(iPrecision_);        // Displayed digits after the decimal point
        for (ci = oMap_.begin(); ci != oMap_.end(); ++ci)
        {
            tcout << ci->first << TEXT(" ") << ci->second;
        }
    }
}


typedef cStatCounter<tstring, cSimpleStats> SIMPLE_STATS;

} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CSTATCOUNTER_H */
