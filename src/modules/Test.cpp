#include "../../include/Test.h"
#include <iostream>

extern "C" ModuleInterface* create()
{
    return new Test;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

Test::Test()
{
}

void Test::Init()
{
    D = new Data();
    D->Init(true, false, false, true);
    ID->AddConsumer(D);
    runthreadloop = true;
}


void Test::stopthreadloop()
{
    runthreadloop = false;
}

void Test::threadloop()
{
    std::vector< std::string > privmsg_result;
    while (runthreadloop)
    {
        privmsg_result = D->GetPrivmsgQueue();
        PRIVMSG(privmsg_result);
    }
}

void Test::PRIVMSG(std::vector< std::string > data)
{
    std::cout << "Test::PRIVMSG" << std::endl;
    std::string returnstr = "PRIVMSG #blubs :" + data[0] + " " + data[1] + " " + data[2] + " " + data[3];
    if (data.size() >= 5)
    {
        for (unsigned int i = 4; i < data.size(); i++)
        {
            returnstr = returnstr + " " + data[i];
        }
    }
    returnstr = returnstr + "\r\n";
    Send(returnstr);
}

void Test::timerrun()
{
    //cout << "Test::timerrun()" << endl;
}
