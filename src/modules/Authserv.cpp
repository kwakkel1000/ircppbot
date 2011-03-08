#include "../../include/Authserv.h"
#include "../../include/Users.h"
#include "../../include/Global.h"

#include <boost/algorithm/string.hpp>
#include <iostream>



extern "C" UserManagement* create() {
    return new Authserv;
}

extern "C" void destroy(UserManagement* x) {
    delete x;
}

void Authserv::ParseData(std::vector< std::string > data)
{
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
        if (data[1] == "330")       //WHOIS auth
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

void Authserv::auth(std::vector< std::string > data)
{
    Users& U = Global::Instance().get_Users();
    U.SetAuth(data[3], data[4]);
    U.SetOaccess(data[3], -1);
    std::string sqlstring;
    if (U.AddAuth(data[4]) == true)
    {
        sqlstring = "INSERT into auth (auth) VALUES ( '" + data[4] + "' );";
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
