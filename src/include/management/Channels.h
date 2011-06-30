//
//
//  @ Project : ircppbot
//  @ File Name : Channels.h
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

#ifndef Channels_h
#define Channels_h
#include "../interfaces/ChannelsInterface.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Channel.h"

using namespace std;


class Channels : public ChannelsInterface
{

public:

    Channels();
    virtual ~Channels();

    void RegistrateChannel(std::string mChannelUuid, std::string mChannel);
    void UnregistrateChannel(std::string mChannelUuid);

    void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess);
    void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid);

    virtual bool AddChannel(std::string);
    virtual bool DelChannel(std::string);

    virtual bool AddNick(std::string, std::string);
    virtual bool DelNick(std::string, std::string);

    virtual bool AddAuth(std::string, std::string);
    virtual bool DelAuth(std::string, std::string);

    virtual bool SetAccess(std::string, std::string, int);
    virtual int GetAccess(std::string, std::string);

    virtual bool SetGiveops(std::string, int);
    virtual int GetGiveops(std::string);

    virtual bool SetGivevoice(std::string, int);
    virtual int GetGivevoice(std::string);

    std::string GetSetting(std::string msChannel, std::string msKey);
    bool SetSetting(std::string msChannel, std::string msKey, std::string msValue);
    bool InitSetting(std::string msChannel, std::string msKey, std::string msValue);

    // depricated
    virtual std::string GetCid(std::string);
    virtual bool SetCid(std::string, std::string);

    virtual bool GetOp(std::string, std::string);
    virtual bool SetOp(std::string, std::string, bool);

    virtual bool GetVoice(std::string, std::string);
    virtual bool SetVoice(std::string, std::string, bool);

    virtual std::vector< std::string > GetChannels();
    virtual std::vector< std::string > GetNicks(std::string sChannel);
    virtual std::vector< std::string > GetAuths(std::string sChannel);
    virtual void Debug();


private:

    std::vector< std::string > channellist; // making it a std::map  renaming it mChannelList
    std::vector< Channel* > c;
    unsigned int GetChannelIndex(std::string sChannel);
};

#endif // Channels_h
