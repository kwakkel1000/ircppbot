#include "../../include/Nickserv.h"
#include "../../include/IrcSocket.h"
#include "../../include/Users.h"
#include "../../include/Channels.h"
#include "../../include/Database.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>


extern "C" UserManagement* create()
{
    return new Nickserv;
}

extern "C" void destroy(UserManagement* x)
{
    delete x;
}

void Nickserv::ParseData(vector<string> data)
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
            botnick = data[2];
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

void Nickserv::auth(vector<string> data)
{
    U->SetAuth(data[3], data[3]);
    U->SetOaccess(data[3], -1);
    string sqlstring;
    if (U->AddAuth(data[3]) == true)
    {
        sqlstring = "INSERT into auth (auth) VALUES ( '" + data[3] + "' );";
        RawSql(sqlstring);
    }
    vector<string> userchannels = U->GetChannels(data[3]);
    if (caseInsensitiveStringCompare(userchannels[0], "NULL") == false)
    {
        for ( unsigned int i = 0 ; i < userchannels.size(); i++ )
        {
            cout << "userchannels[" << convertInt(i) << "] " << userchannels[i] << endl;
        }
        DBUserInfo(data[3]);
    }
    else
    {
        U->DelUser(data[3]);
    }
}

