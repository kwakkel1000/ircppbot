//
//
//  @ Project : ircppbot
//  @ File Name : channels.cpp
//  @ Date : 26-12-2012
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


#include "../include/management/channels.h"
#include <gframe/output.h>

channels::channels()
{
}

channels::~channels()
{
    std::map< std::string, channel >::iterator m_ChannelListIterator;
    for (m_ChannelListIterator = m_ChannelList.end(); m_ChannelListIterator != m_ChannelList.begin(); m_ChannelListIterator--)
    {
        delChannel((*m_ChannelListIterator).first);
    }
}

bool channels::getChannels(std::map< std::string, channel > &channelList)
{
    channelList = m_ChannelList;
    return true;
}

channel& channels::getChannel(std::string channelName)
{
    std::map< std::string, channel >::iterator m_ChannelListIterator;
    m_ChannelListIterator = m_ChannelList.find(channelName);
    if (m_ChannelListIterator != m_ChannelList.end())
    {
        output::instance().addStatus(false, "channel& channels::getChannel(std::string channelName) channel not found");
        exit(EXIT_FAILURE);
    }
    return (*m_ChannelListIterator).second;
}

bool channels::addChannel(std::string channelName)
{
    std::pair< std::map< std::string, channel >::iterator, bool > ret;
    ret = m_ChannelList.insert (std::pair< std::string, channel >(channelName, channel()));
    if (ret.second)
    {
        return true;
    }
    output::instance().addStatus(false, "bool channels::addChannel(std::string channelName) channel already exists");
    return false;
}

bool channels::delChannel(std::string channelName)
{
    size_t ret = m_ChannelList.erase(channelName);
    if (ret == 1)
    {
        output::instance().addStatus(true, "bool channels::delChannel(std::string channelName) channel found, erase succesfull: " + channelName);
        return true;
    }
    output::instance().addStatus(false, "bool channels::delChannel(std::string channelName) channel found, erase failed: " + channelName);
    return false;
}
