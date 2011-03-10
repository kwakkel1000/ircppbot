#include "../include/User.h"

User::User()

{
oaccess = -1;
uid = -1;
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
    nick = data;
}
string User::GetNick()
{
    return nick;
}

void User::SetAuth(string data)
{
    auth = data;
}
string User::GetAuth()
{
    return auth;
}

bool User::AddChannel(string data)
{
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
    //cout << "vector<string> User::GetChannels()" << endl;
    if (channels.size() > 0)
    {
        return channels;
    }
    return NULLvector;
}

void User::SetOaccess(int data)
{
    oaccess = data;
}

int User::GetOaccess()
{
    return oaccess;
}

void User::God()
{
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
    god = data;
}

int User::GetGod()
{
    return god;
}

bool User::SetUid(int data)
{
    uid = data;
    return true;
}

int User::GetUid()
{
    return uid;
}

bool User::SetGone(bool set)
{
    gone = set;
    return true;
}

bool User::GetGone()
{
    return gone;
}

bool User::SetX(bool set)
{
    x = set;
    return true;
}

bool User::GetX()
{
    return x;
}

bool User::SetD(bool set)
{
    d = set;
    return true;
}

bool User::GetD()
{
    return d;
}

bool User::SetIrcop(bool set)
{
    ircop = set;
    return true;
}

bool User::GetIrcop()
{
    return ircop;
}

bool User::SetLanguage(std::string _lang)
{
	lang = _lang;
    return true;
}

std::string User::GetLanguage()
{
    return lang;
}

int User::GetChannelIndex(string data)
{
    for ( unsigned int i = 0 ; i < channels.size(); i++ )
    {
        if (caseInsensitiveStringCompare(channels[i],data))
        {
            return i;
        }
    }
    return -1;
}


bool User::caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 )
{
    std::string str1Cpy( str1 );
    std::string str2Cpy( str2 );
    std::transform( str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower );
    std::transform( str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower );
    return ( str1Cpy == str2Cpy );
}
