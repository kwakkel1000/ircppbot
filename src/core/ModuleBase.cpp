#include "../include/core/ModuleBase.h"

#include "../include/core/Global.h"
#include "../include/core/Database.h"
#include <iostream>
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
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    ConfigReaderInterface& CR = Global::Instance().get_ConfigReader();
    std::string overwatchchannel = CR.GetString("overwatchchannel");
    std::string debugstring = "PRIVMSG " + overwatchchannel + " :[" + nick + ":" + auth + "] [" + chan + ":" + convertInt(C.GetAccess(chan, auth)) + "] ";
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
    size_t chanpos1 = std::string::npos;
    size_t chanpos2 = std::string::npos;
    size_t chanpos3 = std::string::npos;
    size_t triggerpos = std::string::npos;
    chanpos1 = data[2].find("#");
    chanpos2 = data[3].find("#");
    std::string chan = "NULL";
    std::string command = "NULL";
    int triggertype = -1;
    int chantrigger = -1;
    std::string nick = HostmaskToNick(data);
    if (data.size() >= 4)
    {
        data3 = data[3];
        boost::erase_all(data3, ":");
    }
    triggerpos = data3.substr(0, trigger.length()).find(trigger);
	if (data3.substr(0, trigger.length()) == trigger)
	{
		data3 = data3.substr(trigger.length(), data3.length()-1);
	}
    if (data.size() >= 5)
    {
		chanpos3 = data[4].find("#");
    }
    if (triggerpos != std::string::npos)
    {
        triggertype = 1; //PRIVMSG ... :!;
        if (data3 != "")
        {
            if (chanpos2 != std::string::npos && chanpos3 == std::string::npos)		//chanpos1 yes/no both valid
            {
                chantrigger = 1;   //PRIVMSG nick #chan :!#chan command   ||   PRIVMSG nick bot :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    //boost::erase_all(chan, trigger);
                    if (chan != "")
                    {
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(nick, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos1 != std::string::npos && chanpos2 == std::string::npos && chanpos3 == std::string::npos)
            {
                chantrigger = 0;    //PRIVMSG nick #chan :!command
                //cout << "channel: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    command = data3;
                    //boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        chan = data[2];
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(nick, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos1 == std::string::npos && chanpos2 == std::string::npos && chanpos3 == std::string::npos)
            {
                chantrigger = -1;   //PRIVMSG nick bot :!command
                if (data.size() >= 4)
                {
                    command = data3;
                    chan = "NULL";
                    //boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(nick, command, chan, args, chantrigger);
                    }
                }
            }
            else if (chanpos2 == std::string::npos && chanpos3 != std::string::npos)	//chanpos1 yes/no both valid
            {
                chantrigger = 0;    //PRIVMSG nick #chan :!command #chan	||		PRIVMSG nick bot :!command #chan
                if (data.size() >= 5)
                {
                    command = data3;
                    //boost::erase_all(command, trigger);
                    if (command != "")
                    {
						chan = data[4];
                        for (unsigned int i = 5; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                        ParsePrivmsg(nick, command, chan, args, chantrigger);
                    }
                }
            }
        }
    }
    else
    {
        if (data3 != "")
        {
            if (chanpos1 == std::string::npos && chanpos2 != std::string::npos && chanpos3 == std::string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick bot :#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    command = data[4];
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                    ParsePrivmsg(nick, command, chan, args, chantrigger);
                }
            }
            else if (chanpos1 == std::string::npos && chanpos2 == std::string::npos && chanpos3 != std::string::npos)
            {
                chantrigger = 0;   //PRIVMSG nick bot :command #chan
                if (data.size() >= 5)
                {
                    chan = data[4];
                    command = data3;
                    for (unsigned int i = 5; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                    ParsePrivmsg(nick, command, chan, args, chantrigger);
                }
            }
            else if (chanpos1 == std::string::npos && chanpos2 == std::string::npos && chanpos3 == std::string::npos)
            {
                chantrigger = -1;   //PRIVMSG nick bot :command
                if (data.size() >= 4)
                {
                    chan == "NULL";
                    command = data3;
                    for (unsigned int i = 4 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                    ParsePrivmsg(nick, command, chan, args, chantrigger);
                }
            }
        }
    }
}


void ModuleBase::simulate(std::string nick, std::string auth, std::string chan, std::string simulate_nick, std::string simulate_command, std::vector< std::string > args, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    std::string returnstring;
    int oaccess = U.GetOaccess(nick);
    std::cout << convertInt(oaccess) << std::endl;
    if (oaccess >= oa)
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("simulate_done", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    	ParsePrivmsg(simulate_nick, simulate_command, chan, args, 1);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
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
    ConfigReaderInterface& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    std::cout << data << std::endl;
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
        std::cout << hostname_str << std::endl;
        std::cout << databasename_str << std::endl;
        std::cout << username_str << std::endl;
        std::cout << pass_str << std::endl;
        std::cout << "db fail " << state << std::endl;
    }
    db->disconnect();
    delete db;
    return sql_result;
}

bool ModuleBase::RawSql(std::string data)
{
    ConfigReaderInterface& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    std::cout << data << std::endl;
    database *db;
    db = new database();    // lol whut... connecting for each query? :'D
    int state = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (state == 200)
    {
        db->updateQuery( data.c_str() );
    }
    else
    {
        std::cout << hostname_str << std::endl;
        std::cout << databasename_str << std::endl;
        std::cout << username_str << std::endl;
        std::cout << pass_str << std::endl;
        std::cout << "db fail " << state << std::endl;
    }
    db->disconnect();
    delete db;
    return true;
}

//reply
std::string ModuleBase::irc_reply(std::string reply_name, std::string reply_language)
{
    ReplyInterface& R = Global::Instance().get_Reply();
    return R.irc_reply(reply_name, reply_language);
}

std::string ModuleBase::irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)
{
    ReplyInterface& R = Global::Instance().get_Reply();
    return R.irc_reply_replace(source_string, search_string, replace_string);
}

void ModuleBase::replyReload()
{
    ReplyInterface& R = Global::Instance().get_Reply();
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

std::string ModuleBase::centre(int cmdsize, unsigned int rowamount, unsigned int length)
{
	std::string returnstring = "";
    for (unsigned int l = 0; l < (((length * rowamount) / 2) - cmdsize/2); l++)
    {
        returnstring = returnstring + " ";
    }
    return returnstring;
}

std::string ModuleBase::fillspace(std::string data, unsigned int length)
{
	std::string tmpstr = data;
	for (unsigned int l = data.size(); l < length; l++)
	{
		tmpstr = tmpstr + " ";
	}
    return tmpstr;
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
            tmpvector = tmpvector + fillspace(data[j], length);
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
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int ModuleBase::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}
