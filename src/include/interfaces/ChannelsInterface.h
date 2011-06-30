//
//
//  @ Project : ircppbot
//  @ File Name : ChannelsInterface.h
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

#ifndef ChannelsInterface_h
#define ChannelsInterface_h

#include <string>
#include <vector>

class ChannelsInterface {

public:

    virtual void RegistrateChannel(std::string mChannelUuid, std::string mChannel)=0;
    virtual void UnregistrateChannel(std::string mChannelUuid)=0;

    virtual void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess)=0;
    virtual void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid)=0;

    virtual bool AddChannel(std::string)=0;
    virtual bool DelChannel(std::string)=0;

    virtual bool AddNick(std::string, std::string)=0;
    virtual bool DelNick(std::string, std::string)=0;

    virtual bool AddAuth(std::string, std::string)=0;
    virtual bool DelAuth(std::string, std::string)=0;

    virtual bool SetAccess(std::string, std::string, int)=0;
    virtual int GetAccess(std::string, std::string)=0;

    virtual bool SetGiveops(std::string, int)=0;
    virtual int GetGiveops(std::string)=0;

    virtual bool SetGivevoice(std::string, int)=0;
    virtual int GetGivevoice(std::string)=0;

    virtual std::string GetSetting(std::string msChannel, std::string msKey)=0;
    virtual bool SetSetting(std::string msChannel, std::string msKey, std::string msValue)=0;
    virtual bool InitSetting(std::string msChannel, std::string msKey, std::string msValue)=0;

    // depricated
    virtual std::string GetCid(std::string)=0;
    virtual bool SetCid(std::string, std::string)=0;

    // depricated
    virtual bool GetOp(std::string, std::string)=0;
    virtual bool SetOp(std::string, std::string, bool)=0;

    // depricated
    virtual bool GetVoice(std::string, std::string)=0;
    virtual bool SetVoice(std::string, std::string, bool)=0;

    virtual std::vector< std::string > GetChannels()=0;
    virtual std::vector< std::string > GetNicks(std::string)=0;
    virtual std::vector< std::string > GetAuths(std::string)=0;
    virtual void Debug()=0;
};

#endif // ChannelsInterface_h

