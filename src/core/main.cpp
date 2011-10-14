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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>
#include "../include/core/Bot.h"
#include "../include/core/Global.h"
#include "../include/core/Output.h"


bool forever = true;
std::string sPidFile;

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

void sighandler(int sig)
{
    std::cout<< "Signal " << sig << " caught..." << std::endl;
    switch (sig) {
    case SIGABRT: case SIGTERM:
        /* Do something */
    case SIGILL:
        /* Do something */
        break;
    case SIGSEGV:
        /* Do something */
        break;
    case SIGURG:
        /* Do something */
        break;
    case SIGUSR1: case SIGUSR2:
        /* Do something */
        break;
    default:
        break;
    }
    if (sig == 15)
    {
        exit(EXIT_SUCCESS);
    }
    if (sig == 2)
    {
        exit(EXIT_SUCCESS);
    }
    remove(sPidFile.c_str());
    forever = false;
}

void CheckPid()
{
    pid_t pid;
    std::string sPid = "-2";
    int iFilePid = -2;

    // check if process in pidfile is still running
    std::ifstream ifFilePid (sPidFile.c_str());
    if (ifFilePid.is_open())
    {
        while ( ifFilePid.good() )
        {
            getline (ifFilePid,sPid);
            std::cout << sPid << std::endl;
        }
        ifFilePid.close();
    }
    std::stringstream ss(sPid);
    ss >> iFilePid;
    if (kill(iFilePid, 0) != -1)
    {
        printf("still running \r\n");
        exit(EXIT_FAILURE);
    }
    // end check

    // write current pid to pidfile
    pid = getpid();
    std::cout << pid << std::endl;
    if (pid < 0) {
        printf("pid < 0 FAIL \r\n");
        exit(EXIT_FAILURE);
    }

    std::ofstream ofPidFile (sPidFile.c_str());
    if (ofPidFile.is_open())
    {
        ofPidFile << pid;
        ofPidFile.close();
    }
    else
    {
        printf("cant open pid file \r\n");
        exit(EXIT_FAILURE);
    }
    // end writing pid file
}

int main(int argc, char *argv[])
{
    SetupSIGSEGVSignal();
    SetupSIGTERMSignal();

    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler);

    while(forever)
    {
        bool ineedroot = false;
        std::string sIniFile = "NULL";
        std::string sPidFileLocation = "NULL";
        std::string sLogFileLocation = "NULL";
        std::string sName = "NULL";

        sIniFile = "conf/bot.ini";
        sPidFileLocation = "/var/run/ircppbot/";
        sLogFileLocation = "log/";
        sName = "bot";

        std::vector< std::string > args;
        for (int nArg = 0; nArg < argc; nArg++)
        {
            args.push_back(argv[nArg]);
        }
        for (uint nArg = 0; nArg < args.size(); nArg++)
        {
            if (args[nArg] == "--config" || args[nArg] == "-c")
            {
                if ((nArg+1) <= args.size())
                {
                    sIniFile = args[nArg+1];
                }
            }
            if (args[nArg] == "--debug" || args[nArg] == "-d")
            {
                if ((nArg+1) <= args.size())
                {
                    int i;
                    std::stringstream ss(args[nArg+1]);
                    ss >> i;
                    Output::Instance().setDebugLevel(i);
                }
            }
            if (args[nArg] == "--pid" || args[nArg] == "-p")
            {
                if ((nArg+1) <= args.size())
                {
                    sPidFileLocation = args[nArg+1];
                }
            }
            if (args[nArg] == "--log" || args[nArg] == "-l")
            {
                if ((nArg+1) <= args.size())
                {
                    sLogFileLocation = args[nArg+1];
                }
            }
            if (args[nArg] == "--name" || args[nArg] == "-n")
            {
                if ((nArg+1) <= args.size())
                {
                    sName = args[nArg+1];
                }
            }
            if (args[nArg] == "--INeedRootPowerz")
            {
                ineedroot = true;
            }
            if (args[nArg] == "--help" || "-h")
            {
                ineedroot = true;
            }
        }
        std::string sLogFile = sLogFileLocation + sName + ".log";
        sPidFile = sPidFileLocation + sName + ".pid";
        Output::Instance().setLogFile(sLogFile);
        Output::Instance().init();
        CheckPid();
        if (getuid() == 0 && ineedroot != true)
        {
            std::cout << "dont start as root" << std::endl;
            std::cout << "if you really need root. start with -INeedRootPowerz" << std::endl;
            return 0;
        }
        else
        {
            if (sIniFile == "NULL")
            {
                std::cout << "please provide a ini file with -c <inifile>" << std::endl;
            }
            else
            {
                std::cout << "start first time" << std::endl;
                Output::Instance().addOutput("++++++++++++++++++++++++++++++++++++", 2);
                Output::Instance().addOutput("+ Start bot on " + Output::Instance().sFormatTime("%d-%m-%Y %H:%M:%S") + " +", 2);
                Output::Instance().addOutput("++++++++++++++++++++++++++++++++++++", 2);
                usleep(2000000);
                Global::Instance().set_Run(true);
                while (Global::Instance().get_Run() == true)
                {
                    std::cout << "make new bot" << std::endl;
                    Bot *b = new Bot();
                    b->Init(sIniFile);
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
    return 0;
}


//int main(int argc, char *argv[])
//{
//    struct pidfh *pfh;
//    pid_t otherpid, childpid;
//
//    pfh = pidfile_open("/var/run/daemon.pid", 0600, &otherpid);
//    if (pfh == NULL) {
//         if (errno == EEXIST) {
//                 errx(EXIT_FAILURE, "Daemon already running, pid: %jd.",
//                     (intmax_t)otherpid);
//         }
//         /* If we cannot create pidfile from other reasons, only warn. */
//         warn("Cannot open or create pidfile");
//    }
//
//    if (daemon(0, 0) == -1) {
//         warn("Cannot daemonize");
//         pidfile_remove(pfh);
//         exit(EXIT_FAILURE);
//    }
//
//    pidfile_write(pfh);
//
//    for (;;) {
//         /* Do work. */
//         run(argc, argv);
//         childpid = fork();
//         switch (childpid) {
//         case -1:
//                 syslog(LOG_ERR, "Cannot fork(): %s.", strerror(errno));
//                 break;
//         case 0:
//                 pidfile_close(pfh);
//                 /* Do child work. */
//                 break;
//         default:
//                 syslog(LOG_INFO, "Child %jd started.", (intmax_t)childpid);
//                 break;
//         }
//    }
//    pidfile_remove(pfh);
//    exit(EXIT_SUCCESS);
//}
