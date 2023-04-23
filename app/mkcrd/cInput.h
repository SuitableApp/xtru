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
 * @file cInput.h
 * @brief Perform connection character string input processing.
 * @author S.Sasaki
 * @date 2017/01/10
 */

#ifndef CINPUT_H
#define CINPUT_H
#include <string>
#include <termios.h>
#include "define.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsOracle
{
    /**
     * @enum tLmits
     *   Define Oracle specific restrictions.
     */
    enum tLmits
    {
        ///< The length of the name of the schema object.
        OBJECT_NAME_LEN = 30
        ///< Length of database alias
        , MAX_DBALIAS_LEN = 128
    };
}

/**
 * @brief
 *  Connection mode
 * @enum tSesMode
 */
enum tSesMode
{
    ///< Normal mode
    iDefault = 0
    ///< Sysdba mode
    , iSysDba = 2
    ///< Sysoper mode
    , iSysOper = 4
};

/**
 * @class cInput
 * @brief 
 *  This class controls input from the console.
 */
class cInput
{
private:
    tstring sUser_;                 ///< User name entered
    tstring sHost_;                 ///< Host name entered
    tstring sPassword_;             ///< Password entered
    int iMode_;                     ///< Connection mode entered
    
    struct termios tCookedTermIos_; ///< Global variable for storing terminal settings. cooked mode.
    
    struct termios tRawTermIos_;    ///< Global variable for storing terminal settings. raw mode.

    /**
     * @brief
     *  User name input processing
     */
    void vInputUser();

    /**
     * @brief
     *  Password input processing.
     */
    void vInputPassword();

    /**
     * @brief
     *  Host name input processing
     */
    void vInputHost();

    /**
     * @brief
     *  Connection mode input processing
     */
    void vSelectMode();

    /*/
     * @brief
     *  Request input of character string. Acquire in character units, combine the results and return.
     * @param[in] iDsp
     *  Echo back input characters. When it is false, it outputs [*].
     * @return The input character string
     */
    const char* pGetInputStr(bool iDsp=true);
    
    /**
     * @enum  tPlatform
     *   Maximum size of each item
     */
    enum tPlatform
    {
        ///< Credential File Name Length.
        iMaxCredFile_ = _MAX_PATH-1
        ///< User name character length.
        , iMaxUserName_ = nsOracle::OBJECT_NAME_LEN-1
        ///< Password character length.
        , iMaxPassWord_ = nsOracle::OBJECT_NAME_LEN-1
        ///< Host name character length
        , iMaxDbAlias_ = nsOracle::MAX_DBALIAS_LEN-1
    };
    
public:
    /**
     * @brief
     *  Perform initial setting of input processing.
     * @param[in] sUser
     *    User name character string
     * @param[in] sPass
     *    User pass character string
     * @param[in] sHost
     *    Host name character string
     * @param[in] iMode
     *    Connection mode. Select one in the tSesMode.
     */
    cInput(const tstring sUser, const tstring sPass, const tstring sHost, const int iMode);

    /**
     * @brief
     */
    ~cInput();

    /**
     * @brief
     *  Request input of various information from standard input.
     */
    void inputRequest();

    /**
     * @brief
     *  Acquisition of input information
     * @return It returns in "[user name]/[password]@[host name]" format.
     */
    tstring sGetInputString();
    
    /**
     * @brief
     *  Acquire selected connection information
     * @return Connection mode.
     */
    int iGetInputMode();
};
} // ps::app::mkcrd
} // ps::app
} // ps
#endif /* CINPUT_L */
