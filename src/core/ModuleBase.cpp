//
//
//  @ Project : ircppbot
//  @ File Name : ModuleBase.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "../include/core/ModuleBase.h"

#include <cstring>
#include <string>
#include <vector>

#include "../include/core/Global.h"
#include "../include/core/Output.h"
#include "../include/core/Database.h"


// public
ModuleBase::ModuleBase()
{
        bParseRaw = false;
        bParseMode = false;
        bParseWhois = false;
        bParsePrivmsg = false;
}


// protected
// irc

void ModuleBase::overwatch(std::string bind, std::string command, std::string chan, std::string nick, std::string auth, std::vector< std::string > args)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    UsersInterface& U = Global::Instance().get_Users();
    ConfigReaderInterface& CR = Global::Instance().get_ConfigReader();
    std::string overwatchchannel = CR.GetString("overwatchchannel");
    std::string debugstring = "[" + Output::Instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] [" + nick + ":" + auth + "] [" + chan + ":" + convertInt(C.GetAccess(chan, auth)) + "] ";
    if (U.GetGod(nick) == 1)
    {
        debugstring = debugstring + "[G] ";
    }
    debugstring = debugstring + command + ":" + bind;
    for (unsigned int i = 0; i < args.size(); i++)
    {
        debugstring = debugstring + " " + args[i];
    }
    SendLowPriority(Global::Instance().get_Reply().irc_privmsg(overwatchchannel, debugstring));
}


void ModuleBase::PRIVMSG(std::vector< std::string > data, std::string trigger)
{
    // cout << "PRIVMSG" << endl;
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
        triggertype = 1;    // PRIVMSG ... :!;
        if (data3 != "")
        {
            if (chanpos2 != std::string::npos && chanpos3 == std::string::npos)    // chanpos1 yes/no both valid
            {
                chantrigger = 1;    // PRIVMSG nick #chan :!#chan command   ||   PRIVMSG nick bot :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    // boost::erase_all(chan, trigger);
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
                chantrigger = 0;    // PRIVMSG nick #chan :!command
                // cout << "channel: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    command = data3;
                    // boost::erase_all(command, trigger);
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
                chantrigger = -1;   // PRIVMSG nick bot :!command
                if (data.size() >= 4)
                {
                    command = data3;
                    chan = "NULL";
                    // boost::erase_all(command, trigger);
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
            else if (chanpos2 == std::string::npos && chanpos3 != std::string::npos)    // chanpos1 yes/no both valid
            {
                chantrigger = 0;    // PRIVMSG nick #chan :!command #chan    ||      PRIVMSG nick bot :!command #chan
                if (data.size() >= 5)
                {
                    command = data3;
                    // boost::erase_all(command, trigger);
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
                chantrigger = 1;   // PRIVMSG nick bot :#chan command
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
                chantrigger = 0;   // PRIVMSG nick bot :command #chan
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
                chantrigger = -1;   // PRIVMSG nick bot :command
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
