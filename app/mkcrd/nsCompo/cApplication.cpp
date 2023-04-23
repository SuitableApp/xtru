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
 * @file cApplication.cpp
 * @brief Application information management
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
cApplication::cApplication()
{
    // Acquire the place where the program is located
    TCHAR pFilename[_MAX_PATH];
#if __sun
    ::memset(pFilename, 0, _MAX_PATH);
    ::strncpy(pFilename, ::getexecname(), _MAX_PATH);
#else
    const DWORD iRc = readlink(PROC_PATH, pFilename, sizeof(pFilename)-1);
    Assert(iRc != -1u,  cApplicationError("readlink returns a error."));
#endif
    tstring pDir;
    tstring pFname;
    tstring pExt;
    tstring pTmp;
    tstring sPath(pFilename);
    pDir = sPath.substr(0, sPath.find_last_of('/')).c_str();
    pTmp = sPath.substr(sPath.find_last_of('/') + 1).c_str();
    if (pTmp.find_last_of('.') != tstring::npos)
    {
        pExt = pExt.substr(pExt.find_last_of('.') + 1).c_str();
    }
    if (pExt.size())
    {
        pFname = pTmp.substr(0, pTmp.size() - pExt.size() - 1);
    }
    sDrive_.insert(0, "");
    sDir_.insert(0, pDir.c_str());
    sFname_.insert(0, pFname.c_str());
    sExt_.insert(0, pExt.c_str());
    sExeLoc_ = sDrive_ + sDir_;
    
    // Store the current directory
    TCHAR pInitDir[_MAX_PATH + 1];
    getcwd(pInitDir,sizeof(pInitDir));
    DWORD iChars = std::strlen(pInitDir);

    Assert(iChars != 0, cApplicationError("Error - Faild to invoke ::getcwd()."));
    sInitDir_.append(pInitDir, iChars);
}

/**
 * @details
 */
cApplication::~cApplication()
{}

/**
 * @details
 */
const tstring cApplication::sGetFname()
{
    return sFname_;
}

/**
 * @details
 */
const tstring cApplication::sGetFullName()
{
    return sFname_ + sExt_;
}

/**
 * @details
 */
const tstring cApplication::sGetExeLoc()
{
    return sExeLoc_;
}

/**
 * @details
 */
const tstring cApplication::sGetFullPath()
{
    return sGetExeLoc() + sGetFullName();
}

/**
 * @details
 */
const tstring  cApplication::sGetInitDir()
{
    return sInitDir_;
}
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

