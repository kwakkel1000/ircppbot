//
//
//  @ Project : ircppbot
//  @ File Name : users.cpp
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


#include "../include/management/users.h"
#include <gframe/output.h>

users::users()
{
}

users::~users()
{
    /*std::map< std::string, user >::iterator m_UserListIterator;
    for (m_UserListIterator = m_UserList.begin(); m_UserListIterator < m_UserList.end(); ++m_UserListIterator)
    {
        delUser((*m_UserListIterator).first);
    }*/
}

bool users::addUser(std::string userName)
{
    std::pair< std::map< std::string, user >::iterator, bool > ret;
    ret = m_UserList.insert (std::pair< std::string, user >(userName, user()));
    if (ret.second)
    {
        output::instance().addStatus(true, "bool users::addUser(std::string userName) add: " + userName);
        return true;
    }
    //output::instance().addStatus(false, "bool users::addUser(std::string userName) user already exists");
    return false;
}

bool users::delUser(std::string userName)
{
    size_t ret = m_UserList.erase(userName);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool users::delUser(std::string userName) user found, erase succesfull: " + userName);
        return true;
    }
    output::instance().addStatus(false, "bool users::delUser(std::string userName) user found, erase failed: " + userName);
    return false;
}

bool users::renameUser(std::string oldUserName, std::string newUserName)
{
    user tmpUser = getUser(oldUserName);
    std::pair< std::map< std::string, user >::iterator, bool > ret;
    ret = m_UserList.insert (std::pair< std::string, user >(newUserName, tmpUser));
    if (ret.second)
    {
        delUser(oldUserName);
        return true;
    }
    output::instance().addStatus(false, "bool users::renameUser(std::string oldUserName, std::string newUserName) user already exists");
    return false;
}

bool users::findUser(std::string userName)
{
    std::map< std::string, user >::iterator l_UserListIterator;
    l_UserListIterator = m_UserList.find(userName);
    if (l_UserListIterator == m_UserList.end())
    {
        return false;
    }
    return true;
}

bool users::getUsers(std::map< std::string, user > &userList)
{
    userList = m_UserList;
    return true;
}

user& users::getUser(std::string userName)
{
    std::map< std::string, user >::iterator m_UserListIterator;
    m_UserListIterator = m_UserList.find(userName);
    if (m_UserListIterator == m_UserList.end())
    {
        output::instance().addStatus(false, "user& users::getUser(std::string userName) user not found");
        exit(EXIT_FAILURE);
    }
    return (*m_UserListIterator).second;
}

void users::setBotNick(std::string botNick)
{
    output::instance().addOutput("BOTNICK: " + botNick);
    m_BotNick = botNick;
}
std::string users::getBotNick()
{
    return m_BotNick;
}
