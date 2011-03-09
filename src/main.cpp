#include "../include/Bot.h"
#include "../include/Global.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <signal.h>

void SegFaultAction( int i_num, siginfo_t * i_info, void * i_val )
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
	struct sigaction a_sig[1] = { { 0 } };
	struct sigaction a_old_sig[1] = { { 0 } };

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

void TermAction( int i_num, siginfo_t * i_info, void * i_val )
{
	//thePlatform->shutdownflag=1;
}

void SetupSIGTERMSignal()
{

	struct sigaction a_sig[1] = { { 0 } };
	struct sigaction a_old_sig[1] = { { 0 } };

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

int main (int argc, char *argv[])
{
	SetupSIGSEGVSignal();
	SetupSIGTERMSignal();
    string inifile = "NULL";
    inifile = "conf/trantweak.ini";
    bool ineedroot = false;

    vector<string> args;
    for (int nArg=0; nArg < argc; nArg++)
    {
        args.push_back(argv[nArg]);
    }
    for (uint nArg=0; nArg < args.size(); nArg++)
    {
        if (args[nArg] == "-c")
        {
            if ((nArg+1) <= args.size())
            {
                inifile = args[nArg+1];
            }
        }
        if (args[nArg] == "-INeedRootPowerz")
        {
            ineedroot = true;
        }
    }
    if (getuid() == 0 && ineedroot != true)
    {
        cout << "dont start as root" << endl;
        cout << "if you really need root. start with -INeedRootPowerz" << endl;
        return 0;
    }
    else
    {
        if (inifile == "NULL")
        {
            cout << "please provide a ini file with -c <inifile>" << endl;
        }
        else
        {
            cout << "start first time" << endl;
            usleep(2000000);
            Global::Instance().set_Run(true);
            while (Global::Instance().get_Run() == true)
            {
                cout << "make new bot" << endl;
                Bot *b = new Bot();
                b->Init(inifile);
                b->Run();
                cout << "sleep" << endl;
                usleep(5000000);
                cout << "delete bot" << endl;
                delete b;
                cout << "delete global vars bot" << endl;
                Global::Instance().delete_all();
                cout << "sleep" << endl;
                usleep(5000000);
            }
        }
        cout << "closing down" << endl;
        cin.get();
        return 0;
    }
    return 0;
}
