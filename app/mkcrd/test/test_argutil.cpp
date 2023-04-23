#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "../define.h"
#include "../nsCompo/nsCompo.h"

namespace ps
{
namespace app
{
namespace mkcrd
{
namespace test
{

class cArgs : protected ps::app::mkcrd::nsCompo::cArgUtils
{
public:
    cArgs(const int argc, const _TCHAR* argv[])
        : cArgUtils(argc, argv, ps::app::mkcrd::nsCompo::cArgStyleParser::oMakeMsArgStyle())
    {
        try
        {
            ps::app::mkcrd::nsCompo::cVectorScope oSw;

            // Create active switch list
            ps::app::mkcrd::nsCompo::cSwitchUtils oSu;
            oSu.vAddOpt("/h,/H,-h,-H,--help", "HP");
            oSu.vAddOpt("/f,/F,-f,-F,--cred_file_path", "CF");
            oSu.vAddOpt("/u,/U,-u,-U,--connect_string", "CS");
            oSu.vAddOpt("/inv,/INV,-inv,-INV,--invertion", "IV");
            
            if (iSearchOpt(TEXT("HP"), 2, 0, 0, oSu, oSw))
            {
                tcout << "Display help messege." << std::endl;
            }
            
            if (iSearchOpt(TEXT("CF"), 0, 1, 1, oSu, oSw))
            {
                tcout << "crdfile:" << oSw[1] << std::endl;
            }
            
            if (iSearchOpt(TEXT("CS"), 1, 1, 3, oSu, oSw))
            {
                tcout << "connection string:" << oSw[1] << std::endl;
            }
            
            if (iSearchOpt(TEXT("IV"), 1, 0, 0, oSu, oSw))
            {
                tcout << "decode switch"  << std::endl;
            }
            
            vSearchInvalidOpt(oSu);
        }
        catch (const ps::app::mkcrd::nsCompo::cVectorScope& e)
        {
            tcout << e.sShowMsg(TEXT("Error - ")) << std::endl;
        }
    }
};

} // ps::app::mkcrd::test
} // ps::app::mkcrd
} // ps::app
} // ps

int main(const int argc, const char* argv[])
{
    ps::app::mkcrd::test::cArgs oArgs(argc, argv);
    
    return EXIT_SUCCESS;
}

