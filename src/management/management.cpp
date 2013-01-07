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
#include <gframe/configreader.h>
#include <gframe/output.h>

#include <management/Whois.h>

#include <algorithm>
#include <sstream>
#include <cstring>

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
    //m_IrcData->stop();
    output::instance().addOutput("management::stop", 6);
    m_ModesThread->join();
    m_WhoisThread->join();
    m_EventsThread->join();
}

void management::init(ircdata* ircData)
{
    m_NickServer = (configreader::instance().getString("nickserv") == "true");
    m_WhoExtra = (configreader::instance().getString("whoextra") == "true");
    getAuths();
    m_IrcData = ircData;
    m_IrcData->setModes(true);
    m_IrcData->setWhois(true);
    m_IrcData->setEvents(true);
    irc::instance().addConsumer(m_IrcData);
}

void management::read()
{
    m_Run = true;
    m_ModesThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&management::parseModes, this)));
    m_WhoisThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&management::parseWhois, this)));
    m_EventsThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&management::parseEvents, this)));
    /*int Tijd;
    time_t t;
    t = time(0);
    Tijd = t;
    int iRefreshTime = 0;*/
    while (m_Run)
    {
        /*iRefreshTime++;
        if (iRefreshTime >= 3600)
        {
            iRefreshTime = 0;
            std::vector< std::string > vChannels = C.GetChannels();
            for (size_t iChannelsIndex = 0; iChannelsIndex < vChannels.size(); iChannelsIndex++)
            {
                RefreshChannel(vChannels[iChannelsIndex]);
                usleep(5 * 100000); // 5 * 1000000 5 seconds between each channel
            }
        }*/
        usleep(1000000);
    }
}

void management::parseModes()
{
    output::instance().addOutput("void management::parseModes()");
    std::vector< std::string > data;
    while(m_Run)
    {
        data = m_IrcData->getModesQueue();
        if (data.size() == 4)
        {
            if (data[1] == "MODE")      //KICK
            {
                mode(data);
            }
        }
        if (data.size() >= 4)
        {
            if (data[1] == "TOPIC")      //TOPIC
            {
                //topic(data);
            }
        }
        if (data.size() >= 5)
        {
            if (data[1] == "332")      //TOPIC
            {
                //topic(data);
            }
        }
    }
}

void management::parseWhois()
{
    output::instance().addOutput("void management::parseWhois()");
    std::vector< std::string > data;
    while(m_Run)
    {
        data = m_IrcData->getWhoisQueue();
        if (data.size() >= 4)
        {
            if (data[1] == "001")   //welcome
            {
                //Global::Instance().set_BotNick(data[2]);
            }
            if (data[1] == "307")       //WHOIS regged userName
            {
                if (m_NickServer)
                {
                    userAuth(data[3], data[3]);
                }
            }
            if (data[1] == "318")       //WHOIS end
            {
                endWhois(data[3]);
                //WHOIS(data);
            }
            if (data[1] == "330")       //WHOIS auth
            {
                if (!m_NickServer)
                {
                    userAuth(data[3], data[4]);
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
                    whoextra(data);
                }
            }
        }
        if (data.size() >= 11)
        {
            if (data[1] == "352")       //WHO
            {
                who(data);
            }
        }
    }
}

void management::parseEvents()
{
    output::instance().addOutput("void management::parseEvents()");
    std::vector< std::string > data;
    while(m_Run)
    {
        data = m_IrcData->getEventsQueue();
        if (data.size() == 3)
        {
            if (data[1] == "NICK")
            {
                nick(data);
            }
            if (data[1] == "JOIN")
            {
                join(data);
            }
        }
        if (data.size() >= 3)
        {
            if (data[1] == "QUIT")
            {
                quit(data);
            }
            if (data[1] == "PART")
            {
                part(data);
            }
        }
        if (data.size() >= 4)
        {
            if (data[1] == "KICK")
            {
                kick(data);
            }
        }
    }
}

void management::who(std::vector< std::string > data)
{
    /*std::string channelName = data[3];
    std::string userName = data[7];
    std::string modes = data[8];

    channels::instance().getChannel(channelName).addUser(userName);
    bool added = users::instance().addUser(userName);
    //U.FirstJoin(userName);
    users::instance().getUser(userName).addChannel(channelName);
    getChannelInfo(channelName);

    userModes(userName, modes);
    channelUserModes(channelName, userName, modes);


    if (added)
    {
        irc::instance().addSendQueue(reply::instance().ircWhois(userName));
    }*/
}

void management::whoextra(std::vector< std::string > data)
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
            if (channels::instance().findChannel(channelName))
            {
                channels::instance().getChannel(channelName).addUser(userName);
            }
            if (users::instance().findUser(userName))
            {
                users::instance().addUser(userName);
                users::instance().getUser(userName).addChannel(channelName);
            }
            //U.FirstJoin(userName);
            userAuth(userName, auth);
            getChannelInfo(channelName);

            userModes(userName, modes);
            //channelUserModes(channelName, userName, modes);
        }
    }
}

void management::join(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::join(std::vector< std::string > eventData)", 10);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    //boost::erase_all(channelName, ":");
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    //if (userName == Global::Instance().get_BotNick())
    if (userName == "bot")
    {
        channels::instance().addChannel(channelName);
        whoChannel(channelName);
        getChannelInfo(channelName);
    }
    else
    {
        bool added;
        if (channels::instance().findChannel(channelName))
        {
            channels::instance().getChannel(channelName).addUser(userName);
            added = users::instance().addUser(userName);
            users::instance().getUser(userName).addChannel(channelName);
            if (added)
            {
                irc::instance().addSendQueue(reply::instance().ircWhois(userName));
            }
            /*if (U.GetAuth(userName) != "NULL")
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
            }*/
        }
    }
}

void management::part(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::part(std::vector< std::string > eventData)", 10);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    leaveChannel(channelName, userName);
}

void management::kick(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::kick(std::vector< std::string > eventData)", 10);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    std::string userName = eventData[3];
    leaveChannel(channelName, userName);
}

void management::quit(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::quit(std::vector< std::string > eventData)", 10);
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    //if (userName == Global::Instance().get_BotNick())
    if (userName == "bot")
    {
        // should never see its own quit :S
        exit(EXIT_FAILURE);
    }
    else
    {
        std::unordered_set< std::string > userChannels = users::instance().getUser(userName).getChannels();
        std::unordered_set< std::string >::iterator userChannelsIterator;
        for (userChannelsIterator = userChannels.begin(); userChannelsIterator != userChannels.end(); ++userChannelsIterator)
        {
            leaveChannel(*userChannelsIterator, userName);
        }
        users::instance().delUser(userName);
    }
}



void management::nick(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::nick(std::vector< std::string > eventData)", 10);
    /*UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string oldnick = nickFromHostmask(data);
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
    }*/
}

void management::mode(std::vector< std::string > data)
{
    output::instance().addOutput("void management::mode(std::vector< std::string > data)", 10);
    /*std::string channelName = data[2];
    std::string userName = nickFromHostmask(data);
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
            //C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == adminchar)
        {
            //cout << "channelName: " << data[2] << " user: " << data[parsepos] << " " << adminchar << endl;
            //C.SetOp(data[2], data[parsepos], add);
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
    }*/
}

void management::userAuth(std::string msNick, std::string msAuth)
{
    output::instance().addOutput("void management::userAuth(std::string msNick, std::string msAuth)", 10);
    /*UsersInterface& U = Global::Instance().get_Users();
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
    }*/
}

void management::endWhois(std::string msNick)
{
    NoWhoisUsers.erase (msNick);
    /*std::vector< std::string > userchannels = U.GetChannels(mNick);
    if (boost::iequals(userchannels[0], "NULL"))
    {
        //U.DelUser(mNick);
    }*/
}

bool management::nickFromHostmask(std::string& data)
{
    std::vector< std::string > who;
    who = glib::split(data);
    if (deleteFirst(who[0], ":"))
    {
        size_t pos;
        pos = who[0].find("!");
        data = who[0].substr(0, pos);
        return true;
    }
    else
    {
        return false;
    }
}

bool management::deleteFirst(std::string& data, std::string character)
{
    size_t pos;
    std::string tmpstring = "";
    pos = data.find(character);
    if (pos > 2)
    {
        tmpstring = data.substr(0, pos-1);
    }
    tmpstring = tmpstring + data.substr(pos+1);
    data = tmpstring;
    return true;
}


void management::getUserInfo(std::string data)
{
    /*UsersInterface& U = Global::Instance().get_Users();
    std::string auth = U.GetAuth(data);
    if (boost::iequals(auth, "NULL") != true)
    {
        //U.SetUid(data, DatabaseData::Instance().GetUserUuidByAuth(auth));
        U.SetOaccess(data, DatabaseData::Instance().GetOaccessByAuth(auth));
        U.SetGod(data, DatabaseData::Instance().GetGodByAuth(auth));
        U.SetLanguage(data, DatabaseData::Instance().GetLanguageByAuth(auth));
    }*/
}

void management::getChannelInfo(std::string msChannel)
{
    /*ChannelsInterface& C = Global::Instance().get_Channels();
    C.InitSetting(msChannel, "giveops", BotLib::StringFromInt(DatabaseData::Instance().GetGiveOpsByChannel(msChannel)));
    C.InitSetting(msChannel, "givevoice", BotLib::StringFromInt(DatabaseData::Instance().GetGiveVoiceByChannel(msChannel)));
    C.SetCid(msChannel, DatabaseData::Instance().GetChannelUuidByChannel(msChannel));
    /*C.SetGiveops(msChannel, DatabaseData::Instance().GetGiveOpsByChannel(msChannel));
    C.SetGivevoice(msChannel, DatabaseData::Instance().GetGiveVoiceByChannel(msChannel));*/
/*
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
    }*/
}

void management::getAuths()
{
    /*UsersInterface& U = Global::Instance().get_Users();
    std::vector< std::string > auth_vector;
    auth_vector = DatabaseData::Instance().GetAuths();
    size_t i;
    for (i = 0; i < auth_vector.size(); i++)
    {
        U.AddAuth(auth_vector[i]);
        //std::cout << auth_vector[i] << std::endl;
    }*/
}




void management::leaveChannel(std::string channelName, std::string userName)
{
    //if (userName == Global::Instance().get_BotNick())
    if (userName == "bot")
    {
        //std::unordered_set< std::string > channelUsers = channels::instance().getChannel(channelName).getUsers();
        //std::unordered_set< std::string >::iterator channelUsersIterator;
        std::set< std::string > channelUsers = channels::instance().getChannel(channelName).getUsers();
        std::set< std::string >::iterator channelUsersIterator;
        for (channelUsersIterator = channelUsers.begin(); channelUsersIterator != channelUsers.end(); ++channelUsersIterator)
        {
            users::instance().getUser(*channelUsersIterator).delChannel(channelName);
            if (channels::instance().findChannel(channelName))
            {
                channels::instance().getChannel(channelName).delUser(userName);
                if (users::instance().getUser(userName).getChannels().empty())
                {
                    output::instance().addOutput(*channelUsersIterator + ": no channels left, deleting.", 8);
                    users::instance().delUser(*channelUsersIterator);
                }
            }
            else
            {
                output::instance().addOutput("void management::leaveChannel(std::string channelName, std::string userName) channel not found : " + channelName, 8);
            }
        }
        channels::instance().delChannel(channelName);
    }
    else
    {
        if (channels::instance().findChannel(channelName))
        {
            channels::instance().getChannel(channelName).delUser(userName);
            if (users::instance().findUser(userName))
            {
                users::instance().getUser(userName).delChannel(channelName);
                if (users::instance().getUser(userName).getChannels().empty())
                {
                    output::instance().addOutput(userName + ": no channels left, deleting.", 8);
                    users::instance().delUser(userName);
                }
            }
            else
            {
                output::instance().addOutput("void management::leaveChannel(std::string channelName, std::string userName) user not found : " + userName, 8);
            }
        }
        else
        {
            output::instance().addOutput("void management::leaveChannel(std::string channelName, std::string userName) channel not found : " + channelName, 8);
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
