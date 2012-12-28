//
//
//  @ Project : ircppbot
//  @ File Name : irc.h
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

#ifndef SRC_INCLUDE_IRC_H
#define SRC_INCLUDE_IRC_H

#include <queue>
#include <vector>
#include <string>

#include <gframe/socket/tcpsocket.h>
#include "ircdata.h"

class tcpsocket;
class ircdata;
class irc
{
    public:
        static irc& instance()
        {
            static irc ircobj;
            return ircobj;
        }

        void init(tcpsocket &ircsock);
        void run();
        void stop();

        // consumer
        bool addConsumer(ircdata *ircData);
        bool delConsumer(ircdata *ircData);

        // data
        void addSendQueue(std::string data);
        void addHighPrioritySendQueue(std::string data);
        void addLowPrioritySendQueue(std::string data);

    private:
        irc();
        ~irc();

        // vars
        size_t m_BufferSize;
        tcpsocket *m_IrcSocket;

        // floodvars
        bool m_FloodProtect;
        int m_FloodBuffer;
        double m_FloodTime;
        std::atomic<int> m_Buffer;

        // consumer lists
        std::vector< ircdata * > m_Consumers;
        std::vector< ircdata * > m_RawConsumers;
        std::vector< ircdata * > m_EventsConsumers;
        std::vector< ircdata * > m_ModeConsumers;
        std::vector< ircdata * > m_WhoisConsumers;
        std::vector< ircdata * > m_PrivmsgConsumers;

        // irc thread bools
        std::atomic<bool> m_Send;
        std::atomic<bool> m_Recv;
        std::atomic<bool> m_Parse;

        // threadvars
        std::shared_ptr< std::thread > m_SendThread;
        std::shared_ptr< std::thread > m_RecvThread;
        std::shared_ptr< std::thread > m_ParseThread;
        std::shared_ptr< std::thread > m_FloodThread;

        std::condition_variable m_SendAvailable;
        std::condition_variable m_RecvAvailable;
        std::condition_variable m_FloodAvailable;

        std::mutex m_SendMutex;
        std::mutex m_RecvMutex;
        std::mutex m_FloodMutex;

        // irc queues
        std::queue< std::string > m_HighPrioritySendQueue;
        std::queue< std::string > m_LowPrioritySendQueue;
        std::queue< std::string > m_SendQueue;
        std::queue< std::string > m_RecvQueue;

        // functions
        // thread functions
        void sendLoop();
        void recvLoop();
        void parse();
        void floodTimer();

        // irc functions
        void send();
        void recv();

        // queue functions
        std::string getSendQueue();
        std::string getRecvQueue();
        void addRecvQueue(std::string data);
};

#endif  // SRC_INCLUDE_IRC_H
