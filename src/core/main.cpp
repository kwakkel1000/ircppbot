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
    std::cout<< "Signal " << i_num << " caught..." << std::endl;

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
    remove(sPidFile.c_str());
    forever = false;
}

void TermAction(int i_num, siginfo_t * i_info, void * i_val)
{
    std::cout<< "Signal " << i_num << " caught..." << std::endl;
    usleep(5000000);
    remove(sPidFile.c_str());
    forever = false;
    Global::Instance().set_Run(false);
    usleep(5000000);
    exit(0);
}

void Usr1Action(int i_num, siginfo_t * i_info, void * i_val)
{
    std::cout<< "Signal " << i_num << " caught..." << std::endl;
    usleep(5000000);
    // restart
}

void SetupSignal()
{
    struct sigaction new_action;
    struct sigaction old_action;
    /* Set up the structure to specify the new action. */
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = SA_SIGINFO
#if defined(linux) || defined(__linux) || defined(__linux__)
      | SA_NOMASK
#endif
      ;

// usr
    new_action.sa_sigaction = Usr1Action;
    sigaction (SIGUSR1, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGUSR1, &new_action, NULL);

// SegFault
    new_action.sa_sigaction = SegFaultAction;
    sigaction (SIGSEGV, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGSEGV, &new_action, NULL);


// termination
    new_action.sa_sigaction = TermAction;
    sigaction (SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGINT, &new_action, NULL);

    sigaction (SIGHUP, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGHUP, &new_action, NULL);

    sigaction (SIGTERM, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
      sigaction (SIGTERM, &new_action, NULL);

}

bool ReadPidFile()
{
    std::string sPid = "-2";
    int iFilePid = -2;

    // check if process in pidfile is still running
    std::ifstream ifFilePid (sPidFile.c_str());
    if (ifFilePid.is_open())
    {
        while ( ifFilePid.good() )
        {
            getline (ifFilePid,sPid);
            //std::cout << sPid << std::endl;
        }
        ifFilePid.close();
    }
    std::stringstream ss(sPid);
    ss >> iFilePid;
    if (kill(iFilePid, 0) != -1)
    {
        printf("still running \r\n");
        return true;
        exit(EXIT_FAILURE);
    }
    return false;
    // end check
}

void WritePidFile(int iPid)
{
    // write current pid to pidfile
    if (iPid < 0) {
        printf("pid < 0 FAIL \r\n");
        exit(EXIT_FAILURE);
    }

    std::ofstream ofPidFile (sPidFile.c_str());
    if (ofPidFile.is_open())
    {
        ofPidFile << iPid;
        ofPidFile.close();
    }
    else
    {
        printf("cant open pid file \r\n");
        exit(EXIT_FAILURE);
    }
    // end writing pid file
}

static bool isRoot()
{
    // User root? If one of these were root, we could switch the others to root, too
    return (geteuid() == 0 || getuid() == 0);
}


int main(int argc, char *argv[])
{
    SetupSignal();
    // deprecated (Global::Instance().get_Run() == treu);
    while(forever)
    {
        bool ineedroot = false;
        bool bForeground = false;
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
            if (args[nArg] == "--help" || args[nArg] == "-h")
            {
                fprintf(stdout, "Runs the bot on irc (default as %s, %s%s.pid, %s%s.log %s)\n", sName.c_str(), sPidFileLocation.c_str(), sName.c_str(), sLogFileLocation.c_str(), sName.c_str(), sIniFile.c_str());
                fprintf(stdout, "USAGE bot [OPTIONS]\n");
                fprintf(stdout, "Available options:\n");
                fprintf(stdout, "\t-h, --help           List options\n");
                fprintf(stdout, "\t-v, --version        Output version and exit\n");
                fprintf(stdout, "\t-f, --foreground     Don't fork into the background\n");
                fprintf(stdout, "\t-c, --config         Set config file (%s)\n", sIniFile.c_str());
                fprintf(stdout, "\t-d, --debug          Set debug level [1-10] (default 5)\n");
                fprintf(stdout, "\t-p, --pid            Set Pid file location (default %s)\n", sPidFileLocation.c_str());
                fprintf(stdout, "\t-l, --log            Set log file location (default %s)\n", sLogFileLocation.c_str());
                fprintf(stdout, "\t-n, --name           Set name for pid/log files (default %s)\n", sName.c_str());
                fprintf(stdout, "\t--INeedRootPowerz    Requered when running as root\n");
                exit(EXIT_SUCCESS);
            }
            if (args[nArg] == "--version" || args[nArg] == "-v")
            {
                fprintf(stdout, "Ircppbot %s", "2.5.2");
                exit(EXIT_SUCCESS);
            }
            if (args[nArg] == "--foreground" || args[nArg] == "-f")
            {
                bForeground = true;
            }
            if (args[nArg] == "--config" || args[nArg] == "-c")
            {
                if ((nArg+1) < args.size())
                {
                    sIniFile = args[nArg+1];
                }
            }
            if (args[nArg] == "--debug" || args[nArg] == "-d")
            {
                if ((nArg+1) < args.size())
                {
                    int i;
                    std::stringstream ss(args[nArg+1]);
                    ss >> i;
                    Output::Instance().setDebugLevel(i);
                }
            }
            if (args[nArg] == "--pid" || args[nArg] == "-p")
            {
                if ((nArg+1) < args.size())
                {
                    sPidFileLocation = args[nArg+1];
                }
            }
            if (args[nArg] == "--log" || args[nArg] == "-l")
            {
                if ((nArg+1) < args.size())
                {
                    sLogFileLocation = args[nArg+1];
                }
            }
            if (args[nArg] == "--name" || args[nArg] == "-n")
            {
                if ((nArg+1) < args.size())
                {
                    sName = args[nArg+1];
                }
            }
            if (args[nArg] == "--INeedRootPowerz")
            {
                ineedroot = true;
            }
        }
        std::string sLogFile = sLogFileLocation + sName + ".log";
        sPidFile = sPidFileLocation + sName + ".pid";
        Output::Instance().setLogFile(sLogFile);
        Output::Instance().init();
        if (isRoot())
        {
            fprintf(stdout, "Your are running ircppbot as root!\n");
            fprintf(stdout, "this is dangerouse and can cause great damage!\n");
            if (!ineedroot) {
                return 1;
            }
            fprintf(stdout, "You have been warned.\n");
            fprintf(stdout, "Hit CTRL+C now if you don't want to run ircppbot as root.\n");
            fprintf(stdout, "ircppbot will start in 30 seconds.\n");
            sleep(30);
        }
        if (ReadPidFile())
        {
            return 1;
        }
        if (bForeground) {
            int iPid = getpid();
            WritePidFile(iPid);
            fprintf(stdout, "Staying open for debugging\n");
            fprintf(stdout, "pid [%d]\n", iPid);
        }
        else
        {
            fprintf(stdout, "Forking into the background\n");

            int iPid = fork();

            if (iPid == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return 1;
            }

            if (iPid > 0) {
                // We are the parent. We are done and will go to bed.
                WritePidFile(iPid);
                fprintf(stdout, "pid [%d]\n", iPid);
                return 0;
            }
            // Redirect std in/out/err to /dev/null
            close(0); open("/dev/null", O_RDONLY);
            close(1); open("/dev/null", O_WRONLY);
            close(2); open("/dev/null", O_WRONLY);

            // We are the child. There is no way we can be a process group
            // leader, thus setsid() must succeed.
            setsid();
            // Now we are in our own process group and session (no
            // controlling terminal). We are independent!
        }

        Output::Instance().addOutput("++++++++++++++++++++++++++++++++++++", 2);
        Output::Instance().addOutput("+ Start bot on " + Output::Instance().sFormatTime("%d-%m-%Y %H:%M:%S") + " +", 2);
        Output::Instance().addOutput("++++++++++++++++++++++++++++++++++++", 2);
        usleep(2000000);
        Global::Instance().set_Run(true);
        while (Global::Instance().get_Run() == true)
        {
            Output::Instance().addOutput("make new bot", 2);
            Bot *b = new Bot();
            b->Init(sIniFile);
            b->Run();
            usleep(5000000);
            Output::Instance().addOutput("delete bot", 2);
            delete b;
            Output::Instance().addOutput("delete global vars bot", 2);
            Global::Instance().delete_all();
            usleep(5000000);
        }
        Output::Instance().addOutput("closing down", 2);
        std::cin.get();
        return 0;
    }
    return 0;
}
