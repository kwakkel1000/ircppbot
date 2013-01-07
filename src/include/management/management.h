//
//
//  @ Project : ircppbot
//  @ File Name : management.h
//  @ Date : 05-01-2013
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2013 Gijs Kwakkel
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

#ifndef SRC_INCLUDE_MANAGEMENT_MANAGEMENT_H
#define SRC_INCLUDE_MANAGEMENT_MANAGEMENT_H

#include "ircdata.h"
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <atomic>

class ircdata;
class management
{
    public:
        management();
        ~management();
        void read();
        void stop();
        void init(ircdata* ircData);
        void timerrun(){}

    private:
        //void ParseData(std::vector< std::string > data);
        //void RefreshChannel(std::string sChannel);

        std::atomic<bool> m_NickServer;
        std::atomic<bool> m_Run;
        std::atomic<bool> m_WhoExtra;
        ircdata* m_IrcData;

        void who(std::vector< std::string > data);
        void whoextra(std::vector< std::string > data);

        void join(std::vector< std::string > data);
        void part(std::vector< std::string > data);
        void kick(std::vector< std::string > data);
        void quit(std::vector< std::string > data);

        void nick(std::vector< std::string > data);
        void mode(std::vector< std::string > data);

        bool deleteFirst(std::string& data, std::string character);
        bool nickFromHostmask(std::string& data);
        void userAuth(std::string mNick, std::string mAuth);
        void endWhois(std::string msNick);

        void getChannelInfo(std::string data);
        void getUserInfo(std::string data);
        void getAuths();

        void leaveChannel(std::string channelName, std::string userName);
        void whoChannel(std::string channelName);
        void userModes(std::string userName, std::string userModes);
        void userChannelModes(std::string channelName, std::string userName, std::string channelUserModes);

        void parseModes();
        void parseWhois();
        void parseEvents();

        std::shared_ptr< std::thread > m_ModesThread;
        std::shared_ptr< std::thread > m_WhoisThread;
        std::shared_ptr< std::thread > m_EventsThread;

        std::multimap< std::string, std::string > NoWhoisUsers;
};


#endif // SRC_INCLUDE_MANAGEMENT_MANAGEMENT_H
