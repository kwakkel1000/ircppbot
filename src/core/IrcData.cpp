//
//
//  @ Project : ircppbot
//  @ File Name : IcrData.cpp
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

#include "../include/core/IrcData.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "../include/core/Global.h"
#include "../include/core/Output.h"


IrcData::IrcData()
{
}

IrcData::~IrcData()
{
    stop();
}

void IrcData::stop()
{
    send = false;
    recv = false;
    parse = false;
    while (!HighPrioritySendQueue.empty())
    {
        HighPrioritySendQueue.pop();
    }
    while (!LowPrioritySendQueue.empty())
    {
        LowPrioritySendQueue.pop();
    }
    while (!SendQueue.empty())
    {
        SendQueue.pop();
    }
    while (!RecvQueue.empty())
    {
        RecvQueue.pop();
    }
    SendAvailable.notify_all();
    RecvAvailable.notify_all();
    floodcondition.notify_all();
    std::cout << "void IrcData::stop() joining threads" << std::endl;
    if (floodprotect)
    {
        flood_thread->join();
        std::cout << "flood_thread stopped" << std::endl;
    }
    send_thread->join();
    std::cout << "send_thread stopped" << std::endl;
    recv_thread->join();
    std::cout << "recv_thread stopped" << std::endl;
    parse_thread->join();
    std::cout << "parse_thread stopped" << std::endl;
}

void IrcData::init(IrcSocketInterface *s)
{
    // delete S;
    S = s;
    while (!HighPrioritySendQueue.empty())
    {
        HighPrioritySendQueue.pop();
    }
    while (!LowPrioritySendQueue.empty())
    {
        LowPrioritySendQueue.pop();
    }
    while (!SendQueue.empty())
    {
        SendQueue.pop();
    }
    while (!RecvQueue.empty())
    {
        RecvQueue.pop();
    }
    send = true;
    recv = true;
    parse = true;
    std::string protect_string = Global::Instance().get_ConfigReader().GetString("floodprotect");
    std::string buffer_string = Global::Instance().get_ConfigReader().GetString("floodbuffer");
    std::string time_string = Global::Instance().get_ConfigReader().GetString("floodtime");
    floodprotect = false;
    if (boost::iequals(protect_string, "true"))
    {
        std::stringstream bufferss(buffer_string);
        std::stringstream timess(time_string);
        floodprotect = true;
        bufferss >> floodbuffer;
        timess >> floodtime;
        buffer = 1;
        assert(!flood_thread);
        flood_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::flood_timer, this)));
    }
}

void IrcData::AddConsumer(DataInterface *d)
{
    // Consumers.push_back(d);
    if (d->GetRaw() == true)
    {
        RawConsumers.push_back(d);
    }
    if (d->GetMode() == true)
    {
        ModeConsumers.push_back(d);
    }
    if (d->GetWhois() == true)
    {
        WhoisConsumers.push_back(d);
    }
    if (d->GetPrivmsg() == true)
    {
        PrivmsgConsumers.push_back(d);
    }
}

void IrcData::DelConsumer(DataInterface *d)
{
    unsigned int consumer_iterator;
    std::cout << "RawConsumers.size() " << RawConsumers.size() << std::endl;
    for (consumer_iterator = RawConsumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl;
        if (RawConsumers[consumer_iterator-1] == d)
        {
            std::cout << "consumer removed" << std::endl;
            RawConsumers.erase(RawConsumers.begin() + consumer_iterator-1);
        }
    }
    std::cout << "ModeConsumers.size() " << ModeConsumers.size() << std::endl;
    for (consumer_iterator = ModeConsumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl;
        if (ModeConsumers[consumer_iterator-1] == d)
        {
            std::cout << "consumer removed" << std::endl;
            ModeConsumers.erase(ModeConsumers.begin() + consumer_iterator-1);
        }
    }
    std::cout << "WhoisConsumers.size() " << WhoisConsumers.size() << std::endl;
    for (consumer_iterator = WhoisConsumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl;
        if (WhoisConsumers[consumer_iterator-1] == d)
        {
            std::cout << "consumer removed" << std::endl;
            WhoisConsumers.erase(WhoisConsumers.begin() + consumer_iterator-1);
        }
    }
    std::cout << "PrivmsgConsumers.size() " << PrivmsgConsumers.size() << std::endl;
    for (consumer_iterator = PrivmsgConsumers.size(); consumer_iterator > 0; consumer_iterator--)
    {
        std::cout << "consumer_iterator " << consumer_iterator-1 << std::endl;
        if (PrivmsgConsumers[consumer_iterator-1] == d)
        {
            std::cout << "consumer removed" << std::endl;
            PrivmsgConsumers.erase(PrivmsgConsumers.begin() + consumer_iterator-1);
        }
    }
}

void IrcData::run()
{
    assert(!send_thread);
    send_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::sendloop, this)));
    assert(!recv_thread);
    recv_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::recvloop, this)));
    assert(!parse_thread);
    parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::Parse, this)));
}

void IrcData::AddHighPrioritySendQueue(std::string data)
{
    boost::mutex::scoped_lock lock(SendMutex);
    HighPrioritySendQueue.push(data);
    SendAvailable.notify_one();
}

void IrcData::AddSendQueue(std::string data)
{
    boost::mutex::scoped_lock lock(SendMutex);
    SendQueue.push(data);
    SendAvailable.notify_one();
}

void IrcData::AddLowPrioritySendQueue(std::string data)
{
    boost::mutex::scoped_lock lock(SendMutex);
    LowPrioritySendQueue.push(data);
    SendAvailable.notify_one();
}

// private
void IrcData::sendloop()
{
    while (send == true)
    {
        Send();
    }
    std::string sOutput = "void IrcData::sendloop() after while()";
    Output::Instance().addOutput(sOutput, 2);
}

void IrcData::recvloop()
{
    while (recv == true)
    {
        Recv();
    }
    std::string sOutput = "void IrcData::sendloop() after while()";
    Output::Instance().addOutput(sOutput, 2);
}

void IrcData::Send()
{
    if (send == true)
    {
        std::string data;
        std::string sOutput;
        if (floodprotect)
        {
            boost::mutex::scoped_lock lock(floodmutex);
            while (buffer <= 0)
            {
                floodcondition.wait(lock);
            }
            data = GetSendQueue();
            sOutput = ">> " + data;
            data = data + "\r\n";
            Output::Instance().addOutput(sOutput, 5);
            // try
            {
                if (send)
                {
                    if (boost::iequals(Global::Instance().get_ConfigReader().GetString("chandebug"), "true"))
                    {
                        S->Send(Global::Instance().get_Reply().irc_privmsg(Global::Instance().get_ConfigReader().GetString("debugchannel"), "[" + Output::Instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] " + data));
                        //std::string tmpdata = "PRIVMSG " + Global::Instance().get_ConfigReader().GetString("debugchannel") + " :" + data;
                        //S->Send(tmpdata);
                    }
                    buffer--;
                    S->Send(data);
                }
            }
            /*catch (IrcSocketInterface::Exception& e)
            {
                std::cout << "Exception caught: " << e.Description() << std::endl;
            }*/
        }
        else
        {
            data = GetSendQueue();
            sOutput = ">> " + data;
            data = data + "\r\n";
            Output::Instance().addOutput(sOutput, 5);
            // try
            {
                if (send)
                {
                    if (boost::iequals(Global::Instance().get_ConfigReader().GetString("chandebug"), "true"))
                    {
                        S->Send(Global::Instance().get_Reply().irc_privmsg(Global::Instance().get_ConfigReader().GetString("debugchannel"), "[" + Output::Instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] " + data));
                        //std::string tmpdata = "PRIVMSG " + Global::Instance().get_ConfigReader().GetString("debugchannel") + " :" + data;
                        //S->Send(tmpdata);
                    }
                    S->Send(data);
                }
            }
            /*catch (IrcSocketInterface::Exception& e)
            {
                std::cout << "Exception caught: " << e.Description() << std::endl;
            }*/
        }
    }
}

void IrcData::Recv()
{
    if (recv == true)
    {
        std::string buf;
        std::string sOutput;
        if (S)
        {
            S->Recv(buf);
            sOutput = "<< " + buf;
            Output::Instance().addOutput(sOutput, 5);
            AddRecvQueue(buf);
        }
    }
}

std::string IrcData::GetSendQueue()
{
    boost::mutex::scoped_lock lock(SendMutex);
    std::string temp = "";
    while (HighPrioritySendQueue.empty() && SendQueue.empty() && LowPrioritySendQueue.empty() && send == true)
    {
        SendAvailable.wait(lock);
    }
    if (send == true)
    {
        if (!HighPrioritySendQueue.empty())
        {
            temp = HighPrioritySendQueue.front();
            HighPrioritySendQueue.pop();
            return temp;
        }
        if (!SendQueue.empty())
        {
            temp = SendQueue.front();
            SendQueue.pop();
            return temp;
        }
        if (!LowPrioritySendQueue.empty())
        {
            temp = LowPrioritySendQueue.front();
            LowPrioritySendQueue.pop();
            return temp;
        }
    }
    return temp;
}

std::string IrcData::GetRecvQueue()
{
    boost::mutex::scoped_lock lock(RecvMutex);
    std::string temp = "";
    while (RecvQueue.empty() && recv == true)
    {
        RecvAvailable.wait(lock);
    }
    if (recv == true)
    {
        temp = RecvQueue.front();
        RecvQueue.pop();
    }
    return temp;
}

void IrcData::AddRecvQueue(std::string data)
{
    boost::mutex::scoped_lock lock(RecvMutex);
    RecvQueue.push(data);
    RecvAvailable.notify_one();
}

void IrcData::Parse()
{
    while (parse)
    {
        std::string data;
        std::vector< std::string > result;
        data = GetRecvQueue();
        boost::algorithm::trim(data);
        boost::split(result, data, boost::is_any_of(" "), boost::token_compress_on);
        if (result.size() == 2)
        {
            if (result[0] == "PING")      // PING
            {
                std::string returnstr = "PONG " + result[1] + "\r\n";
                AddHighPrioritySendQueue(returnstr);
            }
        }
        unsigned int consumer_iterator;
        for (consumer_iterator = 0; consumer_iterator < RawConsumers.size(); consumer_iterator++)
        {
            RawConsumers[consumer_iterator]->AddRawQueue(result);
        }
        for (consumer_iterator = 0; consumer_iterator < ModeConsumers.size(); consumer_iterator++)
        {
            // ModeConsumers[consumer_iterator]->AddModeQueue(result);
        }
        for (consumer_iterator = 0; consumer_iterator < WhoisConsumers.size(); consumer_iterator++)
        {
            // WhoisConsumers[consumer_iterator]->AddWhoisQueue(result);
        }
        if (result.size() >= 4)
        {
            if (result[1] == "PRIVMSG")   // PRIVMSG
            {
                for (consumer_iterator = 0; consumer_iterator < PrivmsgConsumers.size(); consumer_iterator++)
                {
                    PrivmsgConsumers[consumer_iterator]->AddPrivmsgQueue(result);
                }
            }
        }
    }
}

void IrcData::flood_timer()
{
    while (send)
    {
        if (buffer < floodbuffer)
        {
            buffer++;            floodcondition.notify_one();
        }
        usleep(floodtime*1000);
    }
}
