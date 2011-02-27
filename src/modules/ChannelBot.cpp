#include "../../include/ChannelBot.h"
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
    stopthreadloop();
}

void ChannelBot::Init()
{
    D->Init(true, false, false, true);
    ID->AddConsumer(D);
    BindInit();

    vector<string> chans = C->GetChannels();
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
    runthreadloop = true;
}

void ChannelBot::stopthreadloop()
{
    runthreadloop = false;
    cout << "notify" << endl;
    parse_wait_condition.notify_one();
}

void ChannelBot::threadloop()
{
    std::vector< std::string > raw_result;
    std::vector< std::string > privmsg_result;
    while (runthreadloop)
    {
        raw_result = D->GetRawQueue();
        privmsg_result = D->GetPrivmsgQueue();
        if (raw_result.size() == 3)
        {
            if (raw_result[1] == "JOIN")      //JOIN
            {
                JOIN(raw_result);
            }
            if (raw_result[1] == "NICK")      //NICK nickchange
            {
                NICK(raw_result);
            }
        }
        if (raw_result.size() >= 3)
        {
            if (raw_result[1] == "QUIT")      //QUIT
            {
                QUIT(raw_result);
            }
            if (raw_result[1] == "PART")      //PART
            {
                PART(raw_result);
            }
        }
        if (raw_result.size() >= 4)
        {
            if (raw_result[1] == "001")   //welcome
            {
                botnick = raw_result[2];
            }
            if (raw_result[1] == "INVITE")   //INVITE
            {
                INVITE(raw_result);
            }
            if (raw_result[1] == "PRIVMSG")   //PRIVMSG
            {
                PRIVMSG(raw_result);
            }
        }
        if (raw_result.size() >= 5)
        {
            if (raw_result[1] == "KICK")      //KICK
            {
                KICK(raw_result);
            }
        }
        //PRIVMSG(privmsg_result);
    }
}

void ChannelBot::ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    cout << "ChannelBot" << endl;
    //cout << "chan " << chan << endl;
    //cout << "command " << command << endl;
    string nick = HostmaskToNick(data);
    string auth = U->GetAuth(nick);
    if (args.size() == 0)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (caseInsensitiveStringCompare(command, binds[i]))
            {
                if (caseInsensitiveStringCompare(commands[i], "auth"))
                {
                    authme(nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "reloadchan"))
                {
                    DBChannelInfo(chan);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "ping"))
                {
                    ping(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "version"))
                {
                    version(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "up"))
                {
                    up(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "down"))
                {
                    down(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "resync"))
                {
                    //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                    resync(chan, nick, auth, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "access"))
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
                if (caseInsensitiveStringCompare(commands[i], "myaccess"))
                {
                    if (chantrigger == 0)
                    {
                        myaccess(nick, nick, auth, cas[i]);
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "users"))
                {
                    //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                    users(chan, nick, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "ccommands"))
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
            if (caseInsensitiveStringCompare(command, binds[i]))
            {
                if (caseInsensitiveStringCompare(commands[i], "access"))
                {
                    if (chantrigger == 0)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, args[0], U->GetAuth(args[0]), cas[i]);
                    }
                    if (chantrigger == 1)
                    {
                        //if (nick isin chan || C->GetAccess(chan, auth) > 0 || U->GetGod() > 0)
                        access(chan, nick, args[0], U->GetAuth(args[0]), cas[i]);
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "myaccess"))
                {
                    if (chantrigger == 0)
                    {
                        if (U->GetGod(nick) == 1)
                        {
                            myaccess(nick, args[0], U->GetAuth(args[0]), cas[i]);
                        }
                    }
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "deluser"))
                {
                    deluser(chan, nick, auth, args[0], U->GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() >= 1)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (caseInsensitiveStringCompare(command, binds[i]))
            {
                if (caseInsensitiveStringCompare(commands[i], "op"))
                {
                    op(chan, nick, auth, args[0], U->GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "deop"))
                {
                    deop(chan, nick, auth, args[0], U->GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "voice"))
                {
                    voice(chan, nick, auth, args[0], U->GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "devoice"))
                {
                    devoice(chan, nick, auth, args[0], U->GetAuth(args[0]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "kickuser"))
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
                    kickuser(chan, nick, auth, args[0], U->GetAuth(args[0]), reason, cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
    if (args.size() == 2)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (caseInsensitiveStringCompare(command, binds[i]))
            {
                if (caseInsensitiveStringCompare(commands[i], "adduser"))
                {
                    adduser(chan, nick, auth, args[0], U->GetAuth(args[0]), convertString(args[1]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
                if (caseInsensitiveStringCompare(commands[i], "changelevel"))
                {
                    changelevel(chan, nick, auth, args[0], U->GetAuth(args[0]), convertString(args[1]), cas[i]);
                    overwatch(commands[i], command, chan, nick, auth, args);
                }
            }
        }
    }
}

void ChannelBot::overwatch(string bind, string command, string chan, string nick, string auth, vector<string> args)
{
    string debugstring = "PRIVMSG " + debugchannel + " :[" + nick + ":" + auth + "] [" + chan + ":" + convertInt(C->GetAccess(chan, auth)) + "] ";
    if (U->GetGod(nick) == 1)
    {
        debugstring = debugstring + "[G] ";
    }
    debugstring = debugstring + command + ":" + bind;
    for (unsigned int i = 0; i < args.size(); i++)
    {
        debugstring = debugstring + " " + args[i];
    }
    debugstring = debugstring + "\r\n";
    Send(debugstring);
}

void ChannelBot::version(string chan, string nick, int ca)
{
    string returnstr = "PRIVMSG " + chan + " :" + nick + ": Tran V0.1 C++ IRC bot\r\n";
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
    if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
    {
        int access = C->GetAccess(chan, auth);
        if (access > reqaccess || U->GetGod(nick) == 1)
        {
            if (C->AddAuth(chan, reqauth))
            {
                int uid = U->GetUid(reqnick);
                int cid = C->GetCid(chan);
                if ((cid > -1) && (uid > -1) && (reqaccess > 0))
                {
                    string sqlstring = "INSERT into users ( uid, cid, access) VALUES ( '" + convertInt(uid) + "', '" + convertInt(cid) + "', '" + convertInt(reqaccess) + "' );";
                    C->SetAccess(chan, reqauth, reqaccess);
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
    if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
    {
        int access = C->GetAccess(chan, auth);
        int tmpaccess = C->GetAccess(chan, reqauth);
        string sqlstring;
        if (tmpaccess > 0)
        {
            if (tmpaccess < access || U->GetGod(nick) == 1)
            {
                int uid = U->GetUid(reqnick);
                int cid = C->GetCid(chan);
                if ((cid > -1) && (uid > -1))
                {
                    string sqlstring = "DELETE from users where uid = '" + convertInt(uid) + "' AND cid = '" + convertInt(cid) + "';";
                    C->DelAuth(chan, reqauth);
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
    if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
    {
        int oldaccess = C->GetAccess(chan, reqauth);
        int access = C->GetAccess(chan, auth);
        if (((access > reqaccess) && (access > oldaccess)) || U->GetGod(nick) == 1)
        {
            int uid = U->GetUid(reqnick);
            int cid = C->GetCid(chan);
            if ((cid > -1) && (uid > -1) && (reqaccess > 0))
            {
                string sqlstring = "UPDATE users SET access = '" + convertInt(reqaccess) + "' WHERE cid = '" + convertInt(cid) + "' AND uid = '" + convertInt(uid) + "';";
                C->SetAccess(chan, reqauth, reqaccess);
                RawSql(sqlstring);
                string sendstring = "NOTICE " + nick + " :user " + reqnick + "[" + reqauth + "] changed access to " + convertInt(reqaccess) + " access\r\n";
                Send(sendstring);
            }
        }
    }
}

void ChannelBot::op(string chan, string nick, string auth, string reqnick, string reqauth, int ca)
{
    int access = C->GetAccess(chan, auth);
    if (access >= C->GetGiveops(chan))
    {
        if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
        {
            int tmpaccess = C->GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U->GetGod(nick) == 1)
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
    int access = C->GetAccess(chan, auth);
    if (access >= C->GetGiveops(chan))
    {
        if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
        {
            int tmpaccess = C->GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U->GetGod(nick) == 1)
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
    int access = C->GetAccess(chan, auth);
    if (access >= C->GetGiveops(chan))
    {
        if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
        {
            int tmpaccess = C->GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U->GetGod(nick) == 1)
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
    int access = C->GetAccess(chan, auth);
    if (access >= C->GetGiveops(chan))
    {
        if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
        {
            int tmpaccess = C->GetAccess(chan, reqauth);
            string sqlstring;
            //if (tmpaccess > 0)
            //{
                if (tmpaccess < access || U->GetGod(nick) == 1)
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
    vector<string> auths = C->GetAuths(chan);
    sort(auths.begin(), auths.end());
    string returnstr;
    cout << auths.size() << endl;
    vector<string> sortauths;
    vector<int> sortaccess;
    for ( unsigned int i = 0 ; i < auths.size(); i++ )
    {
        int access = C->GetAccess(chan, auths[i]);
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
    if (caseInsensitiveStringCompare(reqauth,"NULL") != true)
    {
        int access = C->GetAccess(chan, auth);
        int tmpaccess = C->GetAccess(chan, reqauth);
        string sqlstring;
        //if (tmpaccess > 0)
        //{
            if (tmpaccess < access || U->GetGod(nick) == 1)
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
    int access = C->GetAccess(chan, reqauth);
    int oaccess = U->GetOaccess(reqnick);
    int god = U->GetGod(reqnick);
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
    string reply_string = "NOTICE " + nick + " :" + irc_reply("myaccess", U->GetLanguage(nick)) + "\r\n";
    reply_string = irc_reply_replace(reply_string, "$nick$", reqnick);
    reply_string = irc_reply_replace(reply_string, "$auth$", reqauth);
    Send(reply_string);
    vector<string>channels = C->GetChannels();
    for ( unsigned int i = 0 ; i < channels.size(); i++ )
    {
        int access = C->GetAccess(channels[i], reqauth);
        if (access > 0)
        {
            string returnstr = "NOTICE " + nick + " :" + channels[i] + "     " + convertInt(access) + "\r\n";
            Send(returnstr);
        }
    }
}

void ChannelBot::up(string chan, string nick, string auth, int ca)
{
    string reply_string;
    //bool giveop = false;
    //bool givevoice = false;
    int access = C->GetAccess(chan, auth);
    if (access >= C->GetGiveops(chan))
    {
        if (C->GetOp(chan, nick) == false)
        {
            string returnstring = "MODE " + chan + " +o " + nick + "\r\n";
            Send(returnstring);
            reply_string = "NOTICE " + nick + " :" + irc_reply("up_op", U->GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
        else
        {
			reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U->GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
    }
    else if (access >= C->GetGivevoice(chan))
    {
        if (C->GetVoice(chan, nick) == false)
        {
            string returnstring = "MODE " + chan + " +v " + nick + "\r\n";
            Send(returnstring);
            reply_string = "NOTICE " + nick + " :" + irc_reply("up_voice", U->GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
        else
        {
			reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U->GetLanguage(nick)) + "\r\n";
			reply_string = irc_reply_replace(reply_string, "$channel$", chan);
			Send(reply_string);
        }
    }
    if (C->GetOp(chan, nick) == true && C->GetVoice(chan, nick) == true)
    {
        reply_string = "NOTICE " + nick + " :" + irc_reply("up_already_up", U->GetLanguage(nick)) + "\r\n";
		reply_string = irc_reply_replace(reply_string, "$channel$", chan);
		Send(reply_string);
    }
}

void ChannelBot::down(string chan, string nick, string auth, int ca)
{
    //bool takeop = false;
    //bool takevoice = false;
    string reply_string;
    if (C->GetOp(chan, nick) == true && C->GetVoice(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -ov " + nick + " " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down", U->GetLanguage(nick)) + "\r\n";
    }
    else if (C->GetVoice(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -v " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_devoice", U->GetLanguage(nick)) + "\r\n";
    }
    else if (C->GetOp(chan, nick) == true)
    {
        string returnstring = "MODE " + chan + " -o " + nick + "\r\n";
        Send(returnstring);
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_deop", U->GetLanguage(nick)) + "\r\n";
    }
    if (C->GetOp(chan, nick) == false && C->GetVoice(chan, nick) == false)
    {
        reply_string = "NOTICE " + nick + " :" + irc_reply("down_already_down", U->GetLanguage(nick)) + "\r\n";
    }
    reply_string = irc_reply_replace(reply_string, "$channel$", chan);
    Send(reply_string);
}

void ChannelBot::resync(string chan, string nick, string auth, int ca)
{
    //int access = C->GetAccess(chan, auth);

            /*if (caseInsensitiveStringCompare(command,"resync"))
            {
                vector<string> nicks = C->GetNicks(chan);
                int i;
                for (i = 0 ; i < nicks.size() ; i++)
                {
                    if (!caseInsensitiveStringCompare(botnick, nicks[i]))
                    {
                        down(chan, nicks[i]);
                        up(chan, nicks[i]);
                        *//*string auth = U->GetAuth(nicks[i]);
                        int access = C->GetAccess(chan, auth);
                        if ((access < C->GetGiveops(chan)) && (access < C->GetGivevoice(chan)))
                        {
                            if (C->GetOp(chan, nicks[i]) == true || C->GetVoiceOp(chan, nicks[i]) == true)
                            {
                                string returnstring = "MODE " + chan + " -ov " + nicks[i] + " " + nicks[i] + "\r\n";
                                Send(returnstring);
                            }
                            string returnstring = "MODE " + chan + " -ov " + nicks[i] + " " + nicks[i] + "\r\n";
                            Send(returnstring);
                        }
                        else if (access < C->GetGiveops(chan))
                        {
                            string returnstring = "MODE " + chan + " -o " + nicks[i] + "\r\n";
                            Send(returnstring);
                        }
                        else if (access < C->GetGivevoice(chan))
                        {
                            string returnstring = "MODE " + chan + " -v " + nicks[i] + "\r\n";
                            Send(returnstring);
                        }*//*
                    }
                }
            }*/
}

void ChannelBot::ccommands(string nick, string auth, int ca)
{
    string returnstring;
    unsigned int length = U->GetWidth(nick);
    unsigned int amount = U->GetWidthLength(nick);
    string commandrpl = irc_reply("ccommands", U->GetLanguage(nick));
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
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
    }
    else
    {
        if (U->GetUid(nick) >= 0)
        {
            OnUserJoin(chan[0], nick);
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
    if (nick == botnick)
    {

    }
    else
    {

    }
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

vector<string> ChannelBot::lineout(vector<string> data, unsigned int rowamount, unsigned int length)
{
    vector<string> return_vector;
    string tmpvector;
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

string ChannelBot::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int ChannelBot::convertString(string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

void ChannelBot::OnUserJoin(string chan, string nick)
{
    up(chan, nick, U->GetAuth(nick), 0);
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
    vector< vector<string> > sql_result;
    string sql_string = "select channels.id, users.access, auth.auth, channels.giveops, channels.givevoice from users JOIN auth ON users.uid = auth.id JOIN channels ON users.cid = channels.id where channels.channel = '" + data + "';";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << " " << sql_result[i][3] << " " << sql_result[i][4] << endl;
        C->SetCid(data, convertString(sql_result[i][0]));
        C->AddAuth(data, sql_result[i][2]);
        C->SetAccess(data, sql_result[i][2], convertString(sql_result[i][1]));
        C->SetGiveops(data, convertString(sql_result[i][3]));
        C->SetGivevoice(data, convertString(sql_result[i][4]));
        C->SetGiveops(data, convertString(sql_result[i][3]));
        C->SetGivevoice(data, convertString(sql_result[i][4]));
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

vector< vector<string> > ChannelBot::RawSqlSelect(string data)
{
    cout << data << endl;
    database *db;
    vector< vector<string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
    delete db;
    return sql_result;
}

bool ChannelBot::RawSql(string data)
{
    cout << data << endl;
    database *db;
    db = new database();    // lol whut... connecting for each query? :'D
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    db->updateQuery( data.c_str() );
    db->disconnect();
    delete db;
    return true;
}

string ChannelBot::irc_reply(string reply_name, string reply_language)
{
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (caseInsensitiveStringCompare(reply_name, reply_name_vector[i]) && caseInsensitiveStringCompare(reply_language, reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    return "NO SUCH REPLY " + reply_name;
}

string ChannelBot::irc_reply_replace(string source_string, string search_string, string replace_string)
{
    size_t search_pos;
    search_pos = source_string.find(search_string);
    if (search_pos != string::npos)
    {
        source_string.replace(search_pos, search_string.length(), replace_string);
    }
    return source_string;
}

string ChannelBot::HostmaskToNick(vector<string> data)
{
    vector<string> who;
    who = Split(data[0],"!",true,true);
    string nick = who[0];
    who = Split(nick, ":",true,true);
    nick = who[0];
    return nick;
}

bool ChannelBot::Send(string data)
{
    ID->AddSendQueue(data);
    /*
    //cout << "bool ChannelBot::Send(string data)" << endl;
    cout << ">> " << data;
    try
    {
        S->Send(data);
    }
    catch (IrcSocket::Exception& e)
    {
        cout << "Exception caught: " << e.Description() << endl;
        return true;
    }
    if (chandebug == true)
    {
        string debugstring = "PRIVMSG " + debugchannel + " :>>" + data + "\r\n";
        try
        {
          S->Send(debugstring);
        }
        catch (IrcSocket::Exception& e)
        {
            cout << "Exception caught: " << e.Description() << endl;
            return false;
        }
    }*/
    return true;
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

bool ChannelBot::caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 ) {
    std::string str1Cpy( str1 );
    std::string str2Cpy( str2 );
    std::transform( str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower );
    std::transform( str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower );
    return ( str1Cpy == str2Cpy );
}

/// <summary>
/// Splits the string s on the given delimiter(s) and
/// returns a list of tokens without the delimiter(s)
/// </summary>
/// <param name=s>The string being split</param>
/// <param name=match>The delimiter(s) for splitting</param>
/// <param name=removeEmpty>Removes empty tokens from the list</param>
/// <param name=fullMatch>
/// True if the whole match string is a match, false
/// if any character in the match string is a match
/// </param>
/// <returns>A list of tokens</returns>

vector<string> ChannelBot::Split(const string& s, const string& match, bool removeEmpty, bool fullMatch)
{
    typedef string::size_type (string::*find_t)(const string& delim, string::size_type offset) const;
    vector<string> result;                    // return container for tokens

    string::size_type start = 0,            // starting position for searches

    skip = 1;                                // positions to skip after a match

    find_t pfind = &string::find_first_of;    // search algorithm for matches


    if (fullMatch)
    {

        // use the whole match string as a key

        // instead of individual characters

        // skip might be 0. see search loop comments

        skip = match.length();

        pfind = &string::find;

    }

    //cout << "vector:";

    while (start != string::npos)
    {

        // get a complete range [start..end)

        string::size_type end = (s.*pfind)(match, start);


        // null strings always match in string::find, but

        // a skip of 0 causes infinite loops. pretend that

        // no tokens were found and extract the whole string

        if (skip == 0) end = string::npos;


        string token = s.substr(start, end - start);


        if (!(removeEmpty && token.empty()))

        {

            // extract the token and add it to the result list

            result.push_back(token);

        }


        // start the next range

        if ((start = end) != string::npos) start += skip;

        //cout << " [" << token << "]";

    }

    //cout << "\n";


    return result;

}
