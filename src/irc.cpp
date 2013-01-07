//
//
//  @ Project : ircppbot
//  @ File Name : irc.cpp
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

#include "include/irc.h"
#include "include/reply.h"
#include <gframe/output.h>
#include <gframe/configreader.h>
#include <gframe/glib.h>
#include <iostream>
#include <sstream>

irc::irc()
{
}

irc::~irc()
{
    stop();
}

void irc::stop()
{
    m_Send = false;
    m_Recv = false;
    m_Parse = false;
    while (!m_HighPrioritySendQueue.empty())
    {
        m_HighPrioritySendQueue.pop();
    }
    while (!m_LowPrioritySendQueue.empty())
    {
        m_LowPrioritySendQueue.pop();
    }
    while (!m_SendQueue.empty())
    {
        m_SendQueue.pop();
    }
    while (!m_RecvQueue.empty())
    {
        m_RecvQueue.pop();
    }
    m_SendAvailable.notify_all();
    m_RecvAvailable.notify_all();
    m_FloodAvailable.notify_all();
    output::instance().addOutput("void irc::stop() joining threads");
    if (m_FloodProtect)
    {
        m_FloodThread->join();
        output::instance().addOutput("m_FloodThread stopped");
    }
    m_SendThread->join();
    output::instance().addOutput("m_SendThread stopped");
    m_RecvThread->join();
    output::instance().addOutput("m_RecvThread stopped");
    m_ParseThread->join();
    output::instance().addOutput("m_ParseThread stopped");
}

void irc::init(tcpsocket &ircsock)
{
    output::instance().addOutput("void irc::init(tcpsocket *ircsock) begin");
    m_IrcSocket = &ircsock;
    while (!m_HighPrioritySendQueue.empty())
    {
        m_HighPrioritySendQueue.pop();
    }
    while (!m_LowPrioritySendQueue.empty())
    {
        m_LowPrioritySendQueue.pop();
    }
    while (!m_SendQueue.empty())
    {
        m_SendQueue.pop();
    }
    while (!m_RecvQueue.empty())
    {
        m_RecvQueue.pop();
    }
    m_Send = true;
    m_Recv = true;
    m_Parse = true;
    std::string protect_string = configreader::instance().getString("floodprotect");
    std::string buffer_string = configreader::instance().getString("floodbuffer");
    std::string time_string = configreader::instance().getString("floodtime");
    m_FloodProtect = false;
    if (glib::iequals(protect_string, "true"))
    {
        std::stringstream bufferss(buffer_string);
        std::stringstream timess(time_string);
        m_FloodProtect = true;
        bufferss >> m_FloodBuffer;
        timess >> m_FloodTime;
        m_FloodTime = m_FloodTime * 1000000000;
        m_Buffer = 1;
        m_FloodThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&irc::floodTimer, this)));
    }
    output::instance().addOutput("void irc::init(gsocket *ircsock) end");
}

bool irc::addConsumer(ircdata *ircData)
{
    // Consumers.push_back(d);
    if (ircData->getRaw() == true)
    {
        output::instance().addOutput("bool irc::addConsumer(ircdata *ircData) raw consumer added");
        m_RawConsumers.push_back(ircData);
    }
    if (ircData->getEvents() == true)
    {
        output::instance().addOutput("bool irc::addConsumer(ircdata *ircData) events consumer added");
        m_EventsConsumers.push_back(ircData);
    }
    if (ircData->getModes() == true)
    {
        output::instance().addOutput("bool irc::addConsumer(ircdata *ircData) modes consumer added");
        m_ModesConsumers.push_back(ircData);
    }
    if (ircData->getWhois() == true)
    {
        output::instance().addOutput("bool irc::addConsumer(ircdata *ircData) whois consumer added");
        m_WhoisConsumers.push_back(ircData);
    }
    if (ircData->getPrivmsg() == true)
    {
        output::instance().addOutput("bool irc::addConsumer(ircdata *ircData) privmsg consumer added");
        m_PrivmsgConsumers.push_back(ircData);
    }
    return true;
}

bool irc::delConsumer(ircdata *ircData)
{
    size_t consumerIterator;
    output::instance().addOutput("RawConsumers.size() " + glib::stringFromInt(m_RawConsumers.size()), 9);
    for (consumerIterator = m_RawConsumers.size(); consumerIterator > 0; consumerIterator--)
    {
        output::instance().addOutput("consumerIterator " + glib::stringFromInt(consumerIterator-1), 9);
        if (m_RawConsumers[consumerIterator-1] == ircData)
        {
            output::instance().addOutput("consumer removed");
            m_RawConsumers.erase(m_RawConsumers.begin() + consumerIterator-1);
        }
    }
    output::instance().addOutput("EventsConsumers.size() " + glib::stringFromInt(m_EventsConsumers.size()), 9);
    for (consumerIterator = m_EventsConsumers.size(); consumerIterator > 0; consumerIterator--)
    {
        output::instance().addOutput("consumerIterator " + glib::stringFromInt(consumerIterator-1), 9);
        if (m_EventsConsumers[consumerIterator-1] == ircData)
        {
            output::instance().addOutput("consumer removed");
            m_EventsConsumers.erase(m_EventsConsumers.begin() + consumerIterator-1);
        }
    }
    output::instance().addOutput("ModesConsumers.size() " + glib::stringFromInt(m_ModesConsumers.size()), 9);
    for (consumerIterator = m_ModesConsumers.size(); consumerIterator > 0; consumerIterator--)
    {
        output::instance().addOutput("consumerIterator " + glib::stringFromInt(consumerIterator-1), 9);
        if (m_ModesConsumers[consumerIterator-1] == ircData)
        {
            output::instance().addOutput("consumer removed");
            m_ModesConsumers.erase(m_ModesConsumers.begin() + consumerIterator-1);
        }
    }
    output::instance().addOutput("WhoisConsumers.size() " + glib::stringFromInt(m_WhoisConsumers.size()), 9);
    for (consumerIterator = m_WhoisConsumers.size(); consumerIterator > 0; consumerIterator--)
    {
        output::instance().addOutput("consumerIterator " + glib::stringFromInt(consumerIterator-1), 9);
        if (m_WhoisConsumers[consumerIterator-1] == ircData)
        {
            output::instance().addOutput("consumer removed");
            m_WhoisConsumers.erase(m_WhoisConsumers.begin() + consumerIterator-1);
        }
    }
    output::instance().addOutput("PrivmsgConsumers.size() " + glib::stringFromInt(m_PrivmsgConsumers.size()), 9);
    for (consumerIterator = m_PrivmsgConsumers.size(); consumerIterator > 0; consumerIterator--)
    {
        output::instance().addOutput("consumerIterator " + glib::stringFromInt(consumerIterator-1), 9);
        if (m_PrivmsgConsumers[consumerIterator-1] == ircData)
        {
            output::instance().addOutput("consumer removed", 9);
            m_PrivmsgConsumers.erase(m_PrivmsgConsumers.begin() + consumerIterator-1);
        }
    }
    return true;
}

void irc::run()
{
    m_SendThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&irc::sendLoop, this)));
    m_RecvThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&irc::recvLoop, this)));
    m_ParseThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&irc::parse, this)));
}

void irc::addHighPrioritySendQueue(std::string data)
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    m_HighPrioritySendQueue.push(data);
    m_SendAvailable.notify_one();
}

void irc::addSendQueue(std::string data)
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    m_SendQueue.push(data);
    m_SendAvailable.notify_one();
}

void irc::addLowPrioritySendQueue(std::string data)
{
    std::lock_guard<std::mutex> lock(m_SendMutex);
    m_LowPrioritySendQueue.push(data);
    m_SendAvailable.notify_one();
}

// private
void irc::sendLoop()
{
    while (m_Send == true)
    {
        send();
    }
    output::instance().addOutput("void irc::sendLoop(), after while", 2);
}

void irc::recvLoop()
{
    while (m_Recv == true)
    {
        recv();
    }
    output::instance().addOutput("void irc::recvLoop(), after while", 2);
}

void irc::send()
{
    if (m_Send == true)
    {
        std::string data;
        std::string sOutput;
        if (m_FloodProtect)
        {
            std::unique_lock<std::mutex> lock(m_FloodMutex);
            while (m_Buffer <= 0)
            {
                m_FloodAvailable.wait(lock);
            }
            data = getSendQueue();
            sOutput = ">> " + data;
            data = data + "\r\n";
            output::instance().addOutput(sOutput, 11);
            // try
            {
                if (m_Send)
                {
                    if (glib::iequals(configreader::instance().getString("chandebug"), "true"))
                    {
                        *m_IrcSocket << reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "[" + output::instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] " + data);
                    }
                    m_Buffer--;
                    *m_IrcSocket << data;
                }
            }
            /*catch (IrcSocketInterface::Exception& e)
            {
                output::instance().addOutput("Exception caught: " << e.Description());
            }*/
            lock.unlock();
        }
        else
        {
            data = getSendQueue();
            sOutput = ">> " + data;
            data = data + "\r\n";
            output::instance().addOutput(sOutput, 11);
            // try
            {
                if (m_Send)
                {
                    if (glib::iequals(configreader::instance().getString("chandebug"), "true"))
                    {
                        *m_IrcSocket << reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "[" + output::instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] " + data);
                    }
                    *m_IrcSocket << data;
                }
            }
            /*catch (IrcSocketInterface::Exception& e)
            {
                output::instance().addOutput("Exception caught: " << e.Description());
            }*/
        }
    }
}

void irc::recv()
{
    if (m_Recv == true)
    {
        std::string buf;
        std::string sOutput;
        if (m_IrcSocket)
        {
            *m_IrcSocket >> buf;
            sOutput = "<< " + buf;
            output::instance().addOutput(sOutput, 11);
            addRecvQueue(buf);
            /*if (glib::iequals(configreader::instance().getString("chandebug"), "true"))
            {
                *m_IrcSocket << reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "[" + output::instance().sFormatTime("%d-%m-%Y %H:%M:%S") + "] " + buf + "\r\n");
            }*/
        }
    }
}

std::string irc::getSendQueue()
{
    std::unique_lock<std::mutex> lock(m_SendMutex);
    std::string temp = "";
    while (m_HighPrioritySendQueue.empty() && m_SendQueue.empty() && m_LowPrioritySendQueue.empty() && m_Send == true)
    {
        m_SendAvailable.wait(lock);
    }
    if (m_Send == true)
    {
        if (!m_HighPrioritySendQueue.empty())
        {
            temp = m_HighPrioritySendQueue.front();
            m_HighPrioritySendQueue.pop();
            return temp;
        }
        if (!m_SendQueue.empty())
        {
            temp = m_SendQueue.front();
            m_SendQueue.pop();
            return temp;
        }
        if (!m_LowPrioritySendQueue.empty())
        {
            temp = m_LowPrioritySendQueue.front();
            m_LowPrioritySendQueue.pop();
            return temp;
        }
    }
    lock.unlock();
    return temp;
}

std::string irc::getRecvQueue()
{
    std::unique_lock<std::mutex> lock(m_RecvMutex);
    std::string temp = "";
    while (m_RecvQueue.empty() && m_Recv == true)
    {
        m_RecvAvailable.wait(lock);
    }
    if (m_Recv == true)
    {
        temp = m_RecvQueue.front();
        m_RecvQueue.pop();
    }
    lock.unlock();
    return temp;
}

void irc::addRecvQueue(std::string data)
{
    std::unique_lock<std::mutex> lock(m_RecvMutex);
    m_RecvQueue.push(data);
    lock.unlock();
    m_RecvAvailable.notify_one();
}

void irc::parse()
{
    while (m_Parse)
    {
        std::string data;
        std::vector< std::string > result;
        data = getRecvQueue();
        glib::trim(data);
        result = glib::split(data);
        //boost::split(result, data, boost::is_any_of(" "), boost::token_compress_on);
        if (result.size() == 2)
        {
            if (result[0] == "PING")      // PING
            {
                std::string returnstr = "PONG " + result[1];
                addHighPrioritySendQueue(returnstr);
            }
        }
        size_t consumerIterator;
        for (consumerIterator = 0; consumerIterator < m_RawConsumers.size(); consumerIterator++)
        {
            m_RawConsumers[consumerIterator]->addRawQueue(result);
        }
        for (consumerIterator = 0; consumerIterator < m_EventsConsumers.size(); consumerIterator++)
        {
            if (result.size() == 3)
            {
                if (result[1] == "NICK" || result[1] == "JOIN")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"NICK\" || result[1] == \"JOIN\")", 11);
                    m_EventsConsumers[consumerIterator]->addEventsQueue(result);
                }
            }
            if (result.size() >= 3)
            {
                if (result[1] == "QUIT" || result[1] == "PART")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"QUIT\" || result[1] == \"PART\")", 11);
                    m_EventsConsumers[consumerIterator]->addEventsQueue(result);
                }
            }
            if (result.size() >= 4)
            {
                if (result[1] == "KICK")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"KICK\")", 11);
                    m_EventsConsumers[consumerIterator]->addEventsQueue(result);
                }
            }
        }
        for (consumerIterator = 0; consumerIterator < m_ModesConsumers.size(); consumerIterator++)
        {
            if (result.size() == 4)
            {
                if (result[1] == "MODE")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"MODE\")", 11);
                    m_ModesConsumers[consumerIterator]->addModesQueue(result);
                }
            }
            // own queue would be overkill
            if (result.size() >= 4)
            {
                if (result[1] == "TOPIC")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"TOPIC\")", 11);
                    m_ModesConsumers[consumerIterator]->addModesQueue(result);
                }
            }
            if (result.size() >= 5)
            {
                if (result[1] == "332")
                {
                    output::instance().addOutput("void irc::parse() if (result[1] == \"332\")", 11);
                    m_ModesConsumers[consumerIterator]->addModesQueue(result);
                }
            }
        }
        for (consumerIterator = 0; consumerIterator < m_WhoisConsumers.size(); consumerIterator++)
        {
            if (result.size() >= 4)
            {
                if (result[1] == "307" || result[1] == "318" || result[1] == "330" || result[1] == "402")       //WHOIS regged userName
                {
                    for (consumerIterator = 0; consumerIterator < m_WhoisConsumers.size(); consumerIterator++)
                    {
                        m_WhoisConsumers[consumerIterator]->addWhoisQueue(result);
                    }
                }
            }
            if (result.size() == 7)
            {
                if (result[1] == "354")       //WHOEXTRA
                {
                    for (consumerIterator = 0; consumerIterator < m_WhoisConsumers.size(); consumerIterator++)
                    {
                        m_WhoisConsumers[consumerIterator]->addWhoisQueue(result);
                    }
                }
            }
            if (result.size() >= 11)
            {
                if (result[1] == "352")       //WHO
                {
                    for (consumerIterator = 0; consumerIterator < m_WhoisConsumers.size(); consumerIterator++)
                    {
                        m_WhoisConsumers[consumerIterator]->addWhoisQueue(result);
                    }
                }
            }
        }
        if (result.size() >= 4)
        {
            if (result[1] == "PRIVMSG")   // PRIVMSG
            {
                for (consumerIterator = 0; consumerIterator < m_PrivmsgConsumers.size(); consumerIterator++)
                {
                    m_PrivmsgConsumers[consumerIterator]->addPrivmsgQueue(result);
                }
            }
        }
    }
}

void irc::floodTimer()
{
    while (m_Send)
    {
        if (m_Buffer < m_FloodBuffer)
        {
            m_Buffer++;
            m_FloodAvailable.notify_one();
        }
        struct timespec req, rem;
        req.tv_sec = 0;
        req.tv_nsec = m_FloodTime;
        rem.tv_sec = 0;
        rem.tv_nsec = 0;
        nanosleep(&req, &rem);
    }
}
