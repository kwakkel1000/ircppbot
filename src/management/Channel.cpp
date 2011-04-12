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
	int i = GetNicksIndex(data);
	if (i >= 0)
	{
		return op[i];
	}
	return false;
}

bool Channel::SetVoice(string data, bool set)
{
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
	giveops = data;
	return true;
}

int Channel::GetGiveops()
{
	return giveops;
}

bool Channel::SetGivevoice(int data)
{
	givevoice = data;
	return true;
}

int Channel::GetGivevoice()
{
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
