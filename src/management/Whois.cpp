//
//
//  @ Project : ircppbot
//  @ File Name : Whois.cpp
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

#include "../include/management/Whois.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "../include/core/Global.h"
#include "../include/core/Output.h"

void Whois::AddConsumer(WhoisDataContainerInterface *d)
{
    Consumers.push_back(d);
}

void Whois::DelConsumer(WhoisDataContainerInterface *d)
{
    unsigned int consumer_iterator;
    for (consumer_iterator = Consumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
    	std::string sOutput;
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl;
        if (Consumers[consumer_iterator-1] == d)
        {
			sOutput = "whois consumer removed";
			Output::Instance().addOutput(sOutput, 3);
            Consumers.erase(Consumers.begin() + consumer_iterator-1);
        }
    }
}

void Whois::AddQueue(std::pair< std::string, std::string > data)
{
    unsigned int consumer_iterator;
	for (consumer_iterator = 0; consumer_iterator < Consumers.size(); consumer_iterator++)
	{
		Consumers[consumer_iterator]->AddWhoisQueue(data);
	}
}
