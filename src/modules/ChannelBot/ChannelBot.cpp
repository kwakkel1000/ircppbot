#include "include/ChannelBot.h"
#include "../../include/core/Global.h"
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

extern "C" ModuleInterface* create()
{
    return new ChannelBot;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

ChannelBot::ChannelBot()
{
}

ChannelBot::~ChannelBot()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(D);
    delete D;
}

void ChannelBot::Init()
{
    channelbottrigger = Global::Instance().get_ConfigReader().GetString("channelbottrigger");
    D = new Data();
    D->Init(true, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(D);

    ChannelsInterface& C = Global::Instance().get_Channels();
    vector<string> chans = C.GetChannels();
    for (unsigned int i = 0; i < chans.size(); i++)
    {
        DBChannelInfo(chans[i]);
    }
    longtime = 100;

    int Tijd;
    time_t t= time(0);
    Tijd = t;

    timer_long_sec.push_back(Tijd + 120);
    timer_long_command.push_back("time 120 from now");
    timer_long_sec.push_back(Tijd + 20);
    timer_long_command.push_back("time 20 from now");
    timerlong();
    BindInit();
}

void ChannelBot::stop()
{
    run = false;
    D->stop();
    std::cout << "ChannelBot::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void ChannelBot::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&ChannelBot::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&ChannelBot::parse_privmsg, this)));
}

void ChannelBot::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void ChannelBot::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        PRIVMSG(data, channelbottrigger);
    }
}
void ChannelBot::ParseData(std::vector< std::string > data)
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
        if (data[1] == "INVITE")   //INVITE
        {
            INVITE(data);
        }
    }
    if (data.size() >= 5)
    {
        if (data[1] == "KICK")      //KICK
        {
            KICK(data);
        }
    }
}

void ChannelBot::ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    //cout << "ChannelBot" << endl;
    UsersInterface& U = Global::Instance().get_Users();
    string auth = U.GetAuth(nick);
    if (args.size() == 0)
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
                if (boost::iequals(commands[i], "ping"))
                {
                    ping(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "version"))
                {
                    version(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "up"))
                {
                    up(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "down"))
                {
                    down(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "resync"))
                {
                    //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                    resync(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "access"))
                {
                    if (chantrigger == 0)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, nick, auth, cas[i]);
                    }
                    if (chantrigger == 1)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, nick, auth, cas[i]);
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "myaccess"))
                {
                    if (chantrigger == 0)
                    {
                        myaccess(nick, nick, auth, cas[i]);
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "users"))
                {
                    //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                    users(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "ccommands"))
                {
                    ccommands(nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() == 1)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "access"))
                {
                    if (chantrigger == 0)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, args[0], U.GetAuth(args[0]), cas[i]);
                    }
                    if (chantrigger == 1)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, args[0], U.GetAuth(args[0]), cas[i]);
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "myaccess"))
                {
                    if (chantrigger == 0)
                    {
                        if (U.GetGod(nick) == 1)
                        {
                            myaccess(nick, args[0], U.GetAuth(args[0]), cas[i]);
                        }
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() >= 1)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "op"))
                {
                    op(chan, nick, auth, args[0], U.GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "deop"))
                {
                    deop(chan, nick, auth, args[0], U.GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "voice"))
                {
                    voice(chan, nick, auth, args[0], U.GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "devoice"))
                {
                    devoice(chan, nick, auth, args[0], U.GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "kickuser"))
                {
                    string reason = "";
                    for (unsigned int j = 1; j < args.size()-1; j++)
                    {
                        reason = reason + args[j] + " ";
                    }
                    if (args.size() > 1)
                    {
                        reason = reason + args[args.size()-1];
                    }
                    kickuser(chan, nick, auth, args[0], U.GetAuth(args[0]), reason, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "deluser"))
                {
                	for (unsigned int args_it = 0; args_it < args.size(); args_it++)
                	{
						deluser(chan, nick, auth, args[args_it], U.GetAuth(args[args_it]), cas[i]);
                	}
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() >= 2)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "adduser"))
                {
                	unsigned int last_args_it = args.size() - 1;
                	for (unsigned int args_it = 0; args_it < last_args_it; args_it++)
                	{
						adduser(chan, nick, auth, args[args_it], U.GetAuth(args[args_it]), convertString(args[last_args_it]), cas[i]);
                	}
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (boost::iequals(commands[i], "changelevel"))
                {
                	unsigned int last_args_it = args.size() - 1;
                	for (unsigned int args_it = 0; args_it < last_args_it; args_it++)
                	{
						changelevel(chan, nick, auth, args[args_it], U.GetAuth(args[args_it]), convertString(args[last_args_it]), cas[i]);
                	}
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() >= 2)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "simulate"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        overwatch(commands[i], command, chan, nick, auth, args);
                    	std::vector< std::string > simulate_args;
                        for (unsigned int j = 2; j < args.size(); j++)
                        {
                        	simulate_args.push_back(args[j]);
                        }
                        simulate(nick, auth, chan, args[0], args[1], simulate_args, cas[i]);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
            }
        }
    }
}

void ChannelBot::version(string chan, string nick, int ca)
{
    string returnstr = "PRIVMSG " + chan + " :" + nick + ": Tran V0.2 C++ IRC bot\r\n";
    Send(returnstr);
}

void ChannelBot::authme(string nick, int ca)
{
    string returnstr = "WHOIS " + nick + " " + nick + "\r\n";
    Send(returnstr);
}

void ChannelBot::ping(string chan, string nick, int ca)
{
    string returnstr = "PRIVMSG " + chan + " :" + nick + ": PONG\r\n";
    Send(returnstr);
}

void ChannelBot::adduser(string chan, string nick, string auth, string reqnick, string reqauth, int reqaccess, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int access = C.GetAccess(chan, auth);
        if (access > reqaccess || U.GetGod(nick) == 1)
        {
            if (C.AddAuth(chan, reqauth))
            {
                int uid = U.GetUid(reqnick);
                int cid = C.GetCid(chan);
                if ((cid > -1) && (uid > -1) && (reqaccess > 0))
                {
                    string sqlstring = "INSERT into users ( uid, cid, access) VALUES ( '" + convertInt(uid) + "', '" + convertInt(cid) + "', '" + convertInt(reqaccess) + "' );";
                    C.SetAccess(chan, reqauth, reqaccess);
                    RawSql(sqlstring);
                    string sendstring = "NOTICE " + nick + " :user " + reqnick + "[" + reqauth + "] added to the userlist with " + convertInt(reqaccess) + " access\r\n";
                    Send(sendstring);
                }
            }
        }
    }
}

void ChannelBot::deluser(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int access = C.GetAccess(chan, auth);
        int tmpaccess = C.GetAccess(chan, reqauth);
        string sqlstring;
        if (tmpaccess > 0)
        {
            if (tmpaccess < access || U.GetGod(nick) == 1)
            {
                int uid = U.GetUid(reqnick);
                int cid = C.GetCid(chan);
                if ((cid > -1) && (uid > -1))
                {
                    string sqlstring = "DELETE from users where uid = '" + convertInt(uid) + "' AND cid = '" + convertInt(cid) + "';";
                    C.DelAuth(chan, reqauth);
                    RawSql(sqlstring);
                    string sendstring = "NOTICE " + nick + " :user " + reqnick + "[" + reqauth + "] deleted from the userlist\r\n";
                    Send(sendstring);
                }
            }
        }
    }
}

void ChannelBot::changelevel(string chan, string nick, string auth, string reqnick, string reqauth, int reqaccess, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int oldaccess = C.GetAccess(chan, reqauth);
        int access = C.GetAccess(chan, auth);
        if (((access > reqaccess) && (access > oldaccess)) || U.GetGod(nick) == 1)
        {
            int uid = U.GetUid(reqnick);
            int cid = C.GetCid(chan);
            if ((cid > -1) && (uid > -1) && (reqaccess > 0))
            {
                string sqlstring = "UPDATE users SET access = '" + convertInt(reqaccess) + "' WHERE cid = '" + convertInt(cid) + "' AND uid = '" + convertInt(uid) + "';";
                C.SetAccess(chan, reqauth, reqaccess);
                RawSql(sqlstring);
                string sendstring = "NOTICE " + nick + " :user " + reqnick + "[" + reqauth + "] changed access to " + convertInt(reqaccess) + " access\r\n";
                Send(sendstring);
            }
        }
    }
}

void ChannelBot::op(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    int access = C.GetAccess(chan, auth);
    if (access >= C.GetGiveops(chan))
    {
        if (boost::iequals(reqauth,"NULL") != true)
        {
            int tmpaccess = C.GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U.GetGod(nick) == 1)
                {
                    string returnstring = "MODE " + chan + " +o " + reqnick + "\r\n";
                    Send(returnstring);
                }
            //}
        }
        else
        {
            string returnstring = "MODE " + chan + " +o " + reqnick + "\r\n";
            Send(returnstring);
        }
    }
}

void ChannelBot::deop(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    int access = C.GetAccess(chan, auth);
    if (access >= C.GetGiveops(chan))
    {
        if (boost::iequals(reqauth,"NULL") != true)
        {
            int tmpaccess = C.GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U.GetGod(nick) == 1)
                {
                    string returnstring = "MODE " + chan + " -o " + reqnick + "\r\n";
                    Send(returnstring);
                }
            //}
        }
        else
        {
            string returnstring = "MODE " + chan + " -o " + reqnick + "\r\n";
            Send(returnstring);
        }
    }
}

void ChannelBot::voice(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    int access = C.GetAccess(chan, auth);
    if (access >= C.GetGiveops(chan))
    {
        if (boost::iequals(reqauth,"NULL") != true)
        {
            int tmpaccess = C.GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U.GetGod(nick) == 1)
                {
                    string returnstring = "MODE " + chan + " +v " + reqnick + "\r\n";
                    Send(returnstring);
                }
            //}
        }
        else
        {
            string returnstring = "MODE " + chan + " +v " + reqnick + "\r\n";
            Send(returnstring);
        }
    }
}

void ChannelBot::devoice(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    int access = C.GetAccess(chan, auth);
    if (access >= C.GetGiveops(chan))
    {
        if (boost::iequals(reqauth,"NULL") != true)
        {
            int tmpaccess = C.GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U.GetGod(nick) == 1)
                {
                    string returnstring = "MODE " + chan + " -v " + reqnick + "\r\n";
                    Send(returnstring);
                }
            //}
        }
        else
        {
            string returnstring = "MODE " + chan + " -v " + reqnick + "\r\n";
            Send(returnstring);
        }
    }
}

void ChannelBot::users(string chan, string nick, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    vector<string> auths = C.GetAuths(chan);
    sort(auths.begin(), auths.end());
    string returnstr;
    cout << auths.size() << endl;
    vector<string> sortauths;
    vector<int> sortaccess;
    for ( unsigned int i = 0 ; i < auths.size(); i++ )
    {
        int access = C.GetAccess(chan, auths[i]);
        if (access > 0)
        {
            bool added = false;
            for (unsigned int j = 0; j < sortaccess.size(); j++)
            {
                if (sortaccess[j] < access)
                {
                    sortauths.insert(sortauths.begin()+j, auths[i]);
                    sortaccess.insert(sortaccess.begin()+j, access);
                    j = sortaccess.size();
                    added = true;
                }
            }
            if (!added)
            {
                sortauths.push_back(auths[i]);
                sortaccess.push_back(access);
            }
        }
    }
    for (unsigned int i = 0; i < sortauths.size(); i++)
    {
        returnstr = "NOTICE " + nick + " :" + convertInt(sortaccess[i]) + "    " + sortauths[i] + "\r\n";
        Send(returnstr);
    }
}

void ChannelBot::kickuser(string chan, string nick, string auth, string reqnick, string reqauth, string reason, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int access = C.GetAccess(chan, auth);
        int tmpaccess = C.GetAccess(chan, reqauth);
        string sqlstring;
        //if (tmpaccess > 0)
        //{
            if (tmpaccess < access || U.GetGod(nick) == 1)ƀ
            {
                string returnstring = "KICK " + chan + " " + reqnick + " :" + reason + "\r\n";
                Send(returnstring);
            }
        //}
    }
    else
    {
        string returnstring = "KICK " + chan + " " + reqnick + " :" + reason + "\r\n";
        Send(returnstring);
    }
}

void ChannelBot::access(string chan, string nick, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    int access = C.GetAccess(chan, reqauth);
    int oaccess = U.GetOaccess(reqnick);
    int god = U.GetGod(reqnick);
    string returnstr;
    if (access > 0)
    {
        returnstr = "NOTICE " + nick + " :" + reqnick + "[" + reqauth + "] has " + convertInt(access) + " access to " + chan + "\r\n";
        Send(returnstr);
    }
    else
    {
        returnstr = "NOTICE " + nick + " :" + reqnick + "[" + reqauth + "] lacks access to " + chan + "\r\n";
        Send(returnstr);
    }
    if (oaccess > 0)
    {
        returnstr = "NOTICE " + nick + " :" + reqnick + "[" + reqauth + "] has " + convertInt(oaccess) + " oaccess\r\n";
        Send(returnstr);
    }
    if (god > 0)
    {
        returnstr = "NOTICE " + nick + " :" + reqnick + "[" + reqauth + "] has God enabled\r\n";
        Send(returnstr);
    }
}

void ChannelBot::myaccess(string nick, string reqnick, string reqauth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    string reply_string = "NOTICE " + nick + " :" + irc_reply("myaccess", U.GetLanguage(nick)) + "\r\n";
    reply_string = irc_reply_replace(reply_string, "$nick$", reqnick);
    reply_string = irc_reply_replace(reply_string, "$auth$", reqauth);
    Send(reply_string);
    vector<string>channels = C.GetChannels();
    for ( unsigned int i = 0 ; i < channels.size(); i++ )
    {
        int access = C.GetAccess(channels[i], reqauth);
        if (access > 0)
        {
            string returnstr = "NOTICE " + nick + " :" + channels[i] + "     " + convertInt(access) + "\r\n";
            Send(returnstr);
        }
    }
}

void ChannelBot::up(string chan, string nick, string auth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    string reply_string;
    //bool giveop = false;
    //bool givevoice = false;
    int access = C.GetAccess(chan, auth);
    if (access >= C.GetGiveops(chan))
    {
        if (C.GetOp(chan, nick) == false)
        {
            string returnstring = "MODE " + chan + " +o " + nick + "\r\n";
            Send(returnstring);
            reply_string = "NOTICE " + nick + " :" + irc_reply("up_op", U.GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
        else
        {
			reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U.GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
    }
    else if (access >= C.GetGivevoice(chan))
    {
        if (C.GetVoice(chan, nick) == false)
        {
            string returnstring = "MODE " + chan + " +v " + nick + "\r\n";
            Send(returnstring);
            reply_string = "NOTICE " + nick + " :" + irc_reply("up_voice", U.GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
        else
        {
			reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U.GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
    }
    if (C.GetOp(chan, nick) == true && C.GetVoice(chan, nick) == true)
    {
        reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U.GetLanguage(nick)) + "\r\n";
		reply_string = irc_reply_replace(reply_string, "$channel$", chan);
		Send(reply_string);
    }
}

void ChannelBot::down(string chan, string nick, string auth, int ca)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    string reply_string;
    if (C.GetOp(chan, nick) == true && C.GetVoice(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -ov " + nick + " " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down", U.GetLanguage(nick)) + "\r\n";
    }
    else if (C.GetVoice(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -v " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_devoice", U.GetLanguage(nick)) + "\r\n";
    }
    else if (C.GetOp(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -o " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_deop", U.GetLanguage(nick)) + "\r\n";
    }
    if (C.GetOp(chan, nick) == false && C.GetVoice(chan, nick) == false)
    {
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_already_down", U.GetLanguage(nick)) + "\r\n";
    }
    reply_string = irc_reply_replace(reply_string, "$channel$", chan);
    Send(reply_string);
}

void ChannelBot::resync(string chan, string nick, string auth, int ca)
{
	ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
	std::vector< std::string > nicks = C.GetNicks(chan);
	int access ;
	for (unsigned int i = 0; i < nicks.size(); i++)
	{
		if (!boost::iequals(Global::Instance().get_BotNick(), nicks[i]))
		{
			access = C.GetAccess(chan, U.GetAuth(nicks[i]));
			if (access >= C.GetGiveops(chan))
			{
				if (C.GetOp(chan, nicks[i]) == false)
				{
					string returnstring = "MODE " + chan + " +o " + nicks[i] + "\r\n";
					Send(returnstring);
				}
			}
			else if (access >= C.GetGivevoice(chan))
			{
				if (C.GetVoice(chan, nicks[i]) == false)
				{
					string returnstring = "MODE " + chan + " +v " + nicks[i] + "\r\n";
					Send(returnstring);
				}
			}
			if ((access < C.GetGiveops(chan)) && (access < C.GetGivevoice(chan)))
			{
				if (C.GetOp(chan, nicks[i]) == true || C.GetVoice(chan, nicks[i]) == true)
				{
					string returnstring = "MODE " + chan + " -ov " + nicks[i] + " " + nicks[i] + "\r\n";
					Send(returnstring);
				}
			}
			else if (access < C.GetGiveops(chan))
			{
				if (C.GetOp(chan, nicks[i]) == false)
				{
					string returnstring = "MODE " + chan + " -o " + nicks[i] + "\r\n";
					Send(returnstring);
				}
			}
			else if (access < C.GetGivevoice(chan))
			{
				if (C.GetVoice(chan, nicks[i]) == false)
				{
					string returnstring = "MODE " + chan + " -v " + nicks[i] + "\r\n";
					Send(returnstring);
				}
			}
		}
	}
}

void ChannelBot::ccommands(string nick, string auth, int ca)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    unsigned int length = U.GetWidth(nick);
    unsigned int amount = U.GetWidthLength(nick);
    string commandrpl = irc_reply("ccommands", U.GetLanguage(nick));
    returnstring = "NOTICE " + nick + " :";
    for (unsigned int l = 0; l < (((length * amount) / 2) - commandrpl.size()/2); l++)
    {
        returnstring = returnstring + " ";
    }
    returnstring = returnstring + commandrpl + "\r\n";
    Send(returnstring);
    vector<string> sortbinds = binds;
    sort (sortbinds.begin(), sortbinds.end());
    vector<string> command_reply_vector = lineout(sortbinds, amount, length);
    for (unsigned int h = 0; h < command_reply_vector.size(); h++)
    {
        returnstring = "NOTICE " + nick + " :" + command_reply_vector[h] + "\r\n";
        Send(returnstring);
    }
}

void ChannelBot::INVITE(vector<string> data)
{
    string returnstr = "JOIN " + data[3] + "\r\n";
    Send(returnstr);
}

void ChannelBot::JOIN(vector<string> data)
{
    std::string botnick = Global::Instance().get_BotNick();
    UsersInterface& U = Global::Instance().get_Users();
    std::string chan = data[2];
    boost::erase_all(chan, ":");
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
    }
    else
    {
        if (U.GetUid(nick) >= 0)
        {
            OnUserJoin(chan, nick);
        }
    }
}

void ChannelBot::PART(vector<string> data)
{/*
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
        vector<string> chanusers = C->GetNicks(chan[0]);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            U->DelChannel(chanusers[i], chan[0]);
            C->DelNick(chan[0], chanusers[i]);
            if (U->GetChannels(chanusers[i])[0] == "NULL")
            {
                cout << "no channels left" << endl;
                U->DelUser(chanusers[i]);
            }
        }
        C->DelChannel(chan[0]);
    }
    else
    {
        C->DelNick(chan[0], nick);
        U->DelChannel(nick, chan[0]);
        if (U->GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U->DelUser(nick);
        }
        cout << "PART" << endl;
    }*/
}

void ChannelBot::KICK(vector<string> data)
{/*
    string chan = data[2];
    string nick = data[3];
    if (nick == botnick)
    {
        vector<string> chanusers = C->GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            C->DelNick(chan, chanusers[i]);
            U->DelChannel(chanusers[i], chan);
            if (U->GetChannels(chanusers[i])[0] == "NULL")
            {
                cout << "no channels left" << endl;
                U->DelUser(chanusers[i]);
            }
        }
        C->DelChannel(chan);
    }
    else
    {
        C->DelNick(chan, nick);
        U->DelChannel(nick, chan);
        if (U->GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U->DelUser(nick);
        }
        cout << "KICK" << endl;
    }*/
}

void ChannelBot::MODE(vector<string> data)
{
    string nick = HostmaskToNick(data);
    /*if (nick == botnick)
    {

    }
    else
    {

    }*/
}

void ChannelBot::QUIT(vector<string> data)
{/*
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
        U->~Users();
        C->~Channels();
    }
    else
    {
        vector<string> channels = U->GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C->DelNick(channels[i], nick);
        }
        U->DelUser(nick);
    }*/
}

void ChannelBot::NICK(vector<string> data)
{/*
    string oldnick = HostmaskToNick(data);
    vector<string> nick = Split(data[2], ":",true,true);
    if (oldnick == botnick)
    {
        botnick = nick[0];
        U->ChangeNick(oldnick, nick[0]);
        vector<string> channels = U->GetChannels(nick[0]);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C->DelNick(channels[i], oldnick);
            C->AddNick(channels[i], nick[0]);
        }
        cout << "NICK" << endl;
    }
    else
    {
        U->ChangeNick(oldnick, nick[0]);
        vector<string> channels = U->GetChannels(nick[0]);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C->DelNick(channels[i], oldnick);
            C->AddNick(channels[i], nick[0]);
        }
        cout << "NICK" << endl;
    }*/
}


void ChannelBot::OnUserJoin(string chan, string nick)
{
    UsersInterface& U = Global::Instance().get_Users();
    up(chan, nick, U.GetAuth(nick), 0);
}

void ChannelBot::BindInit()
{

    commands.clear();
    binds.clear();
    cas.clear();

    vector< vector<string> > sql_result;
    string sql_string;
    unsigned int i;

    sql_string = "select command, bind, caccess from commands";
    sql_result = RawSqlSelect(sql_string);
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        commands.push_back(sql_result[i][0]);
        binds.push_back(sql_result[i][1]);
        cas.push_back(convertString(sql_result[i][2]));
    }
}

void ChannelBot::DBChannelInfo(string data)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    vector< vector<string> > sql_result;
    string sql_string = "select channels.id, channels.giveops, channels.givevoice from channels where channels.channel = '" + data + "';";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << endl;
        C.SetCid(data, convertString(sql_result[i][0]));
        C.SetGiveops(data, convertString(sql_result[i][1]));
        C.SetGivevoice(data, convertString(sql_result[i][2]));
    }
    sql_string = "select users.access, auth.auth from users JOIN auth ON users.uid = auth.id JOIN channels ON users.cid = channels.id where channels.channel = '" + data + "';";
    sql_result = RawSqlSelect(sql_string);
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << endl;
        C.AddAuth(data, sql_result[i][1]);
        C.SetAccess(data, sql_result[i][1], convertString(sql_result[i][0]));
    }
}

void ChannelBot::DBreplyinit()
{
    vector< vector<string> > sql_result;
    string sql_string = "select reply_name, reply, language FROM reply;";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << endl;
        reply_name_vector.push_back(sql_result[i][0]);
        reply_vector.push_back(sql_result[i][1]);
        reply_language_vector.push_back(sql_result[i][2]);
    }
}

void ChannelBot::timerrun()
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

void ChannelBot::timerlong()
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
