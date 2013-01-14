//
//
//  @ Project : ircppbot
//  @ File Name : channel.h
//  @ Date : 10-01-2013
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


#ifndef SRC_INCLUDE_MANAGEMENT_CHANNEL_H
#define SRC_INCLUDE_MANAGEMENT_CHANNEL_H
#include <string>
#include <map>
#include <set>
#include <atomic>
#include <mutex>
#include <memory>
#include <cstddef>

#include "user.h"

class user;
class channel
{
    public:
        channel();
        channel(const channel&);
        ~channel();

	typedef enum {
            m_Private    = 'p',
            m_Secret     = 's',
            m_Moderated  = 'm',
            m_InviteOnly = 'i',
            m_NoMessages = 'n',
            m_OpTopic    = 't',
            m_Limit      = 'l',
            m_Key        = 'k',
            m_Op         = 'o',
            m_Voice      = 'v',
            m_Ban        = 'b',
            m_Except     = 'e'
        } e_Modes;

        // ### channel users ###
        std::shared_ptr<user> addUser(std::string userName, std::shared_ptr<user> channelSharedPointer);
        bool delUser(std::string userName);
        std::map< std::string, std::shared_ptr<user> >& getUsers();
        // ### end channel users ###

    private:
        std::map< std::string, std::shared_ptr<user> > m_Users;
        std::map< std::string, std::map< std::string, std::string > > m_UsersModes;
        std::set< std::string > m_Bans;

        // ### irc channel modes ###
        std::string m_Topic;
        std::map< std::string, bool > m_Modes;
        // ### end irc channel modes ###

        std::mutex m_UsersMutex;
        std::mutex m_BansMutex;
};

#endif // SRC_INCLUDE_MANAGEMENT_CHANNEL_H
