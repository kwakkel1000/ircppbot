//
//
//  @ Project : ircppbot
//  @ File Name : auth.cpp
//  @ Date : 08-01-2013
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


#include "../include/management/auth.h"

auth::auth() :
m_Language(""),
m_Width(0),
m_Columns(0),
m_God(false),
m_Users()
{
    m_Users.clear();
}

auth::auth(const auth&)
{
}

auth::~auth()
{
}

bool auth::addUser(std::string userName)
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    std::pair< std::unordered_set< std::string >::iterator, bool > ret;
    ret = m_Users.insert(userName);
    return ret.second;
}

bool auth::delUser(std::string userName)
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    return m_Users.erase(userName);
}

std::unordered_set< std::string > auth::getUsers()
{
    std::lock_guard< std::mutex > lock(m_UsersMutex);
    return m_Users;
}

void auth::setLanguage(std::string language)
{
    m_Language = language;
}
std::string auth::getLanguage()
{
    return m_Language;
}

void auth::setWidth(size_t width)
{
    m_Width = width;
}
size_t auth::getWidth()
{
    return m_Width;
}

void auth::setColumns(size_t columns)
{
    m_Columns = columns;
}
size_t auth::getColumns()
{
    return m_Columns;
}

void auth::setBotAccess(size_t botAccess)
{
    m_BotAccess = botAccess;
}
size_t auth::getBotAccess()
{
    return m_BotAccess;
}

void auth::setGod(bool god)
{
    m_God = god;
}
bool auth::getGod()
{
    return m_God;
}
