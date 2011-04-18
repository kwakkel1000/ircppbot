//
//
//  @ Project : ircppbot
//  @ File Name : Data.h
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

#ifndef SRC_INCLUDE_CORE_DATA_H_
#define SRC_INCLUDE_CORE_DATA_H_

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <queue>
#include <string>
#include <vector>

#include "../interfaces/DataInterface.h"


class Data : public DataInterface
{
    public:
        // functions
        Data();
        ~Data();
        void Init(bool getRaw, bool getMode, bool getWhois, bool getPrivmsg);
        void stop();
        void AddRawQueue(std::vector< std::string > data);
        void AddModeQueue(std::vector< std::string > data);
        void AddWhoisQueue(std::vector< std::string > data);
        void AddPrivmsgQueue(std::vector< std::string > data);

        std::vector< std::string > GetRawQueue();
        std::vector< std::string > GetModeQueue();
        std::vector< std::string > GetWhoisQueue();
        std::vector< std::string > GetPrivmsgQueue();

        bool GetRaw();
        bool GetMode();
        bool GetWhois();
        bool GetPrivmsg();

    private:
        // vars
        bool mRun;
        bool mGetRaw;
        bool mGetMode;
        bool mGetWhois;
        bool mGetPrivmsg;

        // dummy vars
        std::vector< std::string > NULLvector;

        // irc data queues
        std::queue< std::vector< std::string > > RawQueue;
        std::queue< std::vector< std::string > > ModeQueue;
        std::queue< std::vector< std::string > > WhoisQueue;
        std::queue< std::vector< std::string > > PrivmsgQueue;

        // threadvars
        boost::condition RawAvailable;
        boost::condition ModeAvailable;
        boost::condition WhoisAvailable;
        boost::condition PrivmsgAvailable;

        boost::mutex RawMutex;
        boost::mutex ModeMutex;
        boost::mutex WhoisMutex;
        boost::mutex PrivmsgMutex;
};

#endif  // SRC_INCLUDE_CORE_DATA_H_
