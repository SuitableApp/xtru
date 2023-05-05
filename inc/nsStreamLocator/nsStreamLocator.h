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
namespace lib
{
namespace nsStreamLocator
{

typedef enum _tExtType  {iExtData, iExtCtrl, iExtClob, iExtBlob, iNumExtType} tExtType;
typedef boost::array<std::string, iNumExtType> tExts;
typedef ps::lib::cMap<const std::string, const std::string> tEnvMap;

extern boost::filesystem::path sOutput_;   ///< Never be empty.
extern std::string sConnectTo_;
extern tExts oExts_;
extern tEnvMap oEnvMap_;
extern bool iSuppressCtrlf_;    ///< false means that the control file is outputted.

extern const boost::regex regLocationExpr;
extern const boost::regex regMacroSymbolExpr;

/**
 * @struct tInitParams
 * @brief
 * Initialization parameters.
 */
struct tInitParams
{
    std::string sOwner;              ///< @brief {I}
    std::string sTableName;          ///< @brief {T}
    std::string sPartitionName;      ///< @brief {P}
};

/**
 * @brief
 * A map that associates macro substitution processing
 * with a macro identifier.
 */
typedef std::function<std::string(const tInitParams&, const std::string&, const tExtType&, const std::string&) > tMacroAction;
extern const ps::lib::cMap<std::string, tMacroAction> oMacroMap_;

/**
 * @brief
 * A map that associates the construction logic of the stream
 * with the scheme.
 */
typedef std::function<std::ostream*(const std::string&) > tOstreamGenerator;
extern const ps::lib::cMap<std::string, tOstreamGenerator> oSchemeMap_;

/**
 * @brief
 * Checkng wheather initializing has done.
 */
bool iHasBeenInitialized(void);

/**
 * @brief
 * reset static member value.
 * @param [in] sStreamLocator
 * @param [in] iStdout
 *  -# 1st-bit: marked: a data-file stream is fowarded to sStreamLocator, cleard: default locator.
 *  -# 2nd-bit: marked: a ctrl-file stream is fowarded to sStreamLocator, cleard: default locator.
 *  -# 3rd-bit: marked: a lob-file stream is fowarded to sStreamLocator, cleard: default locator.
 *  -# more than 3rd-bit; Not used and reserved for the feuture extention.
 * @param [in] sOutput
 * @param [in] oExts
 * @param [in] iSuppressCtrlf
 */
extern void vInitialize(
    const std::string& sStreamLocator
    , const int32_t iStdout
    , const boost::filesystem::path sOutput
    , const tExts oExts
    , const bool iSuppressCtrlf
);

extern std::string sGetStreamLocator(const tExtType& iExtType);

} // ps::lib::nsStreamLocator

} // ps::lib

} // ps
