//
//
//  @ Project : ircppbot
//  @ File Name : auths.h
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


#ifndef SRC_INCLUDE_MANAGEMENT_AUTHS_H
#define SRC_INCLUDE_MANAGEMENT_AUTHS_H

#include "auth.h"
#include <string>
#include <map>

class auth;
class auths
{
public:
        static auths& instance()
        {
            static auths obj;
            return obj;
        }
        bool addAuth(std::string userAuth);
        bool delAuth(std::string userAuth);
        bool renameAuth(std::string oldUserAuth, std::string newUserAuth);

        bool findAuth(std::string userAuth);
        bool getAuths(std::map< std::string, auth > &authList);
        auth& getAuth(std::string userAuth);


    private:
        auths();
        ~auths();

        std::map< std::string, auth > m_AuthList;
};

#endif // SRC_INCLUDE_MANAGEMENT_AUTHS_H
