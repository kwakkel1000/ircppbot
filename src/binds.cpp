//
//
// @ Project : ircppbot
// @ File Name : binds.cpp
// @ Date : 08-01-2013
// @ Author : Gijs Kwakkel
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

#include "include/binds.h"
#include <gframe/output.h>
#include <gframe/glib.h>
#include <algorithm>

bool binds::setBind(std::string alias, std::string command, int access, std::string who)
{
    std::transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int)) std::tolower);
    std::transform(command.begin(), command.end(), command.begin(), (int(*)(int)) std::tolower);
    std::transform(who.begin(), who.end(), who.begin(), (int(*)(int)) std::tolower);
    bindelement tmpBind;
    tmpBind.command = command;
    tmpBind.access = access;
    m_Binds[who][alias] = tmpBind;
    return true;
}

bool binds::delBinds(std::string who)
{
    std::transform(who.begin(), who.end(), who.begin(), (int(*)(int)) std::tolower);
    return m_Binds.erase(who);
}

bool binds::delBind(std::string alias, std::string who)
{
    std::transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int)) std::tolower);
    std::transform(who.begin(), who.end(), who.begin(), (int(*)(int)) std::tolower);
    std::map< std::string, std::map< std::string, bindelement > >::iterator l_BindsIterator;
    l_BindsIterator = m_Binds.find(who);
    if (l_BindsIterator == m_Binds.end())
    {
        output::instance().addStatus(false, "bool binds::delBind(std::string alias, std::string who) who not found");
        return false;
    }
    return m_Binds[who].erase(alias);
}

bool binds::getBinds(std::map< std::string, std::map< std::string, bindelement > >& bindElements)
{
    bindElements = m_Binds;
    return true;
}

bool binds::getBinds(std::map< std::string, bindelement >& bindElements, std::string who)
{
    std::transform(who.begin(), who.end(), who.begin(), (int(*)(int)) std::tolower);
    std::map< std::string, std::map< std::string, bindelement > >::iterator l_BindsIterator;
    l_BindsIterator = m_Binds.find(who);
    if (l_BindsIterator == m_Binds.end())
    {
        output::instance().addStatus(false, "bool binds::getBinds(std::map< std::string, bindelement >& bindElements, std::string who) who not found");
        return false;
    }
    bindElements = (*l_BindsIterator).second;
    return true;
}

bool binds::getBind(bindelement& bindElement, std::string alias, std::string who)
{
    std::transform(alias.begin(), alias.end(), alias.begin(), (int(*)(int)) std::tolower);
    std::transform(who.begin(), who.end(), who.begin(), (int(*)(int)) std::tolower);
    output::instance().addOutput("bool binds::getBind(bindelement& bindElement, std::string alias, std::string who) alias: " + alias + " who: " + who, 12);
    std::map< std::string, std::map< std::string, bindelement > >::iterator l_BindsIterator;
    std::map< std::string, bindelement > l_Alias;
    std::map< std::string, bindelement >::iterator l_AliasIterator;
    l_BindsIterator = m_Binds.find(who);
    if (l_BindsIterator == m_Binds.end())
    {
        output::instance().addStatus(false, "bool binds::getBind(bindelement& bindElement, std::string alias, std::string who) who not found");
        return false;
    }
    l_Alias = (*l_BindsIterator).second;
    l_AliasIterator = l_Alias.find(alias);
    if (l_AliasIterator == l_Alias.end())
    {
        output::instance().addStatus(false, "bool binds::getBind(bindelement& bindElement, std::string alias, std::string who) command not found");
        return false;
    }
    bindElement = (*l_AliasIterator).second;
    return true;
}

binds::binds()
{
}

binds::~binds()
{
}
