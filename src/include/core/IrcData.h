//
//
//  @ Project : ircppbot
//  @ File Name : IrcData.h
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

#ifndef SRC_INCLUDE_CORE_IRCDATA_H_
#define SRC_INCLUDE_CORE_IRCDATA_H_

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <queue>
#include <vector>
#include <string>

#include "../interfaces/IrcDataInterface.h"
#include "../interfaces/IrcSocketInterface.h"
#include "../interfaces/DataInterface.h"

class IrcSocketInterface;
class DataInterface;
class IrcData : public IrcDataInterface
{
    public:
        IrcData();
        ~IrcData();
        void init(IrcSocketInterface *s);
        void run();
        void stop();

        // consumer
        bool AddConsumer(DataInterface *d);
        bool DelConsumer(DataInterface *d);

        // data
        void AddSendQueue(std::string data);
        void AddHighPrioritySendQueue(std::string data);
        void AddLowPrioritySendQueue(std::string data);

    private:
        // vars
        unsigned int buffersize;
        IrcSocketInterface *S;

        // floodvars
        bool floodprotect;
        int floodbuffer;
        double floodtime;
        int buffer;

        // consumer lists
        std::vector< DataInterface * > Consumers;
        std::vector< DataInterface * > RawConsumers;
        std::vector< DataInterface * > ModeConsumers;
        std::vector< DataInterface * > WhoisConsumers;
        std::vector< DataInterface * > PrivmsgConsumers;

        // irc thread bools
        bool send;
        bool recv;
        bool parse;

        // threadvars
        boost::shared_ptr<boost::thread> send_thread;
        boost::shared_ptr<boost::thread> recv_thread;
        boost::shared_ptr<boost::thread> parse_thread;
        boost::shared_ptr<boost::thread> flood_thread;

        boost::condition SendAvailable;
        boost::condition RecvAvailable;
        boost::condition floodcondition;

        boost::mutex SendMutex;
        boost::mutex RecvMutex;
        boost::mutex floodmutex;

        // irc queues
        std::queue< std::string > HighPrioritySendQueue;
        std::queue< std::string > LowPrioritySendQueue;
        std::queue< std::string > SendQueue;
        std::queue< std::string > RecvQueue;

        // functions
        // thread functions
        void sendloop();
        void recvloop();
        void Parse();
        void flood_timer();

        // irc functions
        void Send();
        void Recv();

        // queue functions
        std::string GetSendQueue();
        std::string GetRecvQueue();
        void AddRecvQueue(std::string data);
};

#endif  // SRC_INCLUDE_CORE_IRCDATA_H_
