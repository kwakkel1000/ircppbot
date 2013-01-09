//
//
//  @ Project : ircppbot
//  @ File Name : auths.cpp
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


#include "../include/management/auths.h"
#include <gframe/output.h>
#include <cstddef>

auths::auths()
{
}

auths::~auths()
{
    /*std::map< std::string, auth >::iterator m_AuthListIterator;
    for (m_AuthListIterator = m_AuthList.begin(); m_AuthListIterator < m_AuthList.end(); ++m_AuthListIterator)
    {
        delAuth((*m_AuthListIterator).first);
    }*/
}

bool auths::addAuth(std::string userAuth)
{
    std::pair< std::map< std::string, auth >::iterator, bool > ret;
    ret = m_AuthList.insert (std::pair< std::string, auth >(userAuth, auth()));
    if (ret.second)
    {
        output::instance().addStatus(true, "bool auths::addAuth(std::string userAuth) add: " + userAuth);
        return true;
    }
    //output::instance().addStatus(false, "bool auths::addAuth(std::string userAuth) auth already exists");
    return false;
}

bool auths::delAuth(std::string userAuth)
{
    size_t ret = m_AuthList.erase(userAuth);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool auths::delAuth(std::string userAuth) auth found, erase succesfull: " + userAuth);
        return true;
    }
    output::instance().addStatus(false, "bool auths::delAuth(std::string userAuth) auth found, erase failed: " + userAuth);
    return false;
}

bool auths::renameAuth(std::string oldAuthName, std::string newAuthName)
{
    auth tmpAuth = getAuth(oldAuthName);
    std::pair< std::map< std::string, auth >::iterator, bool > ret;
    ret = m_AuthList.insert (std::pair< std::string, auth >(newAuthName, tmpAuth));
    if (ret.second)
    {
        delAuth(oldAuthName);
        return true;
    }
    output::instance().addStatus(false, "bool auths::renameAuth(std::string oldAuthName, std::string newAuthName) auth already exists");
    return false;
}

bool auths::findAuth(std::string userAuth)
{
    std::map< std::string, auth >::iterator l_AuthListIterator;
    l_AuthListIterator = m_AuthList.find(userAuth);
    if (l_AuthListIterator == m_AuthList.end())
    {
        return false;
    }
    return true;
}

bool auths::getAuths(std::map< std::string, auth > &authList)
{
    authList = m_AuthList;
    return true;
}

auth& auths::getAuth(std::string userAuth)
{
    std::map< std::string, auth >::iterator m_AuthListIterator;
    m_AuthListIterator = m_AuthList.find(userAuth);
    if (m_AuthListIterator == m_AuthList.end())
    {
        output::instance().addStatus(false, "auth& auths::getAuth(std::string userAuth) auth " + userAuth + " not found");
        exit(EXIT_FAILURE);
    }
    return (*m_AuthListIterator).second;
}
