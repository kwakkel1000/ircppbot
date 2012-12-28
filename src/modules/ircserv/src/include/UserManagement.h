//
//
//  @ Project : ircppbot
//  @ File Name : UserManagement.h
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

#ifndef UserManagement_h
#define UserManagement_h

#include "ircdata.h"
#include <string>
#include <vector>
#include <map>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

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
    void ParseData(std::vector< std::string > data);
    void RefreshChannel(std::string sChannel);

    std::atomic<bool> m_NickServer;
    std::atomic<bool> m_Run;
    std::atomic<bool> m_WhoExtra;
    ircdata* m_IrcData;

    void WHO(std::vector< std::string > data);
    void WHOEXTRA(std::vector< std::string > data);
    void JOIN(std::vector< std::string > data);
    void PART(std::vector< std::string > data);
    void KICK(std::vector< std::string > data);
    void NICK(std::vector< std::string > data);
    void MODE(std::vector< std::string > data);
    void QUIT(std::vector< std::string > data);

    std::string HostmaskToNick(std::vector< std::string > data);
    void UserAuth(std::string mNick, std::string mAuth);
    void EndWhois(std::string msNick);

    void GetChannelInfo(std::string data);
    void GetUserInfo(std::string data);
    void GetAuths();

    void parseModes();
    void parseWhois();
    void parseEvents();

    std::shared_ptr< std::thread > m_ModesThread;
    std::shared_ptr< std::thread > m_WhoisThread;
    std::shared_ptr< std::thread > m_EventsThread;

    std::multimap< std::string, std::string > NoWhoisUsers;
};


#endif // UserManagement_h
