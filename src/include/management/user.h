//
//
//  @ Project : ircppbot
//  @ File Name : user.h
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


#ifndef SRC_INCLUDE_MANAGEMENT_USER_H
#define SRC_INCLUDE_MANAGEMENT_USER_H
#include <string>
#include <map>
#include <atomic>
#include <mutex>
#include <memory>
#include <cstddef>

#include "auth.h"
#include "channel.h"

class auth;
class channel;
class user
{
    public:
        user();
        user(const user&);
        ~user();

        // ### user channels ###
        std::shared_ptr<channel> addChannel(std::string channelName, std::shared_ptr<channel> channelSharedPointer);
        bool delChannel(std::string channelName);
        std::map< std::string, std::shared_ptr<channel> >& getChannels();
        // ### end user channels ###

        void setAuth(std::string authName, std::shared_ptr<auth> authSharedPointer);
        std::pair< std::string, std::shared_ptr<auth> >& getAuth();

        // ### irc user modes ###
        void setGone(bool gone);
        bool getGone() const;

        void setX(bool x);
        bool getX() const;

        void setBot(bool bot);
        bool getBot() const;

        void setIrcOp(bool ircOp);
        bool getIrcOp() const;
        // ### end irc user modes ###

    private:
        std::map< std::string, std::shared_ptr<channel> > m_Channels;

        std::pair< std::string, std::shared_ptr<auth> > m_Auth;

        // ### irc user modes ###
        std::atomic<bool> m_Gone;
        std::atomic<bool> m_X;
        std::atomic<bool> m_Bot;
        std::atomic<bool> m_IrcOp;
        // ### end irc user modes ###

        std::mutex m_ChannelsMutex;
};

#endif // SRC_INCLUDE_MANAGEMENT_USER_H
