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
#include <unordered_set>
#include <atomic>
#include <mutex>

class auth
{
    public:
        auth();
        auth(const auth&);
        ~auth();

        bool addUser(std::string userName);
        bool delUser(std::string userName);
        std::unordered_set< std::string > getUsers();

        // setters and getters
        void setLanguage(std::string language);
        std::string getLanguage();
        void setWidth(size_t width);
        size_t getWidth();
        void setColumns(size_t columns);
        size_t getColumns();
        void setBotAccess(size_t botAccess);
        size_t getBotAccess();
        void setGod(bool god);
        bool getGod();

    private:
        std::string m_Language;
        std::atomic<size_t> m_Width;
        std::atomic<size_t> m_Columns;
        std::atomic<size_t> m_BotAccess;
        std::atomic<bool> m_God;

        std::unordered_set< std::string > m_Users;

        std::mutex m_UsersMutex;
};

#endif // SRC_INCLUDE_MANAGEMENT_AUTH_H

