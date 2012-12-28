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
    m_GetRaw(false),
    m_GetEvents(false),
    m_GetMode(false),
    m_GetWhois(false),
    m_GetPrivmsg(false)
{
}

ircdata::~ircdata()
{
    m_Run = false;
    m_RawAvailable.notify_all();
    m_EventsAvailable.notify_all();
    m_ModeAvailable.notify_all();
    m_WhoisAvailable.notify_all();
    m_PrivmsgAvailable.notify_all();
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

void ircdata::setMode(bool mode)
{
    m_GetMode = mode;
}
bool ircdata::getMode()
{
    return m_GetMode;
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

void ircdata::addModeQueue(std::vector<std::string> data)
{
    std::unique_lock<std::mutex> lock(m_ModeMutex);
    m_ModeQueue.push(data);
    lock.unlock();
    m_ModeAvailable.notify_one();
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
    while (m_RawQueue.empty() && m_Run)
    {
        m_RawAvailable.wait(lock);
    }
    if (!m_RawQueue.empty())
    {
        std::vector< std::string > temp = m_RawQueue.front();
        m_RawQueue.pop();
        lock.unlock();
        return temp;
    }
    lock.unlock();
    return NULLvector;
}

std::vector< std::string > ircdata::getEventsQueue()
{
    std::unique_lock<std::mutex> lock(m_EventsMutex);
    while (m_EventsQueue.empty() && m_Run)
    {
        m_EventsAvailable.wait(lock);
    }
    if (!m_EventsQueue.empty())
    {
        std::vector< std::string > temp = m_EventsQueue.front();
        m_EventsQueue.pop();
        lock.unlock();
        return temp;
    }
    lock.unlock();
    return NULLvector;
}

std::vector< std::string > ircdata::getModeQueue()
{
    std::unique_lock<std::mutex> lock(m_ModeMutex);
    while (m_ModeQueue.empty() && m_Run)
    {
        m_ModeAvailable.wait(lock);
    }
    if (!m_ModeQueue.empty())
    {
        std::vector< std::string > temp = m_ModeQueue.front();
        m_ModeQueue.pop();
        lock.unlock();
        return temp;
    }
    lock.unlock();
    return NULLvector;
}

std::vector< std::string > ircdata::getWhoisQueue()
{
    std::unique_lock<std::mutex> lock(m_WhoisMutex);
    while (m_WhoisQueue.empty() && m_Run)
    {
        m_WhoisAvailable.wait(lock);
    }
    if (!m_WhoisQueue.empty())
    {
        std::vector< std::string > temp = m_WhoisQueue.front();
        m_WhoisQueue.pop();
        lock.unlock();
        return temp;
    }
    lock.unlock();
    return NULLvector;
}

std::vector< std::string > ircdata::getPrivmsgQueue()
{
    std::unique_lock<std::mutex> lock(m_PrivmsgMutex);
    while (m_PrivmsgQueue.empty() && m_Run)
    {
        m_PrivmsgAvailable.wait(lock);
    }
    if (!m_PrivmsgQueue.empty())
    {
        std::vector< std::string > temp = m_PrivmsgQueue.front();
        m_PrivmsgQueue.pop();
        lock.unlock();
        return temp;
    }
    lock.unlock();
    std::vector< std::string > temp;
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    return temp;
}
