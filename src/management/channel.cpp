//
//
//  @ Project : ircppbot
//  @ File Name : channel.cpp
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


#include "../include/management/channel.h"
#include <gframe/output.h>

channel::channel() :
m_Users(),
m_Bans(),
m_Topic(""),
m_Modes()
{
    m_Users.clear();
    m_Bans.clear();
    m_Modes.clear();
}

channel::channel(const channel&)
{
}

channel::~channel()
{
}


// ### channel users ###
std::shared_ptr<user> channel::addUser(std::string userName, std::shared_ptr<user> userSharedPointer)
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    std::pair< std::map< std::string, std::shared_ptr<user> >::iterator, bool > ret;
    ret = m_Users.insert (std::pair< std::string, std::shared_ptr<user> >(userName, userSharedPointer));
    return ret.first->second;
}

bool channel::delUser(std::string userName)
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    size_t ret = m_Users.erase(userName);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool channel::delUser(std::string userName) user found, erase succesfull: " + userName);
        return true;
    }
    output::instance().addStatus(false, "bool channel::delUser(std::string userName) user found, erase failed: " + userName);
    return false;
}

std::map< std::string, std::shared_ptr<user> >&  channel::getUsers()
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    return m_Users;
}
// ### end user channels ###

// ### user channel modes ###
void channel::setUserOwner(std::string userName, bool set)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        l_UserModes->second["owner"] = set;
    }
}
bool channel::getUserOwner(std::string userName)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        return l_UserModes->second["owner"];
    }
    return false;
}

void channel::setUserAdmin(std::string userName, bool set)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        l_UserModes->second["admin"] = set;
    }
}
bool channel::getUserAdmin(std::string userName)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        return l_UserModes->second["admin"];
    }
    return false;
}

void channel::setUserOp(std::string userName, bool set)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        l_UserModes->second["op"] = set;
    }
}
bool channel::getUserOp(std::string userName)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        return l_UserModes->second["op"];
    }
    return false;
}

void channel::setUserHalfOp(std::string userName, bool set)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        l_UserModes->second["halfop"] = set;
    }
}
bool channel::getUserHalfOp(std::string userName)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        return l_UserModes->second["halfop"];
    }
    return false;
}

void channel::setUserVoice(std::string userName, bool set)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        l_UserModes->second["voice"] = set;
    }
}
bool channel::getUserVoice(std::string userName)
{
    std::map< std::string, std::map< std::string, bool > >::iterator l_UserModes = m_UsersModes.find(userName);
    if (l_UserModes != m_UsersModes.end())
    {
        return l_UserModes->second["voice"];
    }
    return false;
}
// ### end user channel modes ###
