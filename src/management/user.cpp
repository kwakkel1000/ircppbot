//
//
//  @ Project : ircppbot
//  @ File Name : user.cpp
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


#include "../include/management/user.h"
#include <gframe/output.h>

user::user() :
m_Channels(),
m_Auth(),
m_Gone(false),
m_X(false),
m_Bot(false),
m_IrcOp(false)
{
    m_Channels.clear();
    setAuth("", nullptr);
}

user::user(const user&)
{
}

user::~user()
{
}

// ### user channels ###
std::shared_ptr<channel> user::addChannel(std::string channelName, std::shared_ptr<channel> channelSharedPointer)
{
    std::lock_guard< std::mutex > lock(m_ChannelsMutex);
    std::pair< std::map< std::string, std::shared_ptr<channel> >::iterator, bool > ret;
    ret = m_Channels.insert (std::pair< std::string, std::shared_ptr<channel> >(channelName, channelSharedPointer));
    return ret.first->second;
}

bool user::delChannel(std::string channelName)
{
    std::lock_guard< std::mutex > lock(m_ChannelsMutex);
    size_t ret = m_Channels.erase(channelName);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool user::delChannel(std::string channelName) channel found, erase succesfull: " + channelName);
        return true;
    }
    output::instance().addStatus(false, "bool user::delChannel(std::string channelName) channel found, erase failed: " + channelName);
    return false;
}

std::map< std::string, std::shared_ptr<channel> >&  user::getChannels()
{
    std::lock_guard< std::mutex > lock(m_ChannelsMutex);
    return m_Channels;
}
// ### end user channels ###

void user::setAuth(std::string authName, std::shared_ptr<auth> authSharedPointer)
{
    m_Auth = std::pair< std::string, std::shared_ptr<auth> >(authName, authSharedPointer);
}
std::pair< std::string, std::shared_ptr<auth> >& user::getAuth()
{
    return m_Auth;
}

// ### user irc modes ###
void user::setGone(bool gone)
{
    m_Gone = gone;
}
bool user::getGone() const
{
    return m_Gone;
}

void user::setX(bool x)
{
    m_X = x;
}
bool user::getX() const
{
    return m_X;
}

void user::setBot(bool bot)
{
    m_Bot = bot;
}
bool user::getBot() const
{
    return m_Bot;
}

void user::setIrcOp(bool ircOp)
{
    m_IrcOp = ircOp;
}
bool user::getIrcOp() const
{
    return m_IrcOp;
}
// ### end user irc modes ###
