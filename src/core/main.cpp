//
//
//  @ Project : ircppbot
//  @ File Name : main.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//


#include <signal.h>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>
#include "../include/core/Bot.h"
#include "../include/core/Global.h"
#include "../include/core/Output.h"

void SegFaultAction(int i_num, siginfo_t * i_info, void * i_val)
{
    const siginfo_t & v = * i_info;

    std::cout << v.si_signo << "= Signal number\n";
    std::cout << v.si_errno << "= An errno value\n";
    std::cout << v.si_code << "= Signal code\n";
    std::cout << v.si_pid << "= Sending process ID\n";
    std::cout << v.si_uid << "= Real user ID of sending process\n";
    std::cout << v.si_status << "= Exit value or signal\n";
#if defined(linux) || defined(__linux) || defined(__linux__)
    std::cout << v.si_utime << "= User time consumed\n";
    std::cout << v.si_stime << "= System time consumed\n";
    std::cout << v.si_int << "= POSIX.1b signal\n";
    std::cout << v.si_ptr << "= POSIX.1b signal\n";
#endif
    std::cout << v.si_addr << "= Memory location which caused fault\n";
    std::cout << v.si_band << "= Band event\n";
#if defined(linux) || defined(__linux) || defined(__linux__)
    std::cout << v.si_fd << "= File descriptor\n";
#endif

    // throw "HELP";
    throw * i_info;
}

void SetupSIGSEGVSignal()
{
    struct sigaction a_sig[1] = { { {0} } };
    struct sigaction a_old_sig[1] = { { {0} } };

    a_sig->sa_sigaction = SegFaultAction;
    a_sig->sa_flags = SA_SIGINFO
#if defined(linux) || defined(__linux) || defined(__linux__)
    | SA_NOMASK
#endif
    ;

    if ( -1 == sigaction( SIGSEGV, a_sig, a_old_sig ) )
    {
        printf("Failed to set SIGSEGV handler");
    }
}

void TermAction(int i_num, siginfo_t * i_info, void * i_val)
{
    // thePlatform->shutdownflag=1;
    exit(0);
}

void SetupSIGTERMSignal()
{
    struct sigaction a_sig[1] = { { {0} } };
    struct sigaction a_old_sig[1] = { { {0} } };

    a_sig->sa_sigaction = TermAction;
    a_sig->sa_flags = SA_SIGINFO
#if defined(linux) || defined(__linux) || defined(__linux__)
      | SA_NOMASK
#endif
      ;

    if ( -1 == sigaction( SIGTERM, a_sig, a_old_sig ) )
    {
        printf("Failed to set SIGTERM handler");
    }
}

int main(int argc, char *argv[])
{
    SetupSIGSEGVSignal();
    SetupSIGTERMSignal();
    std::string inifile = "NULL";
    inifile = "conf/tree.ini";
    bool ineedroot = false;

    std::vector< std::string > args;
    for (int nArg = 0; nArg < argc; nArg++)
    {
        args.push_back(argv[nArg]);
    }
    for (uint nArg = 0; nArg < args.size(); nArg++)
    {
        if (args[nArg] == "-config" || args[nArg] == "-c")
        {
            if ((nArg+1) <= args.size())
            {
                inifile = args[nArg+1];
            }
        }
        if (args[nArg] == "-debug" || args[nArg] == "-d")
        {
            if ((nArg+1) <= args.size())
            {
            	int i;
            	std::stringstream ss(args[nArg+1]);
            	ss >> i;
            	Output::Instance().setDebugLevel(i);
            }
        }
        if (args[nArg] == "-INeedRootPowerz")
        {
            ineedroot = true;
        }
    }
    if (getuid() == 0 && ineedroot != true)
    {
        std::cout << "dont start as root" << std::endl;
        std::cout << "if you really need root. start with -INeedRootPowerz" << std::endl;
        return 0;
    }
    else
    {
        if (inifile == "NULL")
        {
            std::cout << "please provide a ini file with -c <inifile>" << std::endl;
        }
        else
        {
            std::cout << "start first time" << std::endl;
            usleep(2000000);
            Global::Instance().set_Run(true);
            while (Global::Instance().get_Run() == true)
            {
                std::cout << "make new bot" << std::endl;
                Bot *b = new Bot();
                b->Init(inifile);
                b->Run();
                std::cout << "sleep" << std::endl;
                usleep(5000000);
                std::cout << "delete bot" << std::endl;
                delete b;
                std::cout << "delete global vars bot" << std::endl;
                Global::Instance().delete_all();
                std::cout << "sleep" << std::endl;
                usleep(5000000);
            }
        }
        std::cout << "closing down" << std::endl;
        std::cin.get();
        return 0;
    }
    return 0;
}
