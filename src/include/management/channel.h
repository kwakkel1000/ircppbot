//
//
//  @ Project : ircppbot
//  @ File Name : channel.h
//  @ Date : 26-12-2012
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


#ifndef SRC_INCLUDE_MANAGEMENT_CHANNEL_H
#define SRC_INCLUDE_MANAGEMENT_CHANNEL_H
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <mutex>

class channel
{
public:
        channel();
        channel(const channel&);
        ~channel();

        // ### channel users ###
        bool addUser(std::string userName);
        bool delUser(std::string userName);
        std::unordered_set< std::string > getUsers();
        // ### end channel users ###
    private:
        //std::unordered_map< std::string, std::unordered_set< std::string > > m_Users;
        std::unordered_set< std::string > m_Users;
        std::unordered_set< std::string > m_Bans;

        // ### irc channel modes ###
        std::string m_Topic;
        std::unordered_map< std::string, bool > m_Modes;
        // ### end irc channel modes ###

        std::mutex m_UsersMutex;
        std::mutex m_BansMutex;
};

#endif // SRC_INCLUDE_MANAGEMENT_CHANNEL_H
