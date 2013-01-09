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
#include "include/management/auths.h"
#include "include/irc.h"
#include "include/reply.h"

#include <gframe/glib.h>
#include <gframe/configreader.h>
#include <gframe/output.h>
#include <gframe/database.h>

//#include <management/Whois.h>

#include <cstddef>
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
    channels::instance();
    users::instance();
}

management::~management()
{
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

void management::stop()
{
    m_Run = false;
    m_IrcData->stop();
    output::instance().addOutput("management::stop", 6);
    m_ModesThread->join();
    output::instance().addOutput("management::stop m_ModesThread stopped", 6);
    m_WhoisThread->join();
    output::instance().addOutput("management::stop m_WhoisThread stopped", 6);
    m_EventsThread->join();
    output::instance().addOutput("management::stop m_EventsThread stopped", 6);
    irc::instance().delConsumer(m_IrcData);
    delete m_IrcData;
}

void management::init()
{
    m_NickServer = (configreader::instance().getString("nickserv") == "true");
    m_WhoExtra = (configreader::instance().getString("whoextra") == "true");
    getAuths();
    m_IrcData = new ircdata();
    m_IrcData->setModes(true);
    m_IrcData->setWhois(true);
    m_IrcData->setEvents(true);
    irc::instance().addConsumer(m_IrcData);
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
                users::instance().setBotNick(data[2]);
            }
            if (data[1] == "307")       //WHOIS regged userName
            {
                if (m_NickServer)
                {
                    std::string userName = data[3];
                    std::string auth = data[3];
                    userAuth(userName, auth);
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
                    std::string userName = data[3];
                    std::string auth = data[4];
                    userAuth(userName, auth);
                }
            }
            if (data[1] == "402")       //WHOIS no server
            {
                //WHOIS(data);
            }
        }
        if (data.size() == 7)
        {
            if (data[1] == "354")       //WHOEXTRA
            {
                whoextra(data);
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
    std::string channelName = data[3];
    std::string userName = data[7];
    std::string modes = data[8];

    if (!channels::instance().findChannel(channelName))
    {
        channels::instance().addChannel(channelName);
    }
    if (channels::instance().findChannel(channelName))
    {
        channels::instance().getChannel(channelName).addUser(userName);
        bool added = users::instance().addUser(userName);
        //U.FirstJoin(userName);
        users::instance().getUser(userName).addChannel(channelName);

        userModes(userName, modes);
        userChannelModes(channelName, userName, modes);


        if (added)
        {
            irc::instance().addSendQueue(reply::instance().ircWhois(userName));
        }
    }

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
            if (!channels::instance().findChannel(channelName))
            {
                channels::instance().addChannel(channelName);
            }
            if (channels::instance().findChannel(channelName))
            {
                channels::instance().getChannel(channelName).addUser(userName);
            }

            if (!users::instance().findUser(userName))
            {
                users::instance().addUser(userName);
            }
            if (users::instance().findUser(userName))
            {
                users::instance().getUser(userName).addChannel(channelName);
            }
            //U.FirstJoin(userName);
            userAuth(userName, auth);

            userModes(userName, modes);
            userChannelModes(channelName, userName, modes);
        }
    }
}

void management::join(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::join(std::vector< std::string > eventData)", 11);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    if (userName == users::instance().getBotNick())
    {
        output::instance().addOutput("void management::join(std::vector< std::string > eventData) bot(" + userName + ") joins channel(" + channelName + ")", 11);
        channels::instance().addChannel(channelName);
        whoChannel(channelName);
    }
    else
    {
        bool added = users::instance().addUser(userName);
        if (channels::instance().findChannel(channelName))
        {
            channels::instance().getChannel(channelName).addUser(userName);
            users::instance().getUser(userName).addChannel(channelName);
            if (added)
            {
                irc::instance().addSendQueue(reply::instance().ircWhois(userName));
            }
            if (users::instance().getUser(userName).getAuth() != "")
            {
                // something
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
    output::instance().addOutput("void management::part(std::vector< std::string > eventData)", 11);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    leaveChannel(channelName, userName);
}

void management::kick(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::kick(std::vector< std::string > eventData)", 11);
    std::string channelName = eventData[2];
    deleteFirst(channelName, ":");
    std::string userName = eventData[3];
    leaveChannel(channelName, userName);
}

void management::quit(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::quit(std::vector< std::string > eventData)", 11);
    std::string userName = eventData[0];
    nickFromHostmask(userName);
    if (userName == users::instance().getBotNick())
    {
        // should never see its own quit :S
        exit(EXIT_FAILURE);
    }
    else
    {
        if (users::instance().findUser(userName))
        {
            std::unordered_set< std::string > userChannels = users::instance().getUser(userName).getChannels();
            std::unordered_set< std::string >::iterator userChannelsIterator;
            for (userChannelsIterator = userChannels.begin(); userChannelsIterator != userChannels.end(); ++userChannelsIterator)
            {
                leaveChannel(*userChannelsIterator, userName);
            }
        }
        users::instance().delUser(userName);
    }
}



void management::nick(std::vector< std::string > eventData)
{
    output::instance().addOutput("void management::nick(std::vector< std::string > eventData)", 11);
    std::string newUserName = eventData[2];
    std::string oldUserName = eventData[0];
    nickFromHostmask(oldUserName);
    if (!users::instance().findUser(oldUserName))
    {
        output::instance().addOutput("void management::nick(std::vector< std::string > eventData) nickchange but oldnick not found? " + oldUserName, 11);
        users::instance().addUser(oldUserName);
    }
    std::string authName = users::instance().getUser(oldUserName).getAuth();

    if (oldUserName == users::instance().getBotNick())
    {
        users::instance().setBotNick(newUserName);
    }
    if (auths::instance().findAuth(authName))
    {
        auths::instance().getAuth(authName).delUser(oldUserName);
        auths::instance().getAuth(authName).addUser(newUserName);
    }
    std::unordered_set< std::string > channelSet = users::instance().getUser(oldUserName).getChannels();
    std::unordered_set< std::string >::iterator channelSetIterator;
    for (channelSetIterator = channelSet.begin(); channelSetIterator != channelSet.end(); ++channelSetIterator)
    {
        channels::instance().getChannel(*channelSetIterator).delUser(oldUserName);
        channels::instance().getChannel(*channelSetIterator).addUser(newUserName);
    }
    users::instance().renameUser(oldUserName, newUserName);
}

void management::mode(std::vector< std::string > data)
{
    output::instance().addOutput("void management::mode(std::vector< std::string > data)", 11);
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

void management::userAuth(std::string userName, std::string authName)
{
    output::instance().addOutput("void management::userAuth(std::string userName, std::string authName)", 11);
    if (!auths::instance().findAuth(authName))
    {
        auths::instance().addAuth(authName);
        // uuid maybe?
        //databasedata::instance().insert(something);
    }
    if (auths::instance().findAuth(authName))
    {
        auths::instance().getAuth(authName).addUser(userName);
    }

    if (!users::instance().findUser(userName))
    {
        users::instance().addUser(userName);
    }
    if (users::instance().findUser(userName))
    {
        users::instance().getUser(userName).setAuth(authName);
    }
    if (!users::instance().getUser(userName).getChannels().empty())
    {
        getUserInfo(userName);
        /*

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
        */
    }
    else
    {
        auths::instance().getAuth(authName).delUser(userName);
        users::instance().delUser(userName);
    }
}

void management::endWhois(std::string userName)
{
    //NoWhoisUsers.erase (userName);
    if (users::instance().getUser(userName).getChannels().empty())
    {
        std::string authName = users::instance().getUser(userName).getAuth();
        auths::instance().getAuth(authName).delUser(userName);
        users::instance().delUser(userName);
    }
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
    if (pos > 2 && pos != std::string::npos)
    {
        tmpstring = data.substr(0, pos-1);
    }
    tmpstring = tmpstring + data.substr(pos+1);
    data = tmpstring;
    return true;
}


void management::getUserInfo(std::string userName)
{
    std::string authName = users::instance().getUser(userName).getAuth();
    if (authName != "")
    {
        std::string language = "english";
        size_t width = 0;
        size_t columns = 0;
        size_t botAccess = 0;
        bool god = false;
        //
        // fill vars from database or something
        //
        auths::instance().getAuth(authName).setLanguage(language);
        auths::instance().getAuth(authName).setWidth(width);
        auths::instance().getAuth(authName).setColumns(columns);
        auths::instance().getAuth(authName).setBotAccess(botAccess);
        auths::instance().getAuth(authName).setGod(god);
    }
}

void management::getAuths()
{
    std::vector< std::string > authsVector;
    authsVector = databasedata::instance().get(configreader::instance().getString("authtable"), "auth");
    for (size_t authsVectorIterator = 0; authsVectorIterator < authsVector.size(); authsVectorIterator++)
    {
        auths::instance().addAuth(authsVector[authsVectorIterator]);
    }
}

void management::leaveChannel(std::string channelName, std::string userName)
{
    if (userName == users::instance().getBotNick())
    {
        std::unordered_set< std::string > channelUsers = channels::instance().getChannel(channelName).getUsers();
        std::unordered_set< std::string >::iterator channelUsersIterator;
        for (channelUsersIterator = channelUsers.begin(); channelUsersIterator != channelUsers.end(); ++channelUsersIterator)
        {
            users::instance().getUser(*channelUsersIterator).delChannel(channelName);
            if (channels::instance().findChannel(channelName))
            {
                channels::instance().getChannel(channelName).delUser(*channelUsersIterator);
                if (users::instance().getUser(*channelUsersIterator).getChannels().empty())
                {
                    output::instance().addOutput(*channelUsersIterator + ": no channels left, deleting.", 8);
                    if (auths::instance().findAuth(users::instance().getUser(*channelUsersIterator).getAuth()))
                    {
                        auths::instance().getAuth(users::instance().getUser(*channelUsersIterator).getAuth()).delUser(*channelUsersIterator);
                    }
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
                    if (auths::instance().findAuth(users::instance().getUser(userName).getAuth()))
                    {
                        auths::instance().getAuth(users::instance().getUser(userName).getAuth()).delUser(userName);
                    }
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
