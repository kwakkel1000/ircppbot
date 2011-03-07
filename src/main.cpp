#include "../include/Bot.h"
#include "../include/Global.h"
#include <string>
#include <iostream>
#include <vector>


int main (int argc, char *argv[])
{
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
