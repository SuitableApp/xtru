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

int main(const int argc, const char* argv[], const char* envp[])
{
    tstrmap oMap;
    ps::app::mkcrd::nsCompo::cEnvStrings oEnv;
    oEnv.vGetTable( oMap, envp );

    for(auto itr = oMap.begin(); itr != oMap.end(); ++itr)
    {
        tcout << "key = " << itr->first << ", val = " << itr->second << std::endl;
    }
        
    return EXIT_SUCCESS;
}

