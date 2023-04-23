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
 * @file mkcrd.cpp
 * @brief Main program
 * @author S.Sasaki
 * @date 2017/01/10
 */
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "define.h"
#include "nsCompo/nsCompo.h"
#include "nsMkcrd.h"

/**
 * Catch ctrl+c
 */
void break_handler(int s)
{
    tcout << "Canceled by the demand." << std::endl;
    exit(0);
}

/**
 * @brief 
 *  Main program
 * @param[in] argc
 *  Number of arguments on the command line.
 * @param[in] argv
 *  Array of argument strings on the command line.
 * @param[in] envp
 *  Environment variable.
 * @return Exit code.
 */
int main(const int argc, const char* argv[], const char* envp[])
{
    
    std::ios::sync_with_stdio();                 // Share stdio.h and iostream
    
    // Ctrl+C hook interrupt
    struct sigaction tSigIntHandler;
    tSigIntHandler.sa_handler = break_handler;
    sigemptyset(&tSigIntHandler.sa_mask);
    tSigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &tSigIntHandler, nullptr);

    int iRtn = 0;
    
    try
    {
        std::unique_ptr<ps::app::mkcrd::nsCompo::cApplication>  ptr(ps::app::mkcrd::nsCompo::cApplication::oMakeApplication(argc, argv, envp));
        ptr->iMainService();
    }
    catch (const ps::app::mkcrd::nsCompo::cBaseException& e)
    {
        tcout << e.twhat() << std::endl;
        iRtn = e.rc();
    }
    {
        tcout << (
            iRtn ? "Failed with an error." : "Completed successfully."
        ) << std::endl;
    }
    return iRtn;
}

