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
 * @file cEnvStrings.h
 * @brief Control environment variables
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CENVSTRINGS_H
#define CENVSTRINGS_H

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

class cEnvStringsImpl;

/**
 * @class cEnvStrings
 * @brief 
 *  This class controls environment variables.
 */
class cEnvStrings
{
public:
    /**
     * @brief
     *  Perform initialization
     */
    explicit cEnvStrings();

    /**
     * @brief
     */
    ~cEnvStrings();

    /**
     * @brief 
     *  Perform initial setting of input processing.
     * @param[out] oMap
     *  Map containing environment variables.
     * @param[in] envp
     *  Environment variable.
     */
    void vGetTable(tstrmap& oMap, const char* envp[]);
private:
    /**
     * @brief
     *  Implementation class
     */
    std::unique_ptr<cEnvStringsImpl> Impl_;
};
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CENVSTRINGS_H */

