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
 * @file cCryptOpenSSL.h
 * @brief Encode / decode strings using OpenSSL
 * @author S.Sasaki
 * @date 2017/01/10
 */
#ifndef CCRYPTOPENSSL_H
#define CCRYPTOPENSSL_H
#include <memory>
#include "define.h"

namespace ps
{
namespace app
{
namespace mkcrd
{

/**
 * @class cCryptOpenSSLImpl
 * @brief 
 *  This class is an implementation class for OpenSSL encryption / decryption.
 */
class cCryptOpenSSLImpl;

/**
 * @class cCryptOpenSSL
 * @brief 
 *  This class controls encryption / decryption using OpenSSL.
 */
class cCryptOpenSSL
{
public:

    /**
     * @brief 
     *  Generate encryption key and initialize random number
     * @param[in] sCtx
     *  Key generation character string
     */
    cCryptOpenSSL(const tstring& sCtx);
    
    /**
     * @brief 
     *  Perform release processing of OpenSSL
     */
    ~cCryptOpenSSL();
    
    /**
     * @brief 
     *  Encode the specified string in AES 256 bit, CFB mode
     * @param[in] sOrg
     /  Original string
     * @return Encoded string
     */
    tstring sEncryptStr(const tstring& sOrg);
    
    /**
     * @brief 
     *  Decode the specified string in AES 256 bit, CFB mode
     * @param[in] sOrg
     *  Encoded character string
     * @return Decoded character string
     */
    tstring sDecryptStr(const tstring& sOrg);
private:
    std::unique_ptr<cCryptOpenSSLImpl> Impl;    ///< Implementation class
};
} // ps::app::mkcrd
} // ps::app
} // ps


#endif /* CCRYPTOPENSSH_L */
