#include "../../include/Nickserv.h"
#include "../../include/Users.h"
#include "../../include/Global.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

extern "C" UserManagement* create()
{
    return new Nickserv;
}

extern "C" void destroy(UserManagement* x)
{
    delete x;
}

void Nickserv::stop()
{
    run = false;
}

void Nickserv::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Nickserv::parse_raw, this)));
    raw_parse_thread->join();
}

void Nickserv::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void Nickserv::ParseData(std::vector< std::string > data)
{
    //cout << "nickserv parsedata" << endl;
    if (data.size() == 3)
    {
        if (data[1] == "JOIN")      //JOIN
        {
            JOIN(data);
        }
        if (data[1] == "NICK")      //NICK nickchange
        {
            NICK(data);
        }
    }
    if (data.size() >= 3)
    {
        if (data[1] == "QUIT")      //QUIT
        {
            QUIT(data);
        }
        if (data[1] == "PART")      //PART
        {
            PART(data);
        }
    }
    if (data.size() >= 4)
    {
        if (data[1] == "001")   //welcome
        {
            Global::Instance().set_BotNick(data[2]);
        }
        if (data[1] == "318")       //WHOIS end
        {
            //WHOIS(data);
        }
        if (data[1] == "307")       //WHOIS regged nick
        {
            auth(data);
        }
        if (data[1] == "402")       //WHOIS no server
        {
            //WHOIS(data);
        }
    }
    if (data.size() >= 5)
    {
        if (data[1] == "KICK")      //KICK
        {
            KICK(data);
        }
        if (data[1] == "MODE")      //KICK
        {
            MODE(data);
        }
    }
    if (data.size() >= 11)
    {
        if (data[1] == "352")       //WHO
        {
            WHO(data);
        }
    }
}

void Nickserv::auth(std::vector< std::string > data)
{
    Users& U = Global::Instance().get_Users();
    U.SetAuth(data[3], data[3]);
    U.SetOaccess(data[3], -1);
    std::string sqlstring;
    if (U.AddAuth(data[3]) == true)
    {
        sqlstring = "INSERT into auth (auth) VALUES ( '" + data[3] + "' );";
        RawSql(sqlstring);
    }
    std::vector< std::string > userchannels = U.GetChannels(data[3]);
    if (boost::iequals(userchannels[0], "NULL") == false)
    {
        for ( unsigned int i = 0 ; i < userchannels.size(); i++ )
        {
            std::cout << "userchannels[" << convertInt(i) << "] " << userchannels[i] << std::endl;
        }
        DBUserInfo(data[3]);
    }
    else
    {
        U.DelUser(data[3]);
    }
}

