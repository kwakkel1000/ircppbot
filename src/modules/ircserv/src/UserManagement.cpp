//
//
//  @ Project : ircppbot
//  @ File Name : UserManagement.cpp
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

#include "include/UserManagement.h"

#include <interfaces/UsersInterface.h>
#include <interfaces/ChannelsInterface.h>
#include <interfaces/ConfigReaderInterface.h>
#include <core/DatabaseData.h>
#include <core/BotLib.h>
#include <core/Global.h>
#include <core/Output.h>
#include <core/Data.h>
#include <management/Whois.h>

#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <map>

const char addchar    = '+';
const char remchar    = '-';

const char gonechar   = 'G';
const char herechar   = 'H';
const char operchar   = '*';
const char botchar    = 'd';

const char ownerchar  = 'q';
const char adminchar  = 'a';
const char opchar     = 'o';
const char halfopchar = 'h';
const char voicechar  = 'v';

const char ownerwhochar  = '~';
const char adminwhochar  = '&';
const char opwhochar     = '@';
const char halfopwhochar = '%';
const char voicewhochar  = '+';

extern "C" UserManagement* create() {
    return new UserManagement;
}

extern "C" void destroy(UserManagement* x) {
    delete x;
}

UserManagement::UserManagement()
{
}

UserManagement::~UserManagement()
{
    stop();
    Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}

void UserManagement::stop()
{
    Run = false;
    mpDataInterface->stop();
    std::cout << "UserManagement::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
}

void UserManagement::Init(DataInterface* pData)
{
    GetAuths();
    mpDataInterface = pData;
    mpDataInterface->Init(true, false, false, false);
    bool bAdded = Global::Instance().get_IrcData().AddConsumer(mpDataInterface);
    Output::Instance().addStatus(bAdded, "UserManagement::Init Added Whois Consumer");
    NickServ = (Global::Instance().get_ConfigReader().GetString("nickserv") == "true");
}

void UserManagement::read()
{
    Run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&UserManagement::parse_raw, this)));


    ChannelsInterface& C = Global::Instance().get_Channels();
    int Tijd;
    time_t t;
    t = time(0);
    Tijd = t;
    int iRefreshTime = 0;
    while (Run)
    {
        iRefreshTime++;
        if (iRefreshTime >= 3600)
        {
            iRefreshTime = 0;
            std::vector< std::string > vChannels = C.GetChannels();
            for (unsigned int iChannelsIndex = 0; iChannelsIndex < vChannels.size(); iChannelsIndex++)
            {
                RefreshChannel(vChannels[iChannelsIndex]);
                usleep(5 * 100000); // 5 * 1000000 5 seconds between each channel
            }
        }
        usleep(1000000);
    }
}

void UserManagement::parse_raw()
{
    std::vector< std::string > data;
    while(Run)
    {
        data = mpDataInterface->GetRawQueue();
        ParseData(data);
    }
}

void UserManagement::ParseData(std::vector< std::string > data)
{
    if (data.size() == 3)
    {
        if (data[1] == "JOIN")      //JOIN
        {
            IrcJoin(data);
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
        if (data[1] == "307")       //WHOIS regged nick
        {
            if (NickServ)
            {
                UserAuth(data[3], data[3]);
            }
        }
        if (data[1] == "318")       //WHOIS end
        {
            EndWhois(data[3]);
            //WHOIS(data);
        }
        if (data[1] == "330")       //WHOIS auth
        {
            if (!NickServ)
            {
                UserAuth(data[3], data[4]);
            }
        }
        if (data[1] == "402")       //WHOIS no server
        {
            //WHOIS(data);
        }
        if (data[1] == "354")       //WHO (extra)
        {
            if (Global::Instance().get_ConfigReader().GetString("whoextra") == "true")
            {
                WHOEXTRA(data);
            }
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

void UserManagement::WHO(std::vector< std::string > data)
{
    std::string chan = data[3];
    std::string nick = data[7];
    std::string modes = data[8];

    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    C.AddNick(chan, nick);
    bool added = U.AddUser(nick);
    U.FirstJoin(nick);
    U.AddChannel(nick, chan);
    GetChannelInfo(chan);

    size_t Gonepos = modes.find(gonechar);
    if (Gonepos != std::string::npos)
    {
        U.SetGone(nick, true);
    }

    size_t Herepos = modes.find(herechar);
    if (Herepos != std::string::npos)
    {
        U.SetGone(nick, false);
    }

    size_t Ownerpos = modes.find(ownerwhochar);
    if (Ownerpos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Adminpos = modes.find(adminwhochar);
    if (Adminpos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Oppos = modes.find(opwhochar);
    if (Oppos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Halfoppos = modes.find(halfopwhochar);
    if (Halfoppos != std::string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Voicepos = modes.find(voicewhochar);
    if (Voicepos != std::string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Xpos = modes.find("x");
    if (Xpos != std::string::npos)
    {
        U.SetX(nick, true);
    }

    size_t Dpos = modes.find(botchar);
    if (Dpos != std::string::npos)
    {
        U.SetD(nick, true);
    }

    size_t Ircoppos = modes.find(operchar);
    if (Ircoppos != std::string::npos)
    {
        U.SetIrcop(nick, true);
    }

    if (added)
    {
        std::string whoisstring = "WHOIS " + nick + "\r\n";
        Send(whoisstring);
    }
}

void UserManagement::WHOEXTRA(std::vector< std::string > data)
{
    if (data.size() == 7)
    {
        std::string chan = data[3];
        std::string nick = data[4];
        std::string modes = data[5];
        std::string auth = data[6];
        size_t chanpos;
        chanpos = chan.find("#");
        if (chanpos != std::string::npos)
        {
            UsersInterface& U = Global::Instance().get_Users();
            ChannelsInterface& C = Global::Instance().get_Channels();
            C.AddNick(chan, nick);
            U.AddUser(nick);
            U.AddChannel(nick, chan);
            U.FirstJoin(nick);
            UserAuth(nick, auth);
            GetChannelInfo(chan);

            size_t Gonepos = modes.find(gonechar);
            if (Gonepos != std::string::npos)
            {
                U.SetGone(nick, true);
            }

            size_t Herepos = modes.find(herechar);
            if (Herepos != std::string::npos)
            {
                U.SetGone(nick, false);
            }

            size_t Ownerpos = modes.find(ownerwhochar);
            if (Ownerpos != std::string::npos)
            {
                C.SetOp(chan, nick, true);
            }

            size_t Adminpos = modes.find(adminwhochar);
            if (Adminpos != std::string::npos)
            {
                C.SetOp(chan, nick, true);
            }

            size_t Oppos = modes.find(opwhochar);
            if (Oppos != std::string::npos)
            {
                C.SetOp(chan, nick, true);
            }

            size_t Halfoppos = modes.find(halfopwhochar);
            if (Halfoppos != std::string::npos)
            {
                C.SetVoice(chan, nick, true);
            }

            size_t Voicepos = modes.find(voicewhochar);
            if (Voicepos != std::string::npos)
            {
                C.SetVoice(chan, nick, true);
            }

            size_t Xpos = modes.find("x");
            if (Xpos != std::string::npos)
            {
                U.SetX(nick, true);
            }

            size_t Dpos = modes.find(botchar);
            if (Dpos != std::string::npos)
            {
                U.SetD(nick, true);
            }

            size_t Ircoppos = modes.find(operchar);
            if (Ircoppos != std::string::npos)
            {
                U.SetIrcop(nick, true);
            }
        }
    }
}

void UserManagement::IrcJoin(std::vector< std::string > mvRawData)
{
    UsersInterface& _Users = Global::Instance().get_Users();
    ChannelsInterface& _Channels = Global::Instance().get_Channels();
    std::string _sChannel = mvRawData[2];
    boost::erase_all(_sChannel, ":");
    std::string _sNick = BotLib::HostmaskToNick(mvRawData);
    if (_sNick == Global::Instance().get_BotNick())
    {
        _Channels.AddChannel(_sChannel);
        std::string _sWhoString;
        if (Global::Instance().get_ConfigReader().GetString("whoextra") == "true")
        {
             _sWhoString = "WHO " + _sChannel + " %ncaf\r\n";
        }
        else
        {
            _sWhoString = "WHO " + _sChannel + "\r\n";
        }
        Send(_sWhoString);
        GetChannelInfo(_sChannel);
    }
    else
    {
        _Channels.AddNick(_sChannel, _sNick);
        bool _bAdded = false;
        _bAdded = _Users.AddUser(_sNick);
        _Users.AddChannel(_sNick, _sChannel);
        if (_bAdded)
        {
            Send(Global::Instance().get_Reply().irc_whois(_sNick));
        }
        if (_Users.GetAuth(_sNick) != "NULL")
        {
            /*std::string outputString = "WhoisUsers insert:  nick " + _sNick + " channel " + _sChannel;
            Output::Instance().addOutput(outputString, 4);*/
            //WhoisUsers.insert( std::pair< std::string, std::string >(_sNick, _sChannel) );
            Whois::Instance().AddQueue(std::pair< std::string, std::string >(_sNick, _sChannel));
        }
        else
        {
            /*std::string outputString = "NoWhoisUsers insert:  nick " + _sNick + " channel " + _sChannel;
            Output::Instance().addOutput(outputString, 4);*/
            NoWhoisUsers.insert( std::pair< std::string, std::string >(_sNick, _sChannel) );
        }
    }
}

void UserManagement::PART(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string chan = data[2];
    boost::erase_all(chan, ":");
    //vector<string> chan = Split(data[2], ":",true,true);
    std::string nick = BotLib::HostmaskToNick(data);
    // if nick == botnick delete channel
    if (nick == Global::Instance().get_BotNick())
    {
        std::vector< std::string > chanusers = C.GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            U.DelChannel(chanusers[i], chan);
            C.DelNick(chan, chanusers[i]);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                std::string outputString = "no channels left " + chanusers[i];
                Output::Instance().addOutput(outputString, 4);
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan);
    }
    // if not this bot, delete nick from channel, delete channel from nick
    else
    {
        C.DelNick(chan, nick);
        U.DelChannel(nick, chan);
        // if parting last channel, delete user.
        if (U.GetChannels(nick)[0] == "NULL")
        {
            std::string outputString = "no channels left";
            Output::Instance().addOutput(outputString, 4);
            U.DelUser(nick);
        }
        //cout << "PART" << endl;
    }
}

void UserManagement::KICK(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string chan = data[2];
    std::string nick = data[3];
    if (nick == Global::Instance().get_BotNick())
    {
        std::vector< std::string > chanusers = C.GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            C.DelNick(chan, chanusers[i]);
            U.DelChannel(chanusers[i], chan);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                std::cout << "no channels left" << std::endl;
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan);
    }
    else
    {
        C.DelNick(chan, nick);
        U.DelChannel(nick, chan);
        if (U.GetChannels(nick)[0] == "NULL")
        {
            std::cout << "no channels left" << std::endl;
            U.DelUser(nick);
        }
        //cout << "KICK" << endl;
    }
}

void UserManagement::MODE(std::vector< std::string > data)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string nick = BotLib::HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {

    }
    else
    {

    }
    bool add = false;
    int parsepos = 4;
    std::string modeparse = data[3];
    for (unsigned int i = 0; i < modeparse.length(); i++)
    {
        //cout << "modeparse[i]" << modeparse[i] << endl;
        if (modeparse[i] == addchar)
        {
            //cout << "+" << endl;
            add = true;
        }
        if (modeparse[i] == remchar)
        {
            //cout << "-" << endl;
            add = false;
        }
        if (modeparse[i] == ownerchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << ownerchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == adminchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << adminchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == opchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << opchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == halfopchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << halfopchar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == voicechar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << voicechar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
    }
}

void UserManagement::QUIT(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string nick = BotLib::HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        /*U.~Users();
        C.~Channels();*/
    }
    else
    {
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], nick);
        }
        U.DelUser(nick);
    }
}

void UserManagement::NICK(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string oldnick = BotLib::HostmaskToNick(data);
    std::string nick = data[2];
    boost::erase_all(nick, ":");
    //vector<string> nick = Split(data[2], ":",true,true);
    if (oldnick == Global::Instance().get_BotNick())
    {
        Global::Instance().set_BotNick(nick);
        //botnick = nick[0];
        U.ChangeNick(oldnick, nick);
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick);
        }
        //cout << "NICK" << endl;
    }
    else
    {
        U.ChangeNick(oldnick, nick);
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick);
        }
        //cout << "NICK" << endl;
    }
}

void UserManagement::UserAuth(std::string msNick, std::string msAuth)
{
    UsersInterface& U = Global::Instance().get_Users();
    U.SetAuth(msNick, msAuth);
    U.SetOaccess(msNick, -1);
    if (U.AddAuth(msAuth) == true)
    {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        std::stringstream ss;
        ss << uuid;
        std::string UserUuid = ss.str();
        std::string outputString;
        outputString = "UserUuid " + UserUuid;
        Output::Instance().addOutput(outputString, 4);
        // std::cout << "UserUuid: " << UserUuid << std::endl;
        DatabaseData::Instance().AddAuth(UserUuid, msAuth);
    }
    std::vector< std::string > userchannels = U.GetChannels(msNick);
    if (boost::iequals(userchannels[0], "NULL") == false)
    {
        GetUserInfo(msNick);

        std::multimap< std::string, std::string>::iterator it;
        for ( it=NoWhoisUsers.begin() ; it != NoWhoisUsers.end(); it++ )
        {
            if ((*it).first == msNick)
            {
                Whois::Instance().AddQueue(std::pair< std::string, std::string >((*it).first, (*it).second));
                std::string outputString;
                outputString = "user " + (*it).first + " channel " + (*it).second;
                Output::Instance().addOutput(outputString, 4);
            }
        }
        NoWhoisUsers.erase (msNick);
    }
    else
    {
        U.DelUser(msNick);
    }
}

void UserManagement::EndWhois(std::string msNick)
{
    NoWhoisUsers.erase (msNick);
    /*std::vector< std::string > userchannels = U.GetChannels(mNick);
    if (boost::iequals(userchannels[0], "NULL"))
    {
        //U.DelUser(mNick);
    }*/
}

bool UserManagement::Send(std::string data)
{
    Global::Instance().get_IrcData().AddLowPrioritySendQueue(data);
    return true;
}

void UserManagement::GetUserInfo(std::string data)
{
    UsersInterface& U = Global::Instance().get_Users();
    std::string auth = U.GetAuth(data);
    if (boost::iequals(auth, "NULL") != true)
    {
        //U.SetUid(data, DatabaseData::Instance().GetUserUuidByAuth(auth));
        U.SetOaccess(data, DatabaseData::Instance().GetOaccessByAuth(auth));
        U.SetGod(data, DatabaseData::Instance().GetGodByAuth(auth));
        U.SetLanguage(data, DatabaseData::Instance().GetLanguageByAuth(auth));
    }
}

void UserManagement::GetChannelInfo(std::string msChannel)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    C.InitSetting(msChannel, "giveops", BotLib::StringFromInt(DatabaseData::Instance().GetGiveOpsByChannel(msChannel)));
    C.InitSetting(msChannel, "givevoice", BotLib::StringFromInt(DatabaseData::Instance().GetGiveVoiceByChannel(msChannel)));
    C.SetCid(msChannel, DatabaseData::Instance().GetChannelUuidByChannel(msChannel));
    /*C.SetGiveops(msChannel, DatabaseData::Instance().GetGiveOpsByChannel(msChannel));
    C.SetGivevoice(msChannel, DatabaseData::Instance().GetGiveVoiceByChannel(msChannel));*/

    std::vector< std::vector< std::string > > channels_vector;
    std::string ChannelUuid = DatabaseData::Instance().GetChannelUuidByChannel(msChannel);
    channels_vector = DatabaseData::Instance().GetUserUuidAndAccessByChannelUuid(ChannelUuid);
    unsigned int i;
    for (i = 0 ; i < channels_vector.size() ; i++)
    {
        std::string auth = DatabaseData::Instance().GetAuthByUserUuid(channels_vector[i][0]);
        //std::cout << msChannel << " " << auth << " " << channels_vector[i][1] << std::endl;
        C.AddAuth(msChannel, auth);

        C.SetAccess(msChannel, auth, BotLib::IntFromString(channels_vector[i][1]));
    }
}

void UserManagement::GetAuths()
{
    UsersInterface& U = Global::Instance().get_Users();
    std::vector< std::string > auth_vector;
    auth_vector = DatabaseData::Instance().GetAuths();
    unsigned int i;
    for (i = 0; i < auth_vector.size(); i++)
    {
        U.AddAuth(auth_vector[i]);
        //std::cout << auth_vector[i] << std::endl;
    }
}

void UserManagement::RefreshChannel(std::string sChannel)
{
        std::string whostring;
        if (Global::Instance().get_ConfigReader().GetString("whoextra") == "true")
        {
             whostring = "WHO " + sChannel + " %ncaf\r\n";
        }
        else
        {
            whostring = "WHO " + sChannel + "\r\n";
        }
        Send(whostring);
}
