#include "../include/management/User.h"
#include <boost/algorithm/string.hpp>

User::User()
{
	boost::mutex::scoped_lock  lock(User_mutex);
	oaccess = -1;
	god = 0;
	NULLvector.push_back("NULL");
	gone = false;
	x = false;
	d = false;
	ircop = false;
}

User::~User() { }

void User::SetNick(string data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    nick = data;
}
string User::GetNick()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return nick;
}

void User::SetAuth(string data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    auth = data;
}
string User::GetAuth()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return auth;
}

bool User::AddChannel(string data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetChannelIndex(data);
    if (i == -1)
    {
        channels.push_back(data);
        return true;
    }
    return false;
}

bool User::DelChannel(string data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetChannelIndex(data);
    if (i >= 0)
    {
        channels.erase(channels.begin()+i);
        return true;
    }
    return false;
}

vector<string> User::GetChannels()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    //cout << "vector<string> User::GetChannels()" << endl;
    if (channels.size() > 0)
    {
        return channels;
    }
    return NULLvector;
}

void User::SetOaccess(int data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    oaccess = data;
}

int User::GetOaccess()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return oaccess;
}

void User::God()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    if(god > 0)
    {
        god = 0;
    }
    else
    {
        god = 1;
    }
}

void User::SetGod(int data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    god = data;
}

int User::GetGod()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return god;
}

bool User::SetGone(bool set)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    gone = set;
    return true;
}

bool User::GetGone()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return gone;
}

bool User::SetX(bool set)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    x = set;
    return true;
}

bool User::GetX()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return x;
}

bool User::SetD(bool set)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    d = set;
    return true;
}

bool User::GetD()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return d;
}

bool User::SetIrcop(bool set)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    ircop = set;
    return true;
}

bool User::GetIrcop()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return ircop;
}

bool User::SetLanguage(std::string _lang)
{
	boost::mutex::scoped_lock  lock(User_mutex);
	lang = _lang;
    return true;
}

std::string User::GetLanguage()
{
	boost::mutex::scoped_lock  lock(User_mutex);
    return lang;
}

int User::GetChannelIndex(string data)
{
	boost::mutex::scoped_lock  lock(User_mutex);
    for ( unsigned int i = 0 ; i < channels.size(); i++ )
    {
        if (boost::iequals(channels[i],data))
        {
            return i;
        }
    }
    return -1;
}
