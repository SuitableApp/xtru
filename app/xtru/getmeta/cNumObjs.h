
/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
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

#pragma once

namespace ps
{

namespace app
{

namespace xtru
{

namespace getmeta
{

/*
 * @class cNumObjs
 * Store the number of schema objects.
 */
class cNumObjs
{
    friend std::ostream& operator<<(std::ostream& os, const cNumObjs& rhs);
public:
    cNumObjs()
    {
        iDlns_ = iSeqs_ = iType_ = iPckg_ = iFunc_ = iProc_ = iClus_ = iTabs_ = iChds_
        = iSnps_ = iRefs_ = iView_ = iTypb_ = iPkgb_ = iSyns_ = iJobs_ = 0;
    }
    /**
     * @param [in] iDlns database link
     * @param [in] iSeqs sequence
     * @param [in] iType types
     * @param [in] iPckg packages
     * @param [in] iFunc functions 
     * @param [in] iProc procedures
     * @param [in] iClus cluster
     * @param [in] iTabs table
     * @param [in] iChds table that included FK.
     * @param [in] iSnps materialized view
     * @param [in] iRefs refresh group
     * @param [in] iView view
     * @param [in] iTypb type bodies
     * @param [in] iPkgb package bodies
     * @param [in] iSyns synonym
     * @param [in] iJobs job queue
     */
    void vSetValues(
        const int32_t& iDlns
        , const int32_t& iSeqs
        , const int32_t& iType
        , const int32_t& iPckg
        , const int32_t& iFunc
        , const int32_t& iProc
        , const int32_t& iClus
        , const int32_t& iTabs
        , const int32_t& iChds
        , const int32_t& iSnps
        , const int32_t& iRefs
        , const int32_t& iView
        , const int32_t& iTypb
        , const int32_t& iPkgb
        , const int32_t& iSyns
        , const int32_t& iJobs
    );
private:
    std::ostream& vRptBanner(std::ostream& os) const;
    int32_t iDlns_ ///< database link
          , iSeqs_ ///< sequence
          , iType_ ///< types
          , iPckg_ ///< packages
          , iFunc_ ///< functions
          , iProc_ ///< procedures
          , iClus_ ///< cluster
          , iTabs_ ///< table
          , iChds_ ///< table that included FK.
          , iSnps_ ///< materialized view
          , iRefs_ ///< refresh group
          , iView_ ///< view
          , iTypb_ ///< type bodies
          , iPkgb_ ///< package bodies
          , iSyns_ ///< synonym
          , iJobs_ ///< job queue
    ;
};

} // ps::app::xtru::getmeta

} // ps::app::xtru

} // ps::app

} // ps

