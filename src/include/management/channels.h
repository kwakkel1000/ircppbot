//
//
//  @ Project : ircppbot
//  @ File Name : channels.h
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


#ifndef SRC_INCLUDE_MANAGEMENT_CHANNELS_H
#define SRC_INCLUDE_MANAGEMENT_CHANNELS_H

#include <string>
#include <map>
#include "channel.h"

class channel;
class channels
{
    public:
        static channels& instance()
        {
            static channels obj;
            return obj;
        }
        bool addChannel(std::string channelName);
        bool delChannel(std::string channelName);

        bool getChannels(std::map< std::string, channel > &channelList);
        channel& getChannel(std::string channelName);


    private:
        channels();
        ~channels();

        std::map< std::string, channel > m_ChannelList;
};

#endif // SRC_INCLUDE_MANAGEMENT_CHANNELS_H
