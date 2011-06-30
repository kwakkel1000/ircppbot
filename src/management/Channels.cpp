//
//
//  @ Project : ircppbot
//  @ File Name : Channels.cpp
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


#include "../include/management/Channels.h"
#include "../include/core/DatabaseData.h"
#include "../include/core/BotLib.h"
#include <boost/algorithm/string.hpp>

Channels::Channels() { }

Channels::~Channels()
{
    for ( unsigned int i = channellist.size() -1; 0 >= i; i-- )
    {
        cout << "DELETED: CHANNEL: " << channellist[i] << "\r\n";
        //cout <<" auth: " << u[i]->GetAuth() << "\r\n";
        channellist.erase(channellist.begin()+i);
        delete c[i];
        c.erase(c.begin()+i);
    }
}

void Channels::RegistrateChannel(std::string mChannelUuid, std::string mChannel)
{
    DatabaseData::Instance().AddChannel(mChannelUuid, mChannel);
}

void Channels::UnregistrateChannel(std::string mChannelUuid)
{
    DatabaseData::Instance().DeleteChannel(mChannelUuid);
    std::vector< std::string > _auths = GetAuths(DatabaseData::Instance().GetChannelByChannelUuid(mChannelUuid));
    for (unsigned int i = 0; i < _auths.size(); i++)
    {
        DelAuth(DatabaseData::Instance().GetChannelByChannelUuid(mChannelUuid), _auths[0]);
    }
    DelChannel(DatabaseData::Instance().GetChannelByChannelUuid(mChannelUuid));
}

void Channels::AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess)
{
    DatabaseData::Instance().AddUserToChannel(mChannelUuid, mUserUuid, mAccess);
    SetAccess(DatabaseData::Instance().GetChannelByChannelUuid(mChannelUuid), DatabaseData::Instance().GetAuthByUserUuid(mUserUuid), mAccess);
}

void Channels::DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid)
{
    DatabaseData::Instance().DeleteUserFromChannel(mChannelUuid, mUserUuid);
    DelAuth(DatabaseData::Instance().GetChannelByChannelUuid(mChannelUuid), DatabaseData::Instance().GetAuthByUserUuid(mUserUuid));
}

bool Channels::AddChannel(string data)
{
    int i = GetChannelIndex(data);
    if (i == -1)
    {
        channellist.push_back(data);
        Channel *tmpchannel = new Channel();
        //tmpchannel->SetChannel(data);
        c.push_back(tmpchannel);
        return true;
    }
    return false;
}

bool Channels::DelChannel(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        channellist.erase(channellist.begin()+i);
        delete c[i];
        c.erase(c.begin()+i);
        return true;
    }
    return false;
}

bool Channels::AddNick(string data, string nick)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->AddNick(nick);
        return true;
    }
    return false;
}

bool Channels::DelNick(string data, string nick)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->DelNick(nick);
        return true;
    }
    return false;
}

bool Channels::AddAuth(string data, string auth)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->AddAuth(auth);
    }
    return false;
}


bool Channels::DelAuth(string data, string auth)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->DelAuth(auth);
        return true;
    }
    return false;
}

bool Channels::SetAccess(string msChannel, string msAuth, int miAccess)
{
    unsigned int i = GetChannelIndex(msChannel);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->SetAccess(msAuth, miAccess);
        return true;
    }
    return false;
}

int Channels::GetAccess(string data, string auth)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetAccess(auth);
    }
    return -1;
}

std::string Channels::GetSetting(std::string msChannel, std::string msKey)
{
    unsigned int uiChannelIndex = GetChannelIndex(msChannel);
    if ((uiChannelIndex >= 0) && (uiChannelIndex < channellist.size()))
    {
        return c[uiChannelIndex]->GetSetting(msKey);
    }
    return "NULL";
}

bool Channels::SetSetting(std::string msChannel, std::string msKey, std::string msValue)
{
    unsigned int uiChannelIndex = GetChannelIndex(msChannel);
    if ((uiChannelIndex >= 0) && (uiChannelIndex < channellist.size()))
    {
        return c[uiChannelIndex]->SetSetting(msKey, msValue);
    }
    return false;
}

bool Channels::InitSetting(std::string msChannel, std::string msKey, std::string msValue)
{
    unsigned int uiChannelIndex = GetChannelIndex(msChannel);
    if ((uiChannelIndex >= 0) && (uiChannelIndex < channellist.size()))
    {
        return c[uiChannelIndex]->InitSetting(msKey, msValue);
    }
    return false;
}

int Channels::GetGiveops(std::string msChannel)
{
    return BotLib::IntFromString(GetSetting(msChannel, "giveops"));
    return 501;
}

bool Channels::SetGiveops(std::string msChannel, int iGiveops)
{
    /*unsigned int i = GetChannelIndex(msChannel);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetGiveops(iGiveops);
    }*/
    return SetSetting(msChannel, "giveops", BotLib::StringFromInt(iGiveops));
    return true;
}

int Channels::GetGivevoice(std::string msChannel)
{
    return BotLib::IntFromString(GetSetting(msChannel, "givevoice"));
    return 501;
}

bool Channels::SetGivevoice(std::string msChannel, int iGivevoice)
{
    /*unsigned int i = GetChannelIndex(msChannel);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetGivevoice(iGivevoice);
    }*/
    return SetSetting(msChannel, "givevoice", BotLib::StringFromInt(iGivevoice));
    return true;
}

std::vector< std::string > Channels::GetChannels()
{
    return channellist;
}

std::vector< std::string > Channels::GetNicks(string data)
{
    std::vector< std::string > tmp;
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        tmp = c[i]->GetNicks();
    }
    return tmp;
}

std::vector< std::string > Channels::GetAuths(std::string sChannel)
{
    std::vector< std::string > tmp;
    unsigned int i = GetChannelIndex(sChannel);
    if ((i >= 0) && (i < channellist.size()))
    {
        tmp = c[i]->GetAuths();
    }
    return tmp;
}

bool Channels::SetCid(string data, std::string cid)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->SetCid(cid);
        return true;
    }
    return false;
}

std::string Channels::GetCid(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetCid();
    }
    return "NULL";
}

bool Channels::GetOp(string data, string nick)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetOp(nick);
    }
    return false;
}

bool Channels::SetOp(string data, string nick, bool set)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetOp(nick, set);
    }
    return false;
}

bool Channels::GetVoice(string data, string nick)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetVoice(nick);
    }
    return false;
}

bool Channels::SetVoice(string data, string nick, bool set)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetVoice(nick, set);
    }
    return false;
}

void Channels::Debug()
{
    for ( unsigned int i = 0; i < channellist.size(); i++ )
    {
        cout << "CHANNEL: " << channellist[i] << endl;
        vector<string> nicks = c[i]->GetNicks();
        cout << "Channel Nicks:";
        for ( unsigned int j = 0 ; j < nicks.size(); j++ )
        {
            cout << "  " << nicks[j];
        }
        cout << endl;
        vector<string> auths = GetAuths(channellist[i]);
        cout << "Channel Auths:";
        for ( unsigned int j = 0 ; j < auths.size(); j++ )
        {
            cout << "  " << auths[j];
        }
        cout << endl;
    }
}

unsigned int Channels::GetChannelIndex(string data)
{
    for ( unsigned int i = 0; i < channellist.size(); i++ )
    {
        if (boost::iequals(channellist[i],data))
        {
            return i;
        }
    }
    return -1;
}
