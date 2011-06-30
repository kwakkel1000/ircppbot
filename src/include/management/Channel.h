//
//
//  @ Project : ircppbot
//  @ File Name : Channel.h
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

#ifndef Channel_h
#define Channel_h

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ChannelAuth.h"

#include <boost/thread/mutex.hpp>

class Channel
{
public:
    Channel();
    virtual ~Channel();

    virtual bool AddNick(std::string);
    virtual bool DelNick(std::string);
    virtual bool AddAuth(std::string);
    virtual bool DelAuth(std::string);
    virtual bool SetAccess(std::string, int);
    virtual int GetAccess(std::string);
    virtual bool SetOp(std::string, bool);
    virtual bool GetOp(std::string);
    virtual bool SetVoice(std::string, bool);
    virtual bool GetVoice(std::string);

    std::string GetSetting(std::string msKey);
    bool SetSetting(std::string msKey, std::string msValue);
    bool InitSetting(std::string msKey, std::string msValue);

    // depricated
    virtual bool SetGiveops(int);
    virtual int GetGiveops();
    virtual bool SetGivevoice(int);
    virtual int GetGivevoice();
    virtual std::string GetCid();
    virtual bool SetCid(std::string);

    virtual std::vector< std::string > GetNicks();
    virtual std::vector< std::string > GetAuths();

private:
    std::vector< std::string > channelauthlist;
    std::vector< ChannelAuth * > ca;
    std::vector< std::string>  nicks;
    std::vector< bool > op;
    std::vector< bool > voice;
    std::map< std::string, std::string > mSettings;
    int giveops;
    int givevoice;
    std::string cid;
    int GetChannelAuthIndex(std::string data);
    int GetNicksIndex(std::string data);


    boost::mutex Channel_mutex;
    /*boost::mutex User_mutex;
    boost::mutex UsersGetNicks_mutex;
    boost::mutex UsersGetAuths_mutex;
    boost::mutex UsersGetChannels_mutex;
    boost::mutex UsersWhois_mutex;*/
};


#endif // Channel_h
