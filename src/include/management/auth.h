//
//
//  @ Project : ircppbot
//  @ File Name : auth.h
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


#ifndef SRC_INCLUDE_MANAGEMENT_AUTH_H
#define SRC_INCLUDE_MANAGEMENT_AUTH_H
#include <string>
#include <map>
#include <atomic>
#include <mutex>

#include "user.h"

class user;
class auth
{
    public:
        auth();
        auth(const auth&);
        ~auth();

        bool addUser(std::string userName, user* userPointer);
        bool delUser(std::string userName);
        bool getUsers(std::map< std::string, user* >& usersMap);

        // setters and getters
        void setLanguage(std::string language);
        std::string getLanguage();
        void setWidth(std::atomic<size_t> width);
        std::atomic<size_t> getWidth();
        void setColumns(std::atomic<size_t> columns);
        std::atomic<size_t> getColumns();
        void setBotAccess(std::atomic<size_t> botAccess);
        std::atomic<size_t> getBotAccess();
        void setGod(std::atomic<bol> god);
        std::atomic<bool> getGod();

    private:
        std::string m_Language;
        std::atomic<size_t> m_Width;
        std::atomic<size_t> m_Columns;
        std::atomic<size_t> m_BotAccess;
        std::atomic<bool> m_God;

        std::map< std::string, user* > m_Users;
};

#endif // SRC_INCLUDE_MANAGEMENT_AUTH_H

