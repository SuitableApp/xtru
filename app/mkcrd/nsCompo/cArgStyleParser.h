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
 * @file cArgStyleParser.h
 * @brief Command Argument Analyzer Protocol
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CANYARGSTYLE_H
#define CANYARGSTYLE_H

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace nsCompo
{

/**
 * @class cArgStyleParser
 * @brief 
 *  This class is command argument analyzer protocol.
 */
class cArgStyleParser
{
public:
    /**
     * @brief
     *  Factory of objects supporting Microsoft-style syntax<br/>
     *  Switch and switch arguments are separated by blank characters<br/>
     * @return
     */
    static cArgStyleParser* oMakeMsArgStyle() throw();
    
    /**
     * @brief
     *  Factory of objects supporting GNU resource-like syntax<br/>
     *  Between the switch and the switch argument is split by ':' or '=' character<br/>
     * @return
     */
    static cArgStyleParser* oMakeGnuArgStyle() throw();
    
    /**
     * @brief
     *  Convert to standard so that common implementation can be used<br/>
     *  For this class,Meaning Microsoft style<br/>
     * @param[in] oSynonyms
     *  A collection of valid parameters.
     * @param[in,out] oArgs
     *  A collection of specified parameters.
     */
    virtual void vNormalize(const tstrset& oSynonyms, tstrvector& oArgs) = 0;
    
    /**
     * @brief
     */
    virtual ~cArgStyleParser()
    {}
};
} // ps::app::mkcrd::nsCompo
} // ps::app::mkcrd
} // ps::app
} // ps

#endif /* CANYARGSTYLE_H */
