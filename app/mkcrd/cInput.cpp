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
 * @file cInput.cpp
 * @brief Perform connection character string input processing.
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include <iostream>
#include <memory>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include "cInput.h"

#ifdef __sun
#define __cfmakeraw(tio) { \
    tio.c_iflag &= ~(IMAXBEL|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON); \
    tio.c_oflag &= ~OPOST; \
    tio.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN); \
    tio.c_cflag &= ~(CSIZE|PARENB); \
    tio.c_cflag |= CS8; \
}
#else
#define __cfmakeraw(tio) cfmakeraw(&tio)
#endif

namespace ps
{
namespace app
{
namespace mkcrd
{
/**
 * @details
 */
cInput::cInput(const tstring sUser, const tstring sPass, const tstring sHost, const int iMode)
{
    if (!sUser.empty())
    {
        sUser_ = sUser;
    }
    if (!sPass.empty())
    {
        sPassword_ = sPass;
    }
    if (!sHost.empty())
    {
        sHost_ = sHost;
    }
    iMode_ = iMode;
}

/**
 * @details
 */
cInput::~cInput()
{
}

/**
 * @details
 */
void cInput::inputRequest()
{
    // Connection mode input processing
    if (sUser_.empty() || sPassword_.empty() || sHost_.empty())
    {
        vSelectMode();
    }
    
    // User name input processing
    if (sUser_.empty())
    {
        vInputUser();
    }
    else
    {
        if (sUser_.length() > iMaxUserName_)
        {
            tcout << "it is over the number of characters." << std::endl;
            vInputUser();
        }
    }
    
    // Password input processing
    if (sPassword_.empty() || sPassword_ == "*" )
    {
        // Perform input processing when password is empty or half-size "*" is specified.
        vInputPassword();
    }
    else
    {
        if (sPassword_.length() > iMaxPassWord_)
        {
            tcout << "it is over the number of characters." << std::endl;
            vInputPassword();
        }
    }
    
    // Host name input processing
    if (sHost_.empty())
    {
        vInputHost();
    }
    else
    {
        if (sHost_.length() > iMaxDbAlias_)
        {
            tcout << "it is over the number of characters." << std::endl;
            vInputHost();
        }
    }
}

/**
 * @details
 */
tstring cInput::sGetInputString()
{
    tstring sConnStr(sUser_ + "/" + sPassword_);
    if (sHost_.length() > 0)
    {
        sConnStr += "@" + sHost_;
    }
    if (iMode_ == tSesMode::iSysDba)
    {
        sConnStr += " AS SYSDBA";
    }
    else if (iMode_ == tSesMode::iSysOper)
    {
        sConnStr += " AS SYSOPER";
    }
    else
    {
        // do nothing
    }
    return sConnStr;
}

/**
 * @details
 */
int cInput::iGetInputMode()
{
    return iMode_;
}

/**
 * @details
 */
void cInput::vInputUser()
{
    tstring sStr;

    tcout << TEXT("please enter user name.") << std::endl;
    while (1)
    {
        tcout << TEXT("user name:");
        sStr = pGetInputStr();
        if (sStr.length() > iMaxUserName_)              // Number of characters exceeded.
        {
            tcout << "it is over the number of characters. please re-enter." << std::endl;
            continue;
        }
        if (sStr.length() == 0 && iMode_ == tSesMode::iDefault) // When connection mode is normal, no input is not possible.
        {
            tcout << "for normal connection , user name is required." << std::endl;
            continue;
        }
        break;
    }
    sUser_ = sStr;
}

/**
 * @details
 */
void cInput::vInputPassword()
{
    tstring sStr1;
    tstring sStr2;

    while (1)
    {
        tcout << TEXT("please enter password.") << std::endl;
        while (1)
        {
            tcout << TEXT("password:");
            sStr1 = pGetInputStr(false);
            if (sStr1.length() > iMaxPassWord_)             // Number of characters exceeded.
            {
                tcout << "it is over the number of characters. please re-enter." << std::endl;
                continue;
            }
            if (sStr1.length() == 0 && iMode_ == tSesMode::iDefault)    // When connection mode is normal, no input is not possible.
            {
                tcout << "for normal connection , password is required." << std::endl;
                continue;
            }
            break;
        }
        
        // Confirmation input processing.
        tcout << TEXT("please re-enter password.") << std::endl;
        while (1)
        {
            tcout << TEXT("password:");
            sStr2 = pGetInputStr(false);
            break;
        }
        if (sStr1 != sStr2)
        {
            // If they do not match, re-input processing.
            tcout << "password is incorrect. please re-enter." << std::endl;
            continue;
        }
        break;
    }
    sPassword_ = sStr1;
}

/**
 * @details
 */
void cInput::vInputHost()
{
    tstring sStr;

    tcout << TEXT("please enter host name.") << std::endl;
    while (1)
    {
        tcout << TEXT("host name:");
        sStr = pGetInputStr();
        if (sStr.length() > iMaxDbAlias_)       // Number of characters exceeded.
        {
            tcout << "it is over the number of characters. please re-enter." << std::endl;
            continue;
        }
        break;
    }
    sHost_ = sStr;
}

/**
 * @details
 */
void cInput::vSelectMode()
{
    char c;
    
    // Acquire and save the terminal setting (cooked mode) in the initial state.
    tcgetattr(STDIN_FILENO, &tCookedTermIos_);
    
    // Create and save raw mode terminal settings.
    tRawTermIos_ = tCookedTermIos_;
    __cfmakeraw(tRawTermIos_);
    tRawTermIos_.c_iflag |= BRKINT;
    
    // Set the terminal to raw mode.
    tcsetattr(STDIN_FILENO, 0, &tRawTermIos_);

    tcout << TEXT("please select connection mode.") << "\r" << std::endl;
    if (iMode_ != tSesMode::iDefault)
    {
        tcout << TEXT("normal(N), sysdba(A), sysoper(Q):");
        if (iMode_ == tSesMode::iSysDba)
        {
            tcout << "A" << std::endl;
        }
        else
        {
            tcout << "Q" << std::endl;
        }
    }
    else
    {
        while (1)
        {
            tcout << TEXT("normal(N), sysdba(A), sysoper(Q):");
            tcin >> c;
            tcout << c;

            switch (c)
            {
            case 'N':
            case 'n':
                iMode_ = tSesMode::iDefault;
                break;
            case 'A':
            case 'a':
                iMode_ = tSesMode::iSysDba;
                break;
            case 'Q':
            case 'q':
                iMode_ = tSesMode::iSysOper;
                break;
            case 0x03: // CTRL+C
                // Restore terminal settings.
                tcsetattr(STDIN_FILENO, 0, &tCookedTermIos_);
                tcout << std::endl;
                raise(SIGINT);
                break;
            default:
                tcout << "\r" << std::endl << "input chatacter is invalid." << "\r" << std::endl;
                continue;
            }
            break;
        }
    }
    // Restore terminal settings.
    tcsetattr(STDIN_FILENO, 0, &tCookedTermIos_);
    tcout << std::endl;
}

/**
 * @details
 */
const char* cInput::pGetInputStr(bool iDsp)
{
    char c;
    tstring sStr;
    
    // Acquire and save the terminal setting (cooked mode) in the initial state.
    tcgetattr(STDIN_FILENO, &tCookedTermIos_);
    
    // Create and save raw mode terminal settings.
    tRawTermIos_ = tCookedTermIos_;
    __cfmakeraw(tRawTermIos_);
    tRawTermIos_.c_iflag |= BRKINT;
    
    // Set the terminal to raw mode.
    tcsetattr(STDIN_FILENO, 0, &tRawTermIos_);
    while( 1 )
    {
        c = getchar();
        if (c == 0x0a || c == 0xd)
        {
            break;
        }
        else
        {
            if (c == 0x03)      // CTRL+C
            {
                // Restore terminal settings.
                tcsetattr(STDIN_FILENO, 0, &tCookedTermIos_);
                tcout << std::endl;
                raise(SIGINT);
                break;
            }
            else
            {
                sStr.push_back(c);
            }
        }
        if( iDsp )
        {
            tcout << c;
        }
        else
        {
            tcout << "*";
        }
    }
    
    // Restore terminal settings.
    tcsetattr(STDIN_FILENO, 0, &tCookedTermIos_);
    tcout << std::endl;
    return sStr.c_str();
}
} // ps::app::mkcrd
} // ps::app
} // ps
