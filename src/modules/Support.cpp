#include "../../include/Support.h"
#include <iostream>
#include <algorithm>
#include <map>
#include "../../include/Global.h"
#include <boost/algorithm/string.hpp>

extern "C" ModuleInterface* create()
{
    return new Support;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

Support::Support()
{
}

Support::~Support()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(D);
    delete D;
}
void Support::Init()
{
    D = new Data();
    D->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(D);

    timerlong();
}


void Support::stop()
{
    run = false;
    D->stop();
    std::cout << "Support::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void Support::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Support::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Support::parse_privmsg, this)));
}

void Support::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void Support::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        PRIVMSG(data, "!");
    }
}


void Support::ParseData(std::vector< std::string > data)
{
}


void Support::ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    Users& U = Global::Instance().get_Users();
    string nick = HostmaskToNick(data);
    string auth = U.GetAuth(nick);
    /*if (args.size() == 0)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "auth"))
                {
                    authme(nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "reloadchan"))
                {
                    DBChannelInfo(chan);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }*/
    /*if (args.size() >= 1)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "support"))
                {
                	std::string supportstring = args[0];
					if (args.size() >= 2)
					{
						for (unsigned int i = 1; i < args.size(); i++)
						{
							supportstring = supportstring + " " + args[i];
						}
					}
                    support(nick, auth, supportstring);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }*/
}


void Support::timerrun()
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

void Support::timerlong()
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

void Support::support(std::string nick, std::string auth, std::string supportstring)
{
    std::cout << "Support::support" << std::endl;
    support_ret = support_nick_string_map.insert (pair< std::string, std::string > (nick, supportstring));
    if (support_ret.second == true)
    {
        std::cout << "new ticket" << std::endl;
        support_nick.push_back(nick);
    }
    else
    {
        std::cout << "nickname has already a ticket" << std::endl;
    }
}
