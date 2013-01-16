//
//
// @ Project : ircppbot
// @ File Name : binds.h
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


#ifndef SRC_INCLUDE_BINDS_H
#define SRC_INCLUDE_BINDS_H

#include <map>
#include <string>
#include <mutex>

class binds
{
    public:
        static binds& instance()
        {
            static binds obj;
            return obj;
        }
        struct bindelement {
            std::string command;
            int access;
        };

        void init();
        bool setBind(std::string alias, std::string command, int access, std::string who);
        bool delBinds(std::string who);
        bool delBind(std::string alias, std::string who);
        bool getBinds(std::map< std::string, std::map< std::string, bindelement > >& bindElements);
        bool getBinds(std::map< std::string, bindelement >& bindElements, std::string who);
        bool getBind(bindelement& bindElement, std::string alias, std::string who);

    private:
        binds();
        ~binds();


        std::map< std::string, std::map< std::string, bindelement > > m_Binds;

        std::mutex m_BindMutex;

};

#endif // SRC_INCLUDE_BINDS_H

