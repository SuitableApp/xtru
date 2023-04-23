#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "../define.h"
#include "../cInput.h"

int iSplitUser( const tstring& sOrg, tstring& sUser, tstring& sPass, tstring& sHost )
{
    tstring::const_iterator p, b, e = sOrg.end();
    int iMode = ps::app::mkcrd::tSesMode::iDefault;
    p = b = sOrg.begin();
    while (p != e && *p != '/' && *p != '@' && *p != ' ')
    {
        ++p;
    }
    sUser = tstring(b, p);
    if (p == e)
    {
        return iMode;
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
            return iMode;
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
            return iMode;
        }
    }
    if (*p == ' ')
    {
        p = b = (p + 1);
        tstring s(b, e);
        std::transform(s.begin(), s.end(), s.begin(), toupper); 
        if (s == TEXT("AS SYSDBA"))
        {
            iMode = ps::app::mkcrd::tSesMode::iSysDba;
        }
        else if (s == TEXT("AS SYSOPER"))
        {
            iMode = ps::app::mkcrd::tSesMode::iSysOper;
        }
    }
    else
    {
        iMode = ps::app::mkcrd::tSesMode::iDefault;
    }
    
    return iMode;
}

int main(int argc, char *argv[]) {
    tstring user;
    tstring pass;
    tstring host;
    int mode = ps::app::mkcrd::tSesMode::iDefault;
    if( argc > 2 )
    {
        tcout << "Usage:test_input [[UserName]/[Password]@[HostName][ as (sysdba)(sysoper)]]" << std::endl;
    }
    else
    {
        if( argc == 2 )
        {
            mode = iSplitUser(argv[1],user,pass,host);
        }
        ps::app::mkcrd::cInput oInput(user,pass,host,mode);
        oInput.inputRequest();
        tstring sConnStr = oInput.sGetInputString();
        tcout << "Connect string is:" << std::endl << sConnStr << std::endl;
    }

    return EXIT_SUCCESS;
}
