//
//
//  @ Project : ircppbot
//  @ File Name : help.h
//  @ Date : 13-01-2013
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

#ifndef SRC_INCLUDE_HELP_H
#define SRC_INCLUDE_HELP_H

#include <map>
#include <string>

class help
{
    public:
        static help& instance()
        {
            static help obj;
            return obj;
        }

        void init();
        bool setHelp(std::string alias, std::string command, int access, std::string who);
        bool delHelps(std::string who);
        bool delHelp(std::string alias, std::string who);
        bool getHelps(std::map< std::string, std::map< std::string, bindelement > >& bindElements);
        bool getHelps(std::map< std::string, bindelement >& bindElements, std::string who);
        bool getHelp(std::string command, std::string who);

    private:
        help();
        ~help();


        std::map< std::string, std::map< std::string, bindelement > > m_help;
};

#endif  // SRC_INCLUDE_IRC_H
