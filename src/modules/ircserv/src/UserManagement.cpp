//
//
//  @ Project : ircppbot
//  @ File Name : management.cpp
//  @ Date : 27-12-2012
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2012 Gijs Kwakkel
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

#include "include/management/management.h"

#include "include/management/channels.h"
#include "include/management/users.h"
#include "include/irc.h"
#include "include/reply.h"

#include <gframe/glib.h>


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

extern "C" management* create() {
    return new management;
}

extern "C" void destroy(management* x) {
    delete x;
}

management::management() :
    m_NickServer(false),
    m_Run(false),
    m_WhoExtra(false),
    m_IrcData(NULL)
{
}

management::~management()
{
    stop();
    irc::instance().delConsumer(m_IrcData);
    delete m_IrcData;
}

void management::stop()
{
    m_Run = false;
    m_IrcData->stop();
    output::instance().addOutput("management::stop", 6);
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
}

void management::init(ircdata* ircData)
{
    m_NickServer = (configreader::instance().getString("nickserv") == "true");
    m_WhoExtra = (configreader::instance().getString("whoextra") == "true");
    GetAuths();
    m_IrcData = ircData;
    m_IrcData->addModesQueue(true);
    m_IrcData->addWhoisQueue(true);
    m_IrcData->addEventsQueue(true);
    irc::instance().addConsumer(m_IrcData);
}

void management::read()
{
    m_Run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&management::parse_raw, this)));


    ChannelsInterface& C = Global::Instance().get_Channels();
    int Tijd;
    time_t t;
    t = time(0);
    Tijd = t;
    int iRefreshTime = 0;
    while (m_Run)
    {
        iRefreshTime++;
        if (iRefreshTime >= 3600)
        {
            iRefreshTime = 0;
            std::vector< std::string > vChannels = C.GetChannels();
            for (size_t iChannelsIndex = 0; iChannelsIndex < vChannels.size(); iChannelsIndex++)
            {
                RefreshChannel(vChannels[iChannelsIndex]);
                usleep(5 * 100000); // 5 * 1000000 5 seconds between each channel
            }
        }
        usleep(1000000);
    }
}

void management::parse_raw()
{
    std::vector< std::string > data;
    while(m_Run)
    {
        data = m_IrcData->GetRawQueue();
        ParseData(data);
    }
}

void management::ParseData(std::vector< std::string > data)
{
    if (data.size() == 3)
    {
        if (data[1] == "JOIN")      //JOIN
        {
            join(data);
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
            quit(data);
        }
        if (data[1] == "PART")      //PART
        {
            part(data);
        }
    }
    if (data.size() >= 4)
    {
        if (data[1] == "001")   //welcome
        {
            Global::Instance().set_BotNick(data[2]);
        }
        if (data[1] == "307")       //WHOIS regged userName
        {
            if (m_NickServer)
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
            if (!m_NickServer)
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
            if (m_WhoExtra)
            {
                WHOEXTRA(data);
            }
        }
    }
    if (data.size() >= 5)
    {
        if (data[1] == "KICK")      //KICK
        {
            kick(data);
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

void management::WHO(std::vector< std::string > data)
{
    std::string channelName = data[3];
    std::string userName = data[7];
    std::string modes = data[8];

    channels::instance().getChannel(channelName).addUser(userName);
    bool added = users::instance().addUser(userName);
    //U.FirstJoin(userName);
    users::instance().getUser(userName).addChannel(channelName);
    GetChannelInfo(channelName);

    userModes(userName, modes);
    channelUserModes(channelName, userName, modes);


    if (added)
    {
        irc::instance().addSendQueue(reply::instance().ircWhois(userName));
    }
}

void management::WHOEXTRA(std::vector< std::string > data)
{
    if (data.size() == 7)
    {
        std::string channelName = data[3];
        std::string userName = data[4];
        std::string modes = data[5];
        std::string auth = data[6];
        size_t chanpos;
        chanpos = channelName.find("#");
        if (chanpos != std::string::npos)
        {
            UsersInterface& U = Global::Instance().get_Users();
            ChannelsInterface& C = Global::Instance().get_Channels();
            C.AddNick(channelName, userName);
            U.AddUser(userName);
            U.AddChannel(userName, channelName);
            U.FirstJoin(userName);
            UserAuth(userName, auth);
            GetChannelInfo(channelName);

            userModes(userName, modes);
            channelUserModes(channelName, userName, modes);
        }
    }
}

void management::JOIN(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string channelName = data[2];
    boost::erase_all(channelName, ":");
    //vector<string> channelName = Split(data[2], ":",true,true);
    std::string userName = HostmaskToNick(data);
    if (userName == Global::Instance().get_BotNick())
    {
        C.AddChannel(channelName);
        whoChannel(channelName);
        GetChannelInfo(channelName);
    }
    else
    {
        C.AddNick(channelName, userName);
        bool added = false;
        added = U.AddUser(userName);
        U.AddChannel(userName, channelName);
        if (added)
        {
            irc::instance().addSendQueue(reply::instance().ircWhois(userName));
        }
        if (U.GetAuth(userName) != "NULL")
        {
            std::string outputString = "WhoisUsers insert:  userName " + userName + " channel " + channelName;
            Output::Instance().addOutput(outputString, 4);
            //WhoisUsers.insert( std::pair< std::string, std::string >(userName, channelName) );
            Whois::Instance().AddQueue(std::pair< std::string, std::string >(userName, channelName));
        }
        else
        {
            std::string outputString = "NoWhoisUsers insert:  userName " + userName + " channel " + channelName;
            Output::Instance().addOutput(outputString, 4);
            NoWhoisUsers.insert( std::pair< std::string, std::string >(userName, channelName) );
        }
    }
}

void management::part(std::vector< std::string > eventData)
{
    std::string channelName = eventData[2];
    boost::erase_all(channelName, ":");
    std::string userName = HostmaskToNick(eventData);
    leaveChannel(channelName, userName);
}

void management::kick(std::vector< std::string > eventData)
{
    std::string channelName = eventData[2];
    std::string userName = eventData[3];
    leaveChannel(channelName, userName);
}

void management::quit(std::vector< std::string > eventData)
{
    std::string userName = HostmaskToNick(eventData);
    if (userName == Global::Instance().get_BotNick())
    {
        // should never see its own quit :S
        exit(EXIT_FAILURE);
    }
    else
    {
        std::unordered_set< std::string > userChannels = users::instance().getUser(userName).getChannels();
        std::unordered_set< std::string >::iterator userChannelsIterator;
        for (userChannelsIterator = userChannels.begin(); userChannelsIterator != channelUsers.end(); ++userChannelsIterator)
        {
            leaveChannel(*userChannelsIterator, userName);
        }
        users::instance().delUser(userName);
    }
}



void management::NICK(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string oldnick = HostmaskToNick(data);
    std::string userName = data[2];
    boost::erase_all(userName, ":");
    //vector<string> userName = Split(data[2], ":",true,true);
    if (oldnick == Global::Instance().get_BotNick())
    {
        Global::Instance().set_BotNick(userName);
        //botnick = userName[0];
        U.ChangeNick(oldnick, userName);
        std::vector< std::string > channels = U.GetChannels(userName);
        for ( size_t i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], userName);
        }
        //cout << "NICK" << endl;
    }
    else
    {
        U.ChangeNick(oldnick, userName);
        std::vector< std::string > channels = U.GetChannels(userName);
        for ( size_t i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], userName);
        }
        //cout << "NICK" << endl;
    }
}

void management::MODE(std::vector< std::string > data)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string userName = HostmaskToNick(data);
    if (userName == Global::Instance().get_BotNick())
    {

    }
    else
    {

    }
    bool add = false;
    int parsepos = 4;
    std::string modeparse = data[3];
    for (size_t i = 0; i < modeparse.length(); i++)
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
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << ownerchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == adminchar)
        {
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << adminchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == opchar)
        {
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << opchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == halfopchar)
        {
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << halfopchar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == voicechar)
        {
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << voicechar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
    }
}

void management::UserAuth(std::string msNick, std::string msAuth)
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

void management::EndWhois(std::string msNick)
{
    NoWhoisUsers.erase (msNick);
    /*std::vector< std::string > userchannels = U.GetChannels(mNick);
    if (boost::iequals(userchannels[0], "NULL"))
    {
        //U.DelUser(mNick);
    }*/
}

std::string management::HostmaskToNick(std::vector< std::string > data)
{
    std::vector< std::string > who;
    boost::split( who, data[0], boost::is_any_of("!"), boost::token_compress_on );
    std::string userName = who[0];
    boost::erase_all(userName, ":");
    return userName;
}


void management::GetUserInfo(std::string data)
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

void management::GetChannelInfo(std::string msChannel)
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
    size_t i;
    for (i = 0 ; i < channels_vector.size() ; i++)
    {
        std::string auth = DatabaseData::Instance().GetAuthByUserUuid(channels_vector[i][0]);
        //std::cout << msChannel << " " << auth << " " << channels_vector[i][1] << std::endl;
        C.AddAuth(msChannel, auth);
        C.SetAccess(msChannel, auth, convertString(channels_vector[i][1]));
    }
}

void management::GetAuths()
{
    UsersInterface& U = Global::Instance().get_Users();
    std::vector< std::string > auth_vector;
    auth_vector = DatabaseData::Instance().GetAuths();
    size_t i;
    for (i = 0; i < auth_vector.size(); i++)
    {
        U.AddAuth(auth_vector[i]);
        //std::cout << auth_vector[i] << std::endl;
    }
}




void management::leaveChannel(std::string channelName, std::string userName)
{
    if (userName == Global::Instance().get_BotNick())
    {
        std::unordered_set< std::string > channelUsers = channels::instance().getChannel(channelName).getUsers();
        std::unordered_set< std::string >::iterator channelUsersIterator;
        for (channelUsersIterator = channelUsers.begin(); channelUsersIterator != channelUsers.end(); ++channelUsersIterator)
        {
            users::instance().getUser(*channelUsersIterator).delChannel(channelName);
            channels::instance().getChannel(channelName).delUser(*channelUsersIterator);
            if (users::instance().getUser(userName).getChannels().empty())
            {
                output::instance().addOutput(*channelUsersIterator + ": no channels left, deleting.", 8);
                users::instance().delUser(*channelUsersIterator);
            }
        }
        channels::instance().delChannel(channelName);
    }
    else
    {
        channels::instance().getChannel(channelName).delUser(userName);
        users::instance().getUser(userName).delChannel(channelName);
        if (users::instance().getUser(userName).getChannels().empty())
        {
            output::instance().addOutput(userName + ": no channels left, deleting.", 8);
            users::instance().delUser(userName);
        }
    }
}

void management::whoChannel(std::string channelName)
{
    if (m_WhoExtra)
    {
        irc::instance().addSendQueue(reply::instance().ircWhoExtra(channelName));
    }
    else
    {
        irc::instance().addSendQueue(reply::instance().ircWho(channelName));
    }
}

void management::userModes(std::string userName, std::string userModes)
{
    // ### user modes ###
    size_t gonePos = userModes.find(gonechar);
    if (gonePos != std::string::npos)
    {
        users::instance().getUser(userName).setGone(true);
    }

    size_t herePos = userModes.find(herechar);
    if (herePos != std::string::npos)
    {
        users::instance().getUser(userName).setGone(false);
    }

    size_t xPos = userModes.find("x");
    if (xPos != std::string::npos)
    {
        users::instance().getUser(userName).setX(true);
    }

    size_t botPos = userModes.find(botchar);
    if (botPos != std::string::npos)
    {
        users::instance().getUser(userName).setBot(true);
    }

    size_t ircOpPos = userModes.find(operchar);
    if (ircOpPos != std::string::npos)
    {
        users::instance().getUser(userName).setIrcOp(true);
    }
    // ### end user modes ###
}

void management::userChannelModes(std::string channelName, std::string userName, std::string channelUserModes)
{
    // ### user channel modes ###
    size_t ownerPos = channelUserModes.find(ownerwhochar);
    if (ownerPos != std::string::npos)
    {
        //channels::instance().getChannel(channelName).a_random_function(userName, true);
    }

    size_t adminPos = channelUserModes.find(adminwhochar);
    if (adminPos != std::string::npos)
    {
        //channels::instance().getChannel(channelName).a_random_function(userName, true);
    }

    size_t opPos = channelUserModes.find(opwhochar);
    if (opPos != std::string::npos)
    {
        //channels::instance().getChannel(channelName).a_random_function(userName, true);
    }

    size_t halfOpPos = channelUserModes.find(halfopwhochar);
    if (halfOpPos != std::string::npos)
    {
        //channels::instance().getChannel(channelName).a_random_function(userName, true);
    }

    size_t voicePos = channelUserModes.find(voicewhochar);
    if (voicePos != std::string::npos)
    {
        //channels::instance().getChannel(channelName).a_random_function(userName, true);
    }
    // ### end user channel modes ###
}
