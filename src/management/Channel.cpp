//
//
//  @ Project : ircppbot
//  @ File Name : Channel.cpp
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


#include "../include/management/Channel.h"
#include <boost/algorithm/string.hpp>

Channel::Channel()
{
        giveops = 501;
        givevoice = 501;
}

Channel::~Channel()
{
    for ( unsigned int i = 0 ; i < channelauthlist.size(); i++ )
    {
        cout << "DELETED: CHANNEL: " << channelauthlist[i] << "\r\n";
        channelauthlist.erase(channelauthlist.begin()+i);
        ca[i]->~ChannelAuth();
        ca.erase(ca.begin()+i);
    }

}

bool Channel::AddNick(string data)
{
    int i = GetNicksIndex(data);
    if (i == -1)
    {
        nicks.push_back(data);
        op.push_back(false);
        voice.push_back(false);
        return true;
    }
    return false;
}

bool Channel::DelNick(string data)
{
    int i = GetNicksIndex(data);
    if (i >= 0)
    {
        nicks.erase(nicks.begin()+i);
        op.erase(op.begin()+i);
        voice.erase(voice.begin()+i);
        return true;
    }
    return false;
}

bool Channel::AddAuth(string data)
{
    int i = GetChannelAuthIndex(data);
    if (i == -1)
    {
        channelauthlist.push_back (data);
        ChannelAuth *tmpchannelauth = new ChannelAuth();
        tmpchannelauth->SetAccess(-1);
        ca.push_back (tmpchannelauth);
        return true;
    }
    return false;
}

bool Channel::DelAuth(string data)
{
    int i = GetChannelAuthIndex(data);
    if (i >= 0)
    {
        channelauthlist.erase(channelauthlist.begin()+i);
        ca[i]->~ChannelAuth();
        ca.erase(ca.begin()+i);
        return true;
    }
    return false;
}

bool Channel::SetAccess(string data, int access)
{
    int i = GetChannelAuthIndex(data);
    if (i >= 0)
    {
        ca[i]->SetAccess(access);
        return true;
    }
    return false;
}

int Channel::GetAccess(string data)
{
    int i = GetChannelAuthIndex(data);
    if (i >= 0)
    {
        return ca[i]->GetAccess();
    }
    return -1;
}

bool Channel::SetOp(string data, bool set)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    int i = GetNicksIndex(data);
    if (i >= 0)
    {
        op[i] = set;
        return true;
    }
    return false;
}

bool Channel::GetOp(string data)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    int i = GetNicksIndex(data);
    if (i >= 0)
    {
        return op[i];
    }
    return false;
}

bool Channel::SetVoice(string data, bool set)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    int i = GetNicksIndex(data);
    if (i >= 0)
    {
        voice[i] = set;
        return true;
    }
    return false;
}

bool Channel::GetVoice(string data)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    int i = GetNicksIndex(data);
    if (i >= 0)
    {
        return voice[i];
    }
    return false;
}

vector<string> Channel::GetNicks()
{
    return nicks;
}

vector<string> Channel::GetAuths()
{
    return channelauthlist;
}


bool Channel::SetGiveops(int data)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    giveops = data;
    return true;
}

int Channel::GetGiveops()
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    return giveops;
}

bool Channel::SetGivevoice(int data)
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    givevoice = data;
    return true;
}

int Channel::GetGivevoice()
{
    boost::mutex::scoped_lock  lock(Channel_mutex);
    return givevoice;
}


bool Channel::SetCid(std::string data)
{
    cid = data;
    return true;
}

std::string Channel::GetCid()
{
    return cid;
}


int Channel::GetChannelAuthIndex(string data)
{
    for ( unsigned int i = 0 ; i < channelauthlist.size(); i++ )
    {
        if (boost::iequals(channelauthlist[i],data))
        {
            return i;
        }
    }
    return -1;
}

int Channel::GetNicksIndex(string data)
{
    for ( unsigned int i = 0 ; i < nicks.size(); i++ )
    {
        if (boost::iequals(nicks[i],data))
        {
            return i;
        }
    }
    return -1;
}
