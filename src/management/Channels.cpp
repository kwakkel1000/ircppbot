#include "../include/management/Channels.h"
#include <boost/algorithm/string.hpp>
#include "../include/core/DatabaseData.h"

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

bool Channels::SetAccess(string data, string auth, int access)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        c[i]->SetAccess(auth, access);
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

int Channels::GetGiveops(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetGiveops();
    }
    return 501;
}

bool Channels::SetGiveops(string data, int giveops)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetGiveops(giveops);
    }
    return true;
}

int Channels::GetGivevoice(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetGivevoice();
    }
    return 501;
}

bool Channels::SetGivevoice(string data, int givevoice)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->SetGivevoice(givevoice);
    }
    return true;
}

vector<string> Channels::GetChannels()
{
    return channellist;
}

vector<string> Channels::GetNicks(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetNicks();
    }
    vector<string> tmp;
    tmp.push_back("NULL"); // Why not pass an empty vector
    return tmp;
}

vector<string> Channels::GetAuths(string data)
{
    unsigned int i = GetChannelIndex(data);
    if ((i >= 0) && (i < channellist.size()))
    {
        return c[i]->GetAuths();
    }
    vector<string> tmp;
    tmp.push_back("NULL"); // Why not pass an empty vector
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
