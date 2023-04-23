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
 * @file cApplication.h
 * @brief Application information management
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CAPPLICATION_H
#define CAPPLICATION_H
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
 * @class cApplication
 * @brief 
 *  This class is application information management.
 */
class cApplication
{
public:
    /**
     * @brief 
     *  Generate application information.
     * @param[in] argc
     *  Number of arguments on the command line.
     * @param[in] argv
     *  Array of argument strings on the command line.
     * @param[in] envp
     *  Environment variable.
     * @return Application Information Class.
     */
    static cApplication* oMakeApplication(const int argc, const _TCHAR* argv[], const _TCHAR* envp[]);

    /**
     * @brief 
     *  Process of acquiring application information
     */
    cApplication();

    /**
     * @brief 
     */
    virtual ~cApplication();
    virtual void iMainService() = 0;  //
protected:
    /**
     * @brief 
     *  Acquire the file name of the program
     * @return file name of the program
     */
    const tstring sGetFname();
    
    /**
     * @brief 
     *  Retrieve the file name and extension of the program
     * @return file name and extension
     */
    const tstring sGetFullName();

    /**
     * @brief 
     *  Acquire the location of the program in full path
     * @return program location
     */
    const tstring sGetExeLoc();

    /**
     * @brief 
     *  Acquire program name as full path
     * @return program name (full path)
     */
    const tstring sGetFullPath();

    /**
     * @brief 
     *  Acquire the current directory
     * @return current directory
     */
    const tstring sGetInitDir();
private:
    tstring sDrive_;
    tstring sDir_;
    tstring sFname_;
    tstring sExt_;
    tstring sExeLoc_;
    tstring sInitDir_;
};
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CAPPLICATION_H */
