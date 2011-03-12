#include "include/OCommands.h"
#include "../../include/core/Global.h"
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

extern "C" ModuleInterface* create()
{
    return new OCommands;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

OCommands::OCommands()
{
}

OCommands::~OCommands()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(D);
    delete D;
}

void OCommands::Init()
{
    ocommandstrigger = Global::Instance().get_ConfigReader().GetString("ocommandstrigger");
    D = new Data();
    D->Init(false, false, false, true);
    Global::Instance().get_IrcData().AddConsumer(D);
    BindInit();
}

void OCommands::BindInit()
{
    commands.clear();
    binds.clear();
    oas.clear();

    ccommands.clear();
    cbinds.clear();
    cas.clear();

    vector< vector<string> > sql_result;
    string sql_string = "select command, bind, oaccess from ocommands";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        commands.push_back(sql_result[i][0]);
        binds.push_back(sql_result[i][1]);
        oas.push_back(convertString(sql_result[i][2]));
    }

    sql_string = "select command, bind, caccess from commands";
    sql_result = RawSqlSelect(sql_string);
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        ccommands.push_back(sql_result[i][0]);
        cbinds.push_back(sql_result[i][1]);
        cas.push_back(convertString(sql_result[i][2]));
    }
}

void OCommands::stop()
{
    run = false;
    D->stop();
    std::cout << "OCommands::stop" << std::endl;
    privmsg_parse_thread->join();
    std::cout << "privmsg_parse_thread stopped" << std::endl;
}

void OCommands::read()
{
    run = true;
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&OCommands::parse_privmsg, this)));
}

void OCommands::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        PRIVMSG(data, ocommandstrigger);
    }
}

void OCommands::ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger)
{
    cout << "OCommands" << endl;
    UsersInterface& U = Global::Instance().get_Users();
    string auth = U.GetAuth(nick);
    if (args.size() == 0)
    {
        if (boost::iequals(command,"debug"))
        {
            U.Debug();
            //C->Debug();
        }

        if (U.GetGod(nick) == 1)
        {
            for (unsigned int i = 0; i < binds.size(); i++)
            {
                if (boost::iequals(command, binds[i]))
                {
                    if (boost::iequals(commands[i], "ocommands"))
                    {
                        ocommands(nick, auth, oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                }
            }
        }
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "god"))
                {
                    overwatch(commands[i], command, chan, nick, auth, args);
                    god(nick, auth, oas[i]);
                }
                if (boost::iequals(commands[i], "ousers"))
                {
                    ousers(nick, oas[i]);
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
                if (boost::iequals(commands[i], "delouser"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        delouser(nick, auth, args[0], U.GetAuth(args[0]), oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "delchannel"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        delchannel(chan, nick, auth, args[0], oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
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
    if (args.size() >= 1)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "say"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        string saystring;
                        for (unsigned int j = 0; j < args.size()-1; j++)
                        {
                            saystring = saystring + args[j] + " ";
                        }
                        if (args.size() > 0)
                        {
                            saystring = saystring + args[args.size()-1];
                        }
                        say(chan, nick, auth, saystring, oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "raw"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        string dostring;
                        for (unsigned int j = 0; j < args.size()-1; j++)
                        {
                            dostring = dostring + args[j] + " ";
                        }
                        if (args.size() > 0)
                        {
                            dostring = dostring + args[args.size()-1];
                        }
                        raw(nick, auth, dostring, oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
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
    if (args.size() == 2)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "addouser"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        addouser(nick, auth, args[0], U.GetAuth(args[0]), convertString(args[1]), oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "changeolevel"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        changeolevel(nick, auth, args[0], U.GetAuth(args[0]), convertString(args[1]),oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "delobind"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        delobind(nick, auth, args[0], args[1], oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "delbind"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        delbind(nick, auth, args[0], args[1], oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "addchannel"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        addchannel(chan, nick, auth, args[0], args[1], U.GetAuth(args[1]), oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
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
    if (args.size() == 3)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(command, binds[i]))
            {
                if (boost::iequals(commands[i], "addobind"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        addobind(nick, auth, args[0], args[1], convertString(args[2]), oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                    else
                    {
                        string returnstring = "NOTICE " + nick + " :" + irc_reply("need_god", U.GetLanguage(nick)) + "\r\n";
                        Send(returnstring);
                    }
                }
                if (boost::iequals(commands[i], "addbind"))
                {
                    if (U.GetGod(nick) == 1)
                    {
                        addbind(nick, auth, args[0], args[1], convertString(args[2]), oas[i]);
                        overwatch(commands[i], command, chan, nick, auth, args);
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
    if (args.size() >= 3)
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
                        simulate(nick, auth, chan, args[0], args[1], simulate_args, oas[i]);
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

void OCommands::god(string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (U.GetGod(nick) > 0)
    {
        vector<string> nicks = U.GetNicks(auth);
        for (unsigned int i = 0; i < nicks.size(); i++)
        {
            if (nicks[i] != "NULL")
            {
                U.SetGod(nicks[i], 0);
            }
        }
        string sqlstring = "UPDATE auth SET god = '0' WHERE auth = '" + auth + "';";!
        RawSql(sqlstring);
        returnstring = "NOTICE " + nick + " :" + irc_reply("god_disabled", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
    	int oaccess = U.GetOaccess(nick);
        if (oaccess >= oa)
        {
            vector<string> nicks = U.GetNicks(auth);
            for (unsigned int i = 0; i < nicks.size(); i++)
            {
                if (nicks[i] != "NULL")
                {
                    U.SetGod(nicks[i], 1);
                }
            }
            string sqlstring = "UPDATE auth SET god = '1' WHERE auth = '" + auth + "';";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("god_enabled", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::say(string chan, string nick, string auth, string saystring, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        string returnstr = "PRIVMSG " + chan + " :" + saystring + "\r\n";
        Send(returnstr);
        returnstring = "NOTICE " + nick + " :" + irc_reply("say", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::raw(string nick, string auth, string dostring, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        string returnstr = dostring + "\r\n";
        Send(returnstr);
        returnstring = "NOTICE " + nick + " :" + irc_reply("raw", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addchannel(string chan, string nick, string auth, string reqchan, string reqnick, string reqauth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int oaccess = U.GetOaccess(nick);
        cout << convertInt(oaccess) << endl;
        if (oaccess >= oa)
        {
            if (C.GetCid(reqchan) == -1)
            {
                string sqlstring = "INSERT into channels ( channel ) VALUES ( '" + reqchan + "' );";
                RawSql(sqlstring);
                int uid = U.GetUid(reqnick);
                int cid = -1;// = C->GetCid(reqchan);
                vector< vector<string> > sql_result;
                string sql_string = "select channels.id from channels where channels.channel = '" + reqchan + "';";
                sql_result = RawSqlSelect(sql_string);
                unsigned int i;
                for (i = 0 ; i < sql_result.size() ; i++)
                {
                    cid = convertString(sql_result[i][0]);
                }
                if ((cid > -1) && (uid > -1))
                {
                    string sqlstring = "INSERT into users ( uid, cid, access) VALUES ( '" + convertInt(uid) + "', '" + convertInt(cid) + "', '" + convertInt(500) + "' );";
                    RawSql(sqlstring);
                }
                string joinstr = "JOIN " + reqchan + "\r\n";
                Send(joinstr);
                C.AddChannel(reqchan);
                string whostring = "WHO " + reqchan + "\r\n";
                Send(whostring);
                returnstring = "NOTICE " + nick + " :" + irc_reply("addchannel", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$nick$", nick);
                returnstring = irc_reply_replace(returnstring, "$auth$", auth);
                returnstring = irc_reply_replace(returnstring, "$regnick$", reqnick);
                returnstring = irc_reply_replace(returnstring, "$regauth$", reqchan);
                returnstring = irc_reply_replace(returnstring, "$channel$", reqchan);
                Send(returnstring);
            }
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::delchannel(string chan, string nick, string auth, string reqchan, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        int cid = C.GetCid(reqchan);
        string sqlstring;
        if (cid > -1)
        {
            sqlstring = "DELETE from users where cid = '" + convertInt(cid) + "';";
            RawSql(sqlstring);
            sqlstring = "DELETE from channels where id = '" + convertInt(cid) + "';";
            RawSql(sqlstring);
            vector<string> auths = C.GetAuths(reqchan);
            for (unsigned int i = 0; i < auths.size(); i++)
            {
                C.DelAuth(reqchan, auths[0]);
            }
            vector<string> nicks = C.GetNicks(reqchan);
            for (unsigned int i = 0; i < auths.size(); i++)
            {
                U.DelChannel(nicks[0], reqchan);
            }
            C.DelChannel(reqchan);
            string partstr = "PART " + reqchan + "\r\n";
            Send(partstr);
            returnstring = "NOTICE " + nick + " :" + irc_reply("delchannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", reqchan);
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delchannel_nochannel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$channel$", reqchan);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addobind(string nick, string auth, string command, string newbind, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    bool exists = false;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(binds[i], newbind) == true)
            {
                exists = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("addobind_exists", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (exists == false)
        {
            commands.push_back(command);
            binds.push_back(newbind);
            oas.push_back(reqaccess);
            string sqlstring = "INSERT into ocommands ( command, bind, oaccess) VALUES ( '" + command + "', '" + newbind + "', '" + convertInt(reqaccess) + "' );";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("addobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::delobind(string nick, string auth, string command, string bind, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        bool deleted = false;
        for (unsigned int i = 0; i < binds.size(); i++)
        {
            if (boost::iequals(binds[i], bind) == true && boost::iequals(commands[i], command) == true)
            {
                commands.erase(commands.begin()+i);
                binds.erase(binds.begin()+i);
                oas.erase(oas.begin()+i);
                string sqlstring = "DELETE from ocommands where bind = '" + bind + "' AND command = '" + command + "';";
                RawSql(sqlstring);
                deleted = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("delobind", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", bind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (!deleted)
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delobind_nobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", bind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addbind(string nick, string auth, string command, string newbind, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    bool exists = false;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        for (unsigned int i = 0; i < cbinds.size(); i++)
        {
            if (boost::iequals(cbinds[i], newbind) == true)
            {
                exists = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("addbind_exists", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (exists == false)
        {
            ccommands.push_back(command);
            cbinds.push_back(newbind);
            cas.push_back(reqaccess);
            string sqlstring = "INSERT into commands ( command, bind, caccess) VALUES ( '" + command + "', '" + newbind + "', '" + convertInt(reqaccess) + "' );";
            RawSql(sqlstring);
            returnstring = "NOTICE " + nick + " :" + irc_reply("addbind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", newbind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::delbind(string nick, string auth, string command, string bind, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    int oaccess = U.GetOaccess(nick);
    cout << convertInt(oaccess) << endl;
    if (oaccess >= oa)
    {
        bool deleted = false;
        for (unsigned int i = 0; i < cbinds.size(); i++)
        {
            if (boost::iequals(cbinds[i], bind) == true && boost::iequals(ccommands[i], command) == true)
            {
                ccommands.erase(ccommands.begin()+i);
                cbinds.erase(cbinds.begin()+i);
                cas.erase(cas.begin()+i);
                string sqlstring = "DELETE from commands where bind = '" + bind + "' AND command = '" + command + "';";
                RawSql(sqlstring);
                deleted = true;
                returnstring = "NOTICE " + nick + " :" + irc_reply("delbind", U.GetLanguage(nick)) + "\r\n";
                returnstring = irc_reply_replace(returnstring, "$bind$", bind);
                returnstring = irc_reply_replace(returnstring, "$command$", command);
                Send(returnstring);
            }
        }
        if (!deleted)
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("delbind_nobind", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$bind$", bind);
            returnstring = irc_reply_replace(returnstring, "$command$", command);
            Send(returnstring);
        }
    }
    else
    {
        returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
        Send(returnstring);
    }
}

void OCommands::addouser(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa)
{/*
    if (boost::iequals(reqauth,"NULL") != true)
    {
        if (U.GetOaccess(nick) >= addousera)
        {

        }
        int access = C->GetAccess(chan, auth);
        if (access > reqaccess || U.GetGod(nick) == 1)
        {
            if (C->AddAuth(chan, reqauth))
            {
                int uid = U.GetUid(reqnick);
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
    }*/
}

void OCommands::delouser(string nick, string auth, string reqnick, string reqauth, int oa)
{/*
    if (boost::iequals(reqauth,"NULL") != true)
    {
        int access = C->GetAccess(chan, auth);
        int tmpaccess = C->GetAccess(chan, reqauth);
        string sqlstring;
        if (tmpaccess > 0)
        {
            if (tmpaccess < access || U.GetGod(nick) == 1)
            {
                int uid = U.GetUid(reqnick);
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
    }*/
}

void OCommands::changeolevel(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (boost::iequals(reqauth,"NULL") != true)
    {
        if (U.GetOaccess(nick) >= oa)
        {
            string sqlstring = "UPDATE auth SET oaccess = '" + convertInt(reqaccess) + "' WHERE auth = '" + reqauth + "';";
            RawSql(sqlstring);
            vector<string> nicks = U.GetNicks(reqauth);
            for (unsigned int i = 0; i < nicks.size(); i++)
            {
                if (nicks[i] != "NULL")
                {
                    U.SetOaccess(nicks[i], reqaccess);
                    if (reqaccess < 1)
                    {
                        U.SetGod(nicks[i], 0);
                        string sqlstring = "UPDATE auth SET god = '0' WHERE auth = '" + reqauth + "';";
                        RawSql(sqlstring);
                    }
                }
            }
            returnstring = "NOTICE " + nick + " :" + irc_reply("changeolevel", U.GetLanguage(nick)) + "\r\n";
            returnstring = irc_reply_replace(returnstring, "$nick$", reqnick);
            returnstring = irc_reply_replace(returnstring, "$auth$", reqauth);
            returnstring = irc_reply_replace(returnstring, "$access$", convertInt(reqaccess));
            Send(returnstring);
        }
        else
        {
            returnstring = "NOTICE " + nick + " :" + irc_reply("need_oaccess", U.GetLanguage(nick)) + "\r\n";
            Send(returnstring);
        }
    }
}

void OCommands::ousers(string nick, int oa)
{
    /*
    vector<string> auths = C->GetAuths(chan);
    string returnstr;
    cout << auths.size() << endl;
    for ( unsigned int i = 0 ; i < auths.size(); i++ )
    {
        int access = C->GetAccess(chan, auths[i]);
        if (access > 0)
        {
            returnstr = "NOTICE " + nick + " :" + convertInt(access) + "    " + auths[i] + "\r\n";
            Send(returnstr);
        }
    }*/
}

void OCommands::ocommands(string nick, string auth, int oa)
{
    UsersInterface& U = Global::Instance().get_Users();
    string returnstring;
    if (boost::iequals(auth,"NULL") != true)
    {
        if (U.GetOaccess(nick) >= oa)
        {
            unsigned int length = U.GetWidth(nick);
            unsigned int amount = U.GetWidthLength(nick);
            string commandrpl = irc_reply("ocommands", U.GetLanguage(nick));
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
    }
}



void OCommands::timerrun()
{
    //cout << "OCommands::timerrun()" << endl;
}
