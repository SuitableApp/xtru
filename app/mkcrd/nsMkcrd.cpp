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
 * @file nsMkcrd.cpp
 * @brief Encryption / decryption control class.
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include <time.h>
#include "define.h"
#include "nsCompo/nsCompo.h"
#include "nsMkcrd.h"
#include "cCryptOpenSSL.h"
#include "cInput.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
/**
 * @details
 */
nsCompo::cApplication* nsCompo::cApplication::oMakeApplication(const int argc, const _TCHAR* argv[], const _TCHAR* envp[])
{
    return new cAppLocal(argc, argv, envp);
}

/**
 * @details
 */
cAppLocal::cAppLocal(const int argc, const _TCHAR* argv[], const _TCHAR* envp[])
    : nsCompo::cArgUtils(argc, argv, nsCompo::cArgStyleParser::oMakeMsArgStyle())
    , nsCompo::SIMPLE_STATS(2) // Display time in seconds
    , sStatId_(TEXT("The process spent"))
{
    vStartElapse(sStatId_);                                             // Start measurement of required time
    tcout << APP_NAME << TEXT(" version ")
        << APP_VERSION << std::endl
        << APP_COPYRIGHT << std::endl;
    Assert(iGetArgc() > 1, nsCompo::cRequestToShowUsage(sShowUsage())); // Display usage
    vScanParams();                                                      // Read command arguments
    //
    // Read environment variables and values to be indicated.
    //
    {
        nsCompo::cEnvStrings oEnvStrings;                               // Read all pairs of OS's environment variable [key = value].
        oEnvStrings.vGetTable(oTableOfReplacement_, envp);              // Set replacement string derived from environment variable.
    }
}

/**
 * @details
 */
cAppLocal::~cAppLocal()
{
    vStopElapse(sStatId_);                                               // Duration measurement end
}

/**
 * @details
 */
const tstring cAppLocal::sShowUsage() const
{
    tostringstream tos;
    tos
    << TEXT("Usage:") << std::endl
    << TEXT("  mkcrd   /f [dir_name_string/]<file_name_string>") << std::endl
    << TEXT("          [/u <username_string>[@<db_alias_string>]]") << std::endl
    << TEXT("          [/h]") << std::endl
    << std::endl
    << TEXT("  [] encloses optional symbols.") << std::endl
    << TEXT("  <> encloses mandatory symbols.") << std::endl
    << TEXT("  '|' separates alternative symbols.") << std::endl
    << TEXT("  '-' can be used instead of '/'.") << std::endl
    << TEXT("  '...' means the symbol immediately before can be repeated once or more.") << std::endl
    << std::endl
    << TEXT("  dir_name_string:") << std::endl
    << TEXT("         Write a directory name where the credential file will be generated.") << std::endl
    << TEXT("         When specifying by absolute path, please describe from '//'.") << std::endl
    << TEXT("         It will be considered that the current directory is specified when omitted. ") << std::endl
    << TEXT("  file_name_string:") << std::endl
    << TEXT("         Write a credential file name string.") << std::endl
    << TEXT("  username_string:") << std::endl
    << TEXT("         Write a Oracle username. And it will be case sensitive in 11g or later by default.") << std::endl
    << TEXT("  db_alias_string:") << std::endl
    << TEXT("         Write a database-alias name.") << std::endl
    << TEXT("         It will be considered that the local database is specified when omitted. ") << std::endl
    << std::endl
    << TEXT("Examples of the way to be used:") << std::endl
    << TEXT("  mkcrd /f //home/user/userid.crd") << std::endl
    << TEXT("  mkcrd /f userid.crd /u SCOTT@ORCL.world") << std::endl
    ;
    return tos.str();
}

/**
 * @details
 */
void cAppLocal::vScanParams()
{
    try
    {
        nsCompo::cVectorScope oSw;                                      // Detected switch

        // Create active switch list
        nsCompo::cSwitchUtils oSu;
        oSu.vAddOpt("/h,/H,-h,-H,--help", "HP");
        oSu.vAddOpt("/f,/F,-f,-F,--cred_file_path", "CF");
        oSu.vAddOpt("/u,/U,-u,-U,--connect_string", "CS");
        oSu.vAddOpt("/inv,/INV,-inv,-INV,--invertion", "IV");
        
        //
        // Request help  --help
        //
        if (iSearchOpt(TEXT("HP"), 2, 0, 0, oSu, oSw))
        {
            Assert(false, nsCompo::cRequestToShowUsage(sShowUsage()));  // Display usage
        }
        
        //
        // Credential file location [--cred_file_path]
        //
        if (iSearchOpt(TEXT("CF"), 0, 1, 1, oSu, oSw))
        {
            sCredFile_ = oSw[1];
        }
        
        //
        // DB connection string encryption [--connect_string]
        //
        if (iSearchOpt(TEXT("CS"), 1, 1, 3, oSu, oSw)) // Since "AS SYSDBA" or "AS SYSOPER" may be specified, up to 3.
        {
            sConnStr_ = oSw[1];
            tcout << TEXT("A connectivity will be established by ") << sConnStr_ <<  TEXT(".") << std::endl;
        }
        
        //
        // DB connection string decoding --invertion
        // Because this is a hidden option, do not display on screen at the time of help
        //
        if (iSearchOpt(TEXT("IV"), 1, 0, 0, oSu, oSw))
        {
            Assert(sCredFile_.size() > 0, nsCompo::cInvalidParamValue(TEXT("Error - Specify a credential file when decryption.")));
        }
        
        //
        // Check that invalid option is not used
        //
        vSearchInvalidOpt(oSu);
        Assert(sCredFile_.size() > 0, nsCompo::cInvalidParamValue(TEXT("Error - Credential file is not specified.")));
        Assert(sCredFile_.size() < _MAX_PATH, nsCompo::cInvalidParamValue(TEXT("Error - Credential file name is too long.")));
    }
    catch (const nsCompo::cVectorScope& e)
    {
        tostringstream tos;
        tos << e.sShowMsg(TEXT("Error - "));
        Assert(false, nsCompo::cInvalidParamValue(tos.str()));
    }
}

/**
 * @details
 */
void cAppLocal::vSplitUser( const tstring& sOrg, tstring& sUser, tstring& sPass, tstring& sHost )
{
    tstring::const_iterator p, b, e = sOrg.end();
    p = b = sOrg.begin();
    while (p != e && *p != '/' && *p != '@' && *p != ' ')
    {
        ++p;
    }
    sUser = tstring(b, p);
    if (p == e)
    {
        return;
    }
    if (*p == '/')
    {
        p = b = (p + 1);
        while (p != e && *p != '@' && *p != ' ')
        {
            ++p;
        }
        sPass = tstring(b, p);
        if (p == e)
        {
            return;
        }
    }
    if (*p == '@')
    {
        p = b = (p + 1);
        while (p != e && *p != ' ')
        {
            ++p;
        }
        sHost = tstring(b, p);
        if (p == e)
        {
            return;
        }
    }
    if (*p == ' ')
    {
        p = b = (p + 1);
        tstring s(b, e);
        std::transform(s.begin(), s.end(), s.begin(), toupper); 
        if (s == TEXT("AS SYSDBA"))
        {
            iMode_ = tSesMode::iSysDba;
        }
        else if (s == TEXT("AS SYSOPER"))
        {
            iMode_ = tSesMode::iSysOper;
        }
    }
    else
    {
        iMode_ = tSesMode::iDefault;
    }
}

/**
 * @details
 */
void cAppLocal::iMainService() 
{
    enum {MAX_BUFF_SIZE = 256};
    tcout.setf(std::ios::fixed);
    tcout << std::setprecision(1);                      // Number of decimal places of floating point number
    cCryptOpenSSL oCrypt(TEXT("MKRCD"));                // Encryption / decomposition processing class
    if (iDoesTagExist(TEXT("IV")))                      // Were decryption required?
    {
        tcout << TEXT("\"") << sCredFile_ << TEXT("\" will be read to decription.") << std::endl;
        tifstream ifs( sCredFile_.c_str() );
        tostringstream toss;
        toss << TEXT("Error - A credential file \"") << sCredFile_ << TEXT("\" could not open.");
        Assert(! ifs.fail(), nsCompo::cInvalidParamValue(toss.str()));
        
        // Read the encrypted 4-byte integer character string one by one into the buffer.
        tstring sCryptedStr;
        TCHAR pBuff[MAX_BUFF_SIZE];
        while (ifs)
        {
            ifs.read(pBuff, MAX_BUFF_SIZE);
            sCryptedStr.append(pBuff, (tstring::size_type) ifs.gcount());
        }
        sConnStr_ = oCrypt.sDecryptStr( sCryptedStr);   // Decryption
        tcout << TEXT("Connect string is:") << std::endl;
        tcout << sConnStr_ << std::endl;
    }
    else
    {
        tcout << TEXT("A credential will be save as ") << sCredFile_ <<  TEXT(".") << std::endl;
        
        // Encrypt sConnStr_ and store it in sCredFile_.
        tstring sUser;
        tstring sHost;
        tstring sPass;
        vSplitUser(sConnStr_,sUser,sPass,sHost);
        cInput oInput(sUser,sPass,sHost,iMode_);
        oInput.inputRequest();
        sConnStr_ = oInput.sGetInputString();
        tofstream ofs( sCredFile_.c_str() );
        Assert(! ofs.fail(), nsCompo::cInvalidParamValue(TEXT("Error - A credential file could not open.")));
//                  tcout << sConnStr_ << std::endl;

        tstring sCryptedStr( oCrypt.sEncryptStr( sConnStr_) );  // Encryption.
        
        ofs << sCryptedStr << std::endl;
    }
}
} // ps::app::mkcrd
} // ps::app
} // ps

