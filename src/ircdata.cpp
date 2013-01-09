//
//
//  @ Project : ircppbot
//  @ File Name : ircdata.cpp
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

#include "include/ircdata.h"

ircdata::ircdata() :
    m_Stop(false),
    m_GetRaw(false),
    m_GetEvents(false),
    m_GetModes(false),
    m_GetWhois(false),
    m_GetPrivmsg(false),
    m_RawQueue(),
    m_EventsQueue(),
    m_ModesQueue(),
    m_WhoisQueue(),
    m_PrivmsgQueue()
{
    m_GetRaw = false;
    m_GetEvents = false;
    m_GetModes = false;
    m_GetWhois = false;
    m_GetPrivmsg = false;
}

ircdata::~ircdata()
{
    stop();
}

void ircdata::stop()
{
    m_Stop = true;
    if (m_GetRaw == true)
    {
        m_RawAvailable.notify_one();
    }
    if (m_GetEvents == true)
    {
        m_EventsAvailable.notify_one();
    }
    if (m_GetModes == true)
    {
        m_ModesAvailable.notify_one();
    }
    if (m_GetWhois == true)
    {
        m_WhoisAvailable.notify_one();
    }
    if (m_GetPrivmsg == true)
    {
        m_PrivmsgAvailable.notify_one();
    }
}

void ircdata::setRaw(bool raw)
{
    m_GetRaw = raw;
}
bool ircdata::getRaw()
{
    return m_GetRaw;
}

void ircdata::setEvents(bool events)
{
    m_GetEvents = events;
}
bool ircdata::getEvents()
{
    return m_GetEvents;
}

void ircdata::setModes(bool modes)
{
    m_GetModes = modes;
}
bool ircdata::getModes()
{
    return m_GetModes;
}

void ircdata::setWhois(bool whois)
{
    m_GetWhois = whois;
}
bool ircdata::getWhois()
{
    return m_GetWhois;
}

void ircdata::setPrivmsg(bool privmsg)
{
    m_GetPrivmsg = privmsg;
}
bool ircdata::getPrivmsg()
{
    return m_GetPrivmsg;
}

// producers
void ircdata::addRawQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_RawMutex);
    m_RawQueue.push(data);
    lock.unlock();
    m_RawAvailable.notify_one();
}

void ircdata::addEventsQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_EventsMutex);
    m_EventsQueue.push(data);
    lock.unlock();
    m_EventsAvailable.notify_one();
}

void ircdata::addModesQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_ModesMutex);
    m_ModesQueue.push(data);
    lock.unlock();
    m_ModesAvailable.notify_one();
}

void ircdata::addWhoisQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_WhoisMutex);
    m_WhoisQueue.push(data);
    lock.unlock();
    m_WhoisAvailable.notify_one();
}

void ircdata::addPrivmsgQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_PrivmsgMutex);
    m_PrivmsgQueue.push(data);
    lock.unlock();
    m_PrivmsgAvailable.notify_one();
}

// consumers
std::vector< std::string > ircdata::getRawQueue()
{
    std::unique_lock<std::mutex> lock(m_RawMutex);
    while (!m_Stop && m_RawQueue.empty())
    {
        m_RawAvailable.wait(lock);
    }
    if (m_Stop)
    {
        return NULLvector;
    }
    std::vector< std::string > temp = m_RawQueue.front();
    m_RawQueue.pop();
    lock.unlock();
    return temp;
}

std::vector< std::string > ircdata::getEventsQueue()
{
    std::unique_lock<std::mutex> lock(m_EventsMutex);
    while (!m_Stop && m_EventsQueue.empty())
    {
        m_EventsAvailable.wait(lock);
    }
    if (m_Stop)
    {
        return NULLvector;
    }
    std::vector< std::string > temp = m_EventsQueue.front();
    m_EventsQueue.pop();
    lock.unlock();
    return temp;
}

std::vector< std::string > ircdata::getModesQueue()
{
    std::unique_lock<std::mutex> lock(m_ModesMutex);
    while (!m_Stop && m_ModesQueue.empty())
    {
        m_ModesAvailable.wait(lock);
    }
    if (m_Stop)
    {
        return NULLvector;
    }
    std::vector< std::string > temp = m_ModesQueue.front();
    m_ModesQueue.pop();
    lock.unlock();
    return temp;
}

std::vector< std::string > ircdata::getWhoisQueue()
{
    std::unique_lock<std::mutex> lock(m_WhoisMutex);
    while (!m_Stop && m_WhoisQueue.empty())
    {
        m_WhoisAvailable.wait(lock);
    }
    if (m_Stop)
    {
        return NULLvector;
    }
    std::vector< std::string > temp = m_WhoisQueue.front();
    m_WhoisQueue.pop();
    lock.unlock();
    return temp;
}

std::vector< std::string > ircdata::getPrivmsgQueue()
{
    std::unique_lock<std::mutex> lock(m_PrivmsgMutex);
    while (!m_Stop && m_PrivmsgQueue.empty())
    {
        m_PrivmsgAvailable.wait(lock);
    }
    if (m_Stop)
    {
        std::vector< std::string > temp;
        temp.push_back("");
        temp.push_back("");
        temp.push_back("");
        temp.push_back("");
        return temp;
    }
    std::vector< std::string > temp = m_PrivmsgQueue.front();
    m_PrivmsgQueue.pop();
    lock.unlock();
    return temp;
}
