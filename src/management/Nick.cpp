//
//
//  @ Project : ircppbot
//  @ File Name : Nick.cpp
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


#include "../include/management/Nick.h"
#include <boost/algorithm/string.hpp>

/**
 * Constructor
 * @param sNickName NickName of the user
 *
 */
Nick::Nick(std::string sNick)
{
    SetNick(sNick);
}

/**
 * Destructor
 *
 */
Nick::~Nick()
{
}

/**
 * HasPermission
 * determine if we have the permission
 * @param sPermission  the permission to compare
 * @return  true if we have the permssion
 */
bool Nick::HasPermission(std::string sPermission) const
{
    for (unsigned int _uiChannelPermissionsIndex = 0; _uiChannelPermissionsIndex < m_vsChannelPermissions.size(); _uiChannelPermissionsIndex++)
    {
        if (boost::iequals(m_vsChannelPermissions[_uiChannelPermissionsIndex], sPermission))
        {
            return true;
        }
    }
    return false;
}

/**
 * AddPermission
 * add a permission
 * @param sPermission  the permission to add
 * @return  true if we could add the permission
 */
bool Nick::AddPermission(std::string sPermission)
{
    if (!sPermission || HasPermission(sPermission))
    {
        return false;
    }

    m_vsChannelPermissions.push_back(sPermission);
    return true;
}

/**
 * RemPermission
 * remove a permission
 * @param sPermission  the permission to remove
 * @return  true if we could remove the permssion
 */
bool Nick::RemPermission(std::string sPermission)
{
    if (!sPermission)
    {
        return false;
    }
    for (unsigned int _uiChannelPermissionsIndex = 0; _uiChannelPermissionsIndex < m_vsChannelPermissions.size(); _uiChannelPermissionsIndex++)
    {
        if (boost::iequals(m_vsChannelPermissions[_uiChannelPermissionsIndex], sPermission))
        {
            m_vsChannelPermissions.erase(m_vsChannelPermissions.begin() + _uiChannelPermissionsIndex);
            return true;
        }
    }
    return false;
}
