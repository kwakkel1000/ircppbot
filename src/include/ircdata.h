//
//
//  @ Project : ircppbot
//  @ File Name : ircdata.h
//  @ Date : 25-12-2012
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

#ifndef SRC_INCLUDE_IRCDATA_H
#define SRC_INCLUDE_IRCDATA_H

// threading
#include <mutex>
#include <memory>
#include <thread>
#include <condition_variable>
#include <atomic>

#include <queue>
#include <string>
#include <vector>

class ircdata
{
    public:
        // functions
        ircdata();
        ~ircdata();
        void stop();
        void addRawQueue(std::vector< std::string > data);
        void addEventsQueue(std::vector< std::string > data);
        void addModesQueue(std::vector< std::string > data);
        void addWhoisQueue(std::vector< std::string > data);
        void addPrivmsgQueue(std::vector< std::string > data);

        std::vector< std::string > getRawQueue();
        std::vector< std::string > getEventsQueue();
        std::vector< std::string > getModesQueue();
        std::vector< std::string > getWhoisQueue();
        std::vector< std::string > getPrivmsgQueue();

        void setRaw(bool raw);
        bool getRaw();
        void setEvents(bool events);
        bool getEvents();
        void setModes(bool modes);
        bool getModes();
        void setWhois(bool whois);
        bool getWhois();
        void setPrivmsg(bool privmsg);
        bool getPrivmsg();

    private:
        // vars
        std::atomic<bool> m_Stop;
        std::atomic<bool> m_GetRaw;
        std::atomic<bool> m_GetEvents;
        std::atomic<bool> m_GetModes;
        std::atomic<bool> m_GetWhois;
        std::atomic<bool> m_GetPrivmsg;

        // dummy vars
        std::vector< std::string > NULLvector;

        // irc data queues
        std::queue< std::vector< std::string > > m_RawQueue;
        std::queue< std::vector< std::string > > m_EventsQueue;
        std::queue< std::vector< std::string > > m_ModesQueue;
        std::queue< std::vector< std::string > > m_WhoisQueue;
        std::queue< std::vector< std::string > > m_PrivmsgQueue;

        std::shared_ptr< std::thread > m_QueryThread;
        std::shared_ptr< std::thread > m_CounterThread;

        // threadvars
        std::condition_variable m_RawAvailable;
        std::condition_variable m_EventsAvailable;
        std::condition_variable m_ModesAvailable;
        std::condition_variable m_WhoisAvailable;
        std::condition_variable m_PrivmsgAvailable;

        std::mutex m_RawMutex;
        std::mutex m_EventsMutex;
        std::mutex m_ModesMutex;
        std::mutex m_WhoisMutex;
        std::mutex m_PrivmsgMutex;
};

#endif  // SRC_INCLUDE_IRCDATA_H
