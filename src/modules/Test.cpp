#include "../../include/Test.h"
#include "../../include/Global.h"
#include <boost/algorithm/string.hpp>

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

Test::~Test()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(D);
    delete D;
}
void Test::Init()
{
    D = new Data();
    D->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(D);

    timerlong();
}


void Test::stop()
{
    run = false;
    D->stop();
    std::cout << "Test::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void Test::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Test::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Test::parse_privmsg, this)));
}

void Test::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void Test::parse_privmsg()
{
    /*std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        //PRIVMSG(data, channelbottrigger);
    }*/
}


void Test::ParseData(std::vector< std::string > data)
{
    cout << "Test" << endl;
    if (data.size() >= 1)
    {
		std::string returnstr = "PRIVMSG #blubs :" + data[0];
		if (data.size() >= 2)
		{
			for (unsigned int i = 1; i < data.size(); i++)
			{
				returnstr = returnstr + " " + data[i];
			}
		}
		returnstr = returnstr + "\r\n";
		SendLowPriority(returnstr);
    }
}


void Test::ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
}


void Test::timerrun()
{
    //cout << "channelbot::timerrun()" << endl;
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    longtime++;
    if (longtime >= 100)
    {
        timerlong();
        longtime = 0;
    }
    for (int i = timer_sec.size() - 1; i >= 0; i--)
    {
        if (timer_sec[i] < Tijd)
        {
            cout << timer_command[i] << endl;
            timer_sec.erase(timer_sec.begin()+i);
            timer_command.erase(timer_command.begin()+i);
        }
    }
}

void Test::timerlong()
{
    int Tijd;
    time_t t= time(0);
    Tijd = t;
    Tijd = Tijd + 100;
    for (int i = timer_long_sec.size() - 1; i >= 0; i--)
    {
        if (timer_long_sec[i] < Tijd)
        {
            cout << "timer_long to timer " << timer_long_command[i] << endl;
            timer_sec.push_back(timer_long_sec[i]);
            timer_command.push_back(timer_long_command[i]);
            timer_long_sec.erase(timer_long_sec.begin()+i);
            timer_long_command.erase(timer_long_command.begin()+i);
        }
    }
}
