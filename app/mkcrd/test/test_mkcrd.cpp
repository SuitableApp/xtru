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
#include "../nsMkcrd.h"

int main(const int argc, const char* argv[], const char* envp[])
{
    
    try
    {
        std::unique_ptr<ps::app::mkcrd::nsCompo::cApplication>  ptr(ps::app::mkcrd::nsCompo::cApplication::oMakeApplication(argc, argv, envp));
        ptr->iMainService();
    }
    catch (const ps::app::mkcrd::nsCompo::cBaseException& e)
    {
        tcout << e.twhat() << std::endl;
    }

    return EXIT_SUCCESS;
}

