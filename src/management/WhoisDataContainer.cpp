//
//
//  @ Project : ircppbot
//  @ File Name : WhoisDataContainer.cpp
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

#include "../include/management/WhoisDataContainer.h"
#include <iostream>
#include <string>
#include <map>

WhoisDataContainer::WhoisDataContainer()
{
    mRun = true;
}

WhoisDataContainer::~WhoisDataContainer()
{
    mRun = false;
}

void WhoisDataContainer::Init()
{
    mRun = true;
    while (!WhoisQueue.empty())
    {
        WhoisQueue.pop();
    }
}

void WhoisDataContainer::stop()
{
    mRun = false;
    WhoisAvailable.notify_all();
}

// producers

void WhoisDataContainer::AddWhoisQueue(std::pair< std::string, std::string > data)
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    WhoisQueue.push(data);
    std::cout << "AddWhoisQueue nick " << data.first << " channel " << data.second << std::endl;
    WhoisAvailable.notify_one();
}

// consumers

std::pair< std::string, std::string > WhoisDataContainer::GetWhoisQueue()
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    while (WhoisQueue.empty() && mRun)
    {
    	std::cout << "WhoisDataContainer lock" << std::endl;
        WhoisAvailable.wait(lock);
    }
    std::cout << "WhoisDataContainer unlock" << std::endl;
    if (!WhoisQueue.empty())
    {
    	std::cout << "WhoisDataContainer not empty" << std::endl;
		std::pair< std::string, std::string > temp;
		temp = WhoisQueue.front();
        WhoisQueue.pop();
        return temp;
    }
    return std::pair< std::string, std::string >("", "");
}
