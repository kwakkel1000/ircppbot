//
//
//  @ Project : ircppbot
//  @ File Name : users.h
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


#ifndef SRC_INCLUDE_MANAGEMENT_USERS_H
#define SRC_INCLUDE_MANAGEMENT_USERS_H

#include "user.h"
#include <string>
#include <map>

class user;
class users
{
public:
        static users& instance()
        {
            static users obj;
            return obj;
        }
        bool addUser(std::string userName);
        bool delUser(std::string userName);
        bool renameUser(std::string oldUserName, std::string newUserName);

        bool findUser(std::string userName);
        bool getUsers(std::map< std::string, user > &userList);
        user& getUser(std::string userName);

        void setBotNick(std::string botNick);
        std::string getBotNick();

    private:
        users();
        ~users();

        std::string m_BotNick;
        std::map< std::string, user > m_UserList;
};

#endif // SRC_INCLUDE_MANAGEMENT_USERS_H
