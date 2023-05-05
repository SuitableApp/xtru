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
 * @file nsMkcrd.h
 * @brief Encryption / decryption control class.
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef NSLOLCAL_H
#define NSLOLCAL_H
#include "define.h"

namespace ps
{
namespace app
{
namespace mkcrd
{

/**
 * @class cAppLocal
 * @brief 
 *  This class controls encryption / decryption.
 */
class cAppLocal
    : public nsCompo::cApplication
    , private nsCompo::cArgUtils            ///< Command argument utility
    , private nsCompo::SIMPLE_STATS         ///< Statistics
{
public:
    /**
     * @brief 
     *  Perform initial processing of the application
     * @param[in] argc
     *  Number of arguments on the command line.
     * @param[in] argv
     *  Array of argument strings on the command line.
     * @param[in] envp
     *  Environment variable.
     */
    explicit cAppLocal(const int argc, const _TCHAR* argv[], const _TCHAR* envp[]);

    /**
     * @brief 
     *  End the measurement of the required time and display it on the screen.
     */
    virtual ~cAppLocal();

    /**
     * @brief 
     *  Central processing
     */
    virtual void iMainService();
private:
    /**
     * @brief 
     *  Display usage on screen.
     * @return String representing specification description.
     */
    virtual const tstring sShowUsage() const;

    /**
     * @brief 
     *  Analyze and store command parameters.
     */
    virtual void vScanParams();

    /**
     * @brief 
     *  Split the specified user name and host name.
     * @param[in] sUsers
     *  The specified character string.
     * @param[out] sUser
     *  User name character string.
     * @param[out] sPass
     *  Password character string.
     * @param[out] sHost
     *  Host name character string.
     */
    virtual void vSplitUser( const tstring& sUsers, tstring& sUser, tstring& sPass, tstring& sHost );
    tstring sStatId_;
    tstrmap oTableOfReplacement_;               ///< Replacement string table
    int iMode_;

    /** 
     * The following data members substitute values with vScanParams() and use them in iMainService().
     */
    tstring sCredFile_;                         ///< Credential file location
    tstring sConnStr_;                          ///< DB connection string
};
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* NSLOLCAL_H */
