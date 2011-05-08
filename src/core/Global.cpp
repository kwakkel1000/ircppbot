//
//
//  @ Project : ircppbot
//  @ File Name : Global.cpp
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


#include "../include/core/Global.h"
#include <iostream>
#include <string>


void Global::delete_all()
{
    std::cout << "void Global::delete_all()" << std::endl;
    // m_IrcData->stop();
    // std::cout << "m_IrcData->stop();" << std::endl;
    delete m_Users;
    std::cout << "m_Users deleted" << std::endl;
    delete m_Channels;
    std::cout << "m_Channels deleted" << std::endl;
    // delete m_IrcData;
    // std::cout << "m_IrcData deleted" << std::endl;
    delete m_Reply;
    std::cout << "m_Reply deleted" << std::endl;
    delete m_ConfigReader;
    std::cout << "m_ConfigReader deleted" << std::endl;
}

void Global::set_Run(bool _Run)
{
    m_Run = _Run;
}
bool Global::get_Run()
{
    return m_Run;
}

void Global::set_StartTime(int _Time)
{
    m_Time = _Time;
}
int Global::get_StartTime()
{
    return m_Time;
}

void Global::set_BotNick(std::string _BotNick)
{
    m_BotNick = _BotNick;
}
std::string Global::get_BotNick()
{
    return m_BotNick;
}

void Global::set_ConfigFile(std::string _ConfigFile)
{
    m_ConfigFile = _ConfigFile;
}
std::string Global::get_ConfigFile()
{
    return m_ConfigFile;
}

void Global::set_Users(UsersInterface* _Users)
{
    m_Users = _Users;
}
UsersInterface& Global::get_Users()
{
    return *m_Users;
}

void Global::set_Channels(ChannelsInterface* _Channels)
{
    m_Channels = _Channels;
}
ChannelsInterface& Global::get_Channels()
{
    return *m_Channels;
}

void Global::set_IrcData(IrcDataInterface* _IrcData)
{
    m_IrcData = _IrcData;
}
IrcDataInterface& Global::get_IrcData()
{
    return *m_IrcData;
}

void Global::set_Reply(ReplyInterface* _Reply)
{
    m_Reply = _Reply;
}
ReplyInterface& Global::get_Reply()
{
    return *m_Reply;
}

void Global::set_ConfigReader(ConfigReaderInterface* _ConfigReader)
{
    m_ConfigReader = _ConfigReader;
}
ConfigReaderInterface& Global::get_ConfigReader()
{
    return *m_ConfigReader;
}
