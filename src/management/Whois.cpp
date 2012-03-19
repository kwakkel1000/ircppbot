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

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "../include/core/Global.h"
#include "../include/core/Output.h"
#include "../include/core/BotLib.h"

bool Whois::AddConsumer(WhoisDataContainerInterface *pWhoisDataContainerInterface)
{
    if (!pWhoisDataContainerInterface)
    {
        return false;
    }
    vConsumers.push_back(pWhoisDataContainerInterface);
    return true;
}

bool Whois::DelConsumer(WhoisDataContainerInterface *pWhoisDataContainerInterface)
{
    if (!pWhoisDataContainerInterface)
    {
        Output::Instance().addOutput("Whois::DelConsumer: Trying to delete NULL!", 1);
        return false;
    }
    // Note: Replace array-access by iterators? Won't have to worry about erasing the correct item then
    std::string sOutput;
    unsigned int consumer_iterator;
    for (consumer_iterator = vConsumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl; // Why use cout AND addOutput?
        if (vConsumers[consumer_iterator-1] == pWhoisDataContainerInterface)
        {
            sOutput = "whois consumer removed";
            Output::Instance().addOutput(sOutput, 1);
            vConsumers.erase(vConsumers.begin() + consumer_iterator-1);
            std::string sOutput = "vConsumers.size() " + BotLib::StringFromInt(vConsumers.size());
            Output::Instance().addOutput(sOutput, 1);
        }
    }
    return true;
}

void Whois::AddQueue(std::pair< std::string, std::string > pData)
{
    std::string sOutput = "vConsumers.size() " + BotLib::StringFromInt(vConsumers.size());
    Output::Instance().addOutput(sOutput, 1);
    unsigned int consumer_iterator;
    //pConsumer->AddWhoisQueue(pData);
    for (consumer_iterator = 0; consumer_iterator < vConsumers.size(); consumer_iterator++)
    {
        vConsumers[consumer_iterator]->AddWhoisQueue(pData);
    }
}
