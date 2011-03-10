#include "../../include/ModuleBase.h"

#include "../../include/Global.h"
#include "../../include/Database.h"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>


//public
ModuleBase::ModuleBase()
{
}


//protected
//irc

void ModuleBase::overwatch(std::string bind, std::string command, std::string chan, std::string nick, std::string auth, std::vector< std::string > args)
{
    Channels& C = Global::Instance().get_Channels();
    Users& U = Global::Instance().get_Users();
    ConfigReader& CR = Global::Instance().get_ConfigReader();
    std::string overwatchchannel = CR.GetString("overwatchchannel");
    string debugstring = "PRIVMSG " + overwatchchannel + " :[" + nick + ":" + auth + "] [" + chan + ":" + convertInt(C.GetAccess(chan, auth)) + "] ";
    if (U.GetGod(nick) == 1)
    {
        debugstring = debugstring + "[G] ";
    }
    debugstring = debugstring + command + ":" + bind;
    for (unsigned int i = 0; i < args.size(); i++)
    {
        debugstring = debugstring + " " + args[i];
    }
    debugstring = debugstring + "\r\n";
    SendLowPriority(debugstring);
}


void ModuleBase::PRIVMSG(std::vector< std::string > data, std::string trigger)
{
	//cout << "PRIVMSG" << endl;
    std::vector< std::string > args;
    std::string data3;
    size_t chanpos1;
    size_t chanpos2;
    size_t triggerpos;
    chanpos1 = data[2].find("#");
    chanpos2 = data[3].find("#");
    triggerpos = data[3].find(trigger);
    std::string chan = "NULL";
    std::string command = "NULL";
    //int triggered = 0;
    int triggertype = -1;
    int chantrigger = -1;
    if (data.size() >= 4)
    {
        data3 = data[3];
        boost::erase_all(data3, ":");
    }
    if (triggerpos != string::npos)
    {
        triggertype = 1; //PRIVMSG ... :!;
        if (data3 != "")
        {
            if (chanpos1 != string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;    //PRIVMSG nick #channel :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    boost::erase_all(chan, trigger);
                    if (chan != "")
                    {
                        //triggered = 1;
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(data, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos1 != string::npos && chanpos2 == string::npos)
            {
                chantrigger = 0;    //PRIVMSG nick #chan :!command
                cout << "channel: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    command = data3;
                    boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        chan = data[2];
                        //triggered = 1;
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(data, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos1 == string::npos && chanpos2 == string::npos)
            {
                chantrigger = -1;   //PRIVMSG nick user :!command
                if (data.size() >= 4)
                {
                    command = data3;
                    chan = "NULL";
                    boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        //triggered = 1;
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(data, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos1 == string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    boost::erase_all(chan, trigger);
                    if (chan != "")
                    {
                        //triggered = 1;
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(data, command, chan, args, chantrigger);
                    }
                }
            }
        }
    }
    else
    {
        if (data3 != "")
        {
            if (chanpos1 == string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :#chan command
                if (data.size() >= 5)
                {
                    //triggered = 1;
                    chan = data3;
                    command = data[4];
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                    ParsePrivmsg(data, command, chan, args, chantrigger);
                }
            }
            if (chanpos1 == string::npos && chanpos2 == string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :command
                if (data.size() >= 5)
                {
                    chan == "NULL";
                    //triggered = 1;
                    command = data3;
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                    ParsePrivmsg(data, command, chan, args, chantrigger);
                }
            }
        }
    }
}
bool ModuleBase::Send(std::string data)
{
    Global::Instance().get_IrcData().AddSendQueue(data);
    return true;
}

bool ModuleBase::SendHighPriority(std::string data)
{
    Global::Instance().get_IrcData().AddHighPrioritySendQueue(data);
    return true;
}

bool ModuleBase::SendLowPriority(std::string data)
{
    Global::Instance().get_IrcData().AddLowPrioritySendQueue(data);
    return true;
}

//mysql
std::vector< std::vector<std::string> > ModuleBase::RawSqlSelect(std::string data)
{
    ConfigReader& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    cout << data << endl;
    database *db;
    std::vector< std::vector<std::string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    int state = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (state == 200)
    {
        sql_result = db->sql_query( data.c_str() );
    }
    else
    {
        cout << hostname_str << endl;
        cout << databasename_str << endl;
        cout << username_str << endl;
        cout << pass_str << endl;
        cout << "db fail " << state << endl;
    }
    db->disconnect();
    delete db;
    return sql_result;
}

bool ModuleBase::RawSql(std::string data)
{
    ConfigReader& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    cout << data << endl;
    database *db;
    db = new database();    // lol whut... connecting for each query? :'D
    int state = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (state == 200)
    {
        db->updateQuery( data.c_str() );
    }
    else
    {
        cout << hostname_str << endl;
        cout << databasename_str << endl;
        cout << username_str << endl;
        cout << pass_str << endl;
        cout << "db fail " << state << endl;
    }
    db->disconnect();
    delete db;
    return true;
}

//reply
std::string ModuleBase::irc_reply(std::string reply_name, std::string reply_language)
{
    Reply& R = Global::Instance().get_Reply();
    return R.irc_reply(reply_name, reply_language);
}

std::string ModuleBase::irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)
{
    Reply& R = Global::Instance().get_Reply();
    return R.irc_reply_replace(source_string, search_string, replace_string);
}

void ModuleBase::replyReload()
{
    Reply& R = Global::Instance().get_Reply();
    R.Reload();
}

//other
std::string ModuleBase::HostmaskToNick(std::vector<std::string> data)
{
    std::vector< std::string > who;
    boost::split( who, data[0], boost::is_any_of("!"), boost::token_compress_on );
    std::string nick = who[0];
    boost::erase_all(nick, ":");
    return nick;
}

std::vector<std::string> ModuleBase::lineout(std::vector<std::string> data, unsigned int rowamount, unsigned int length)
{
    std::vector< std::string > return_vector;
    std::string tmpvector;
    unsigned int k = 0;
    for (unsigned int j = 0; j < data.size(); j++)
    {
        if (k < rowamount)
        {
            tmpvector = tmpvector + data[j];
            for (unsigned int l = data[j].size(); l < length; l++)
            {
                tmpvector = tmpvector + " ";
            }
            k++;
        }
        if (k >= rowamount)
        {
            return_vector.push_back(tmpvector);
            tmpvector = "";
            k = 0;
        }
    }
    if (k != 0)
    {
        return_vector.push_back(tmpvector);
    }
    return return_vector;
}

std::string ModuleBase::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int ModuleBase::convertString(std::string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}
