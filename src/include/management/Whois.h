//
//
//  @ Project : ircppbot
//  @ File Name : Whois.h
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

#ifndef SRC_INCLUDE_MANAGEMENT_WHOIS_H_
#define SRC_INCLUDE_MANAGEMENT_WHOIS_H_

#include <queue>
#include <vector>
#include <string>

#include "../interfaces/WhoisDataContainerInterface.h"

class WhoisDataContainerInterface;
class Whois
{
    public:
        static Whois& Instance()
        {
            static Whois obj;
            return obj;
        }

        void init();
        void stop();

        // consumer
        void AddConsumer(WhoisDataContainerInterface *d);
        void DelConsumer(WhoisDataContainerInterface *d);

        // data
        void AddQueue(std::pair< std::string, std::string > data);

    private:
		Whois() {}
		~Whois() {};
        int buffer;

        // consumer lists
        std::vector< WhoisDataContainerInterface * > Consumers;
};

#endif  // SRC_INCLUDE_MANAGEMENT_WHOIS_H_
