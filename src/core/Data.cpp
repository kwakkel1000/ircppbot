//
//
//  @ Project : ircppbot
//  @ File Name : Data.cpp
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

#include "../include/core/Data.h"
#include <iostream>
#include <string>
#include <vector>

Data::Data()
{
}

Data::~Data()
{
}

void Data::Init(bool getRaw, bool getMode, bool getWhois, bool getPrivmsg)
{
    mRun = true;
    mGetRaw = getRaw;
    mGetMode = getMode;
    mGetWhois = getWhois;
    mGetPrivmsg = getPrivmsg;
}

void Data::stop()
{
    mRun = false;
    RawAvailable.notify_all();
    ModeAvailable.notify_all();
    WhoisAvailable.notify_all();
    PrivmsgAvailable.notify_all();
}
bool Data::GetRaw()
{
    return mGetRaw;
}

bool Data::GetMode()
{
    return mGetMode;
}

bool Data::GetWhois()
{
    return mGetWhois;
}

bool Data::GetPrivmsg()
{
    return mGetPrivmsg;
}

// producers
void Data::AddRawQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(RawMutex);
    RawQueue.push(data);
    RawAvailable.notify_one();
}

void Data::AddModeQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(ModeMutex);
    ModeQueue.push(data);
    ModeAvailable.notify_one();
}

void Data::AddWhoisQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    WhoisQueue.push(data);
    WhoisAvailable.notify_one();
}

void Data::AddPrivmsgQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(PrivmsgMutex);
    PrivmsgQueue.push(data);
    PrivmsgAvailable.notify_one();
}

// consumers
std::vector< std::string > Data::GetRawQueue()
{
    boost::mutex::scoped_lock lock(RawMutex);
    // std::cout << "GetRawQueue before lock " << std::endl;
    while (RawQueue.empty() && mRun)
    {
        RawAvailable.wait(lock);
    }
    if (!RawQueue.empty())
    {
        std::vector< std::string > temp = RawQueue.front();
        RawQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetModeQueue()
{
    boost::mutex::scoped_lock lock(ModeMutex);
    while (ModeQueue.empty() && mRun)
    {
        ModeAvailable.wait(lock);
    }
    if (!ModeQueue.empty())
    {
        std::vector< std::string > temp = ModeQueue.front();
        ModeQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetWhoisQueue()
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    while (WhoisQueue.empty() && mRun)
    {
        WhoisAvailable.wait(lock);
    }
    if (!WhoisQueue.empty())
    {
        std::vector< std::string > temp = WhoisQueue.front();
        WhoisQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetPrivmsgQueue()
{
    boost::mutex::scoped_lock lock(PrivmsgMutex);
    while (PrivmsgQueue.empty() && mRun)
    {
        PrivmsgAvailable.wait(lock);
    }
    if (!PrivmsgQueue.empty())
    {
        std::vector< std::string > temp = PrivmsgQueue.front();
        PrivmsgQueue.pop();
        return temp;
    }
    std::vector< std::string > temp;
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    return temp;
}
