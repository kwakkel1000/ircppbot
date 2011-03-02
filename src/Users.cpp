#include "../include/Users.h"
#include <boost/algorithm/string.hpp>

Users::Users()
{
    NULLvector.push_back("NULL");
}

Users::~Users()
{
    for ( unsigned int i = nick.size() -1; 0 >= i; i-- )
    {
        cout << "DELETED: NICK: " << nick[i] << " auth: " << u[i]->GetAuth() << "\r\n";
        nick.erase(nick.begin()+i);
        delete u[i];
        u.erase(u.begin()+i);
    }
}

bool Users::AddUser(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i == -1)
    {
        nick.push_back (data);
        User *tmpuser = new User();
        tmpuser->SetNick(data);
        tmpuser->SetAuth("NULL");
        u.push_back (tmpuser);
        return true;
    }
    return false;
}

bool Users::DelUser(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        nick.erase(nick.begin()+i);
        u[i]->~User();
        u.erase(u.begin()+i);
        return true;
    }
    return false;
}

bool Users::ChangeNick(string data, string newnick)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        nick[i] = newnick;
        u[i]->SetNick(newnick);
        DelWhois(data);
        AddWhois(newnick);
        return true;
    }
    return false;
}

void Users::Debug()
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    for ( unsigned int i = 0 ; i < nick.size(); i++ )
    {
        cout << "NICK: " << nick[i] << " auth: " << u[i]->GetAuth() << endl;
        vector<string> channels = u[i]->GetChannels();
        cout << "Nick Channels:";
        for ( unsigned int j = 0 ; j < channels.size(); j++ )
        {
            cout << "  " << channels[j];
        }
        cout << endl;
    }
}

bool Users::AddWhois(string data)
{
    //boost::mutex::scoped_lock  lock(UsersWhois_mutex);
    int i = GetWhoisIndex(data);
    if (i == -1)
    {
        whoislist.push_back (data);
        return true;
    }
    return false;
}

bool Users::DelWhois(string data)
{
    //boost::mutex::scoped_lock  lock(UsersWhois_mutex);
    int i = GetWhoisIndex(data);
    if (i >= 0)
    {
        whoislist.erase(whoislist.begin()+i);
        return true;
    }
    return false;
}

string Users::GetWhois()
{
    //boost::mutex::scoped_lock  lock(UsersWhois_mutex);
    //cout << "Users::GetWhois()\r\n";
    if (whoislist.size() > 0)
    {
        return whoislist[0];
    }
    return "NULL";
}

bool Users::SetAuth(string tmpnick, string auth)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        u[i]->SetAuth(auth);
        return true;
    }
    return false;
}

string Users::GetAuth(string tmpnick)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    size_t authstar;
    authstar = tmpnick.find("*");
    if (authstar != string::npos)
    {
        tmpnick.replace(authstar, 1, "");
        return tmpnick;
    }
    else
    {
        int i = GetNickIndex(tmpnick);
        if (i >= 0)
        {
            return u[i]->GetAuth();
        }
    }
    return "NULL";
}

vector<string> Users::GetNicks(string tmpauth)
{
    //boost::mutex::scoped_lock  lock(UsersGetNicks_mutex);
    vector<string> nicks;

    for ( unsigned int i = 0 ; i < nick.size(); i++ )
    {
        if (boost::iequals(u[i]->GetAuth(),tmpauth))
        {
            nicks.push_back(nick[i]);
        }
    }
    if (nicks.size() > 0)
    {
        return nicks;
    }
    else
    {
        return NULLvector;
    }
}

bool Users::SetOaccess(string tmpnick, int oaccess)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        u[i]->SetOaccess(oaccess);
        return true;
    }
    return false;
}

int Users::GetOaccess(string tmpnick)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        return u[i]->GetOaccess();
    }
    return -1;
}

bool Users::God(string tmpnick)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        u[i]->God();
        return true;
    }
    return false;
}

bool Users::SetGod(string tmpnick, int data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        u[i]->SetGod(data);
        return true;
    }
    return false;
}

int Users::GetGod(string tmpnick)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(tmpnick);
    if (i >= 0)
    {
        return u[i]->GetGod();
    }
    return -1;
}

bool Users::AddChannel(string data, string chan)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        u[i]->AddChannel(chan);
        return true;
    }
    return false;
}

bool Users::DelChannel(string data, string chan)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        u[i]->DelChannel(chan);
        return true;
    }
    return false;
}

bool Users::SetUid(string data, int uid)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        u[i]->SetUid(uid);
        return true;
    }
    return false;
}

int Users::GetUid(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetUid();
    }
    return -1;
}

bool Users::SetGone(string data, bool set)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->SetGone(set);
    }
    return false;
}

bool Users::GetGone(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetGone();
    }
    return false;
}

bool Users::SetX(string data, bool set)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->SetX(set);
    }
    return false;
}

bool Users::GetX(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetX();
    }
    return false;
}

bool Users::SetD(string data, bool set)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->SetD(set);
    }
    return false;
}

bool Users::GetD(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetD();
    }
    return false;
}

bool Users::SetIrcop(string data, bool set)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->SetIrcop(set);
    }
    return false;
}

bool Users::GetIrcop(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetIrcop();
    }
    return false;
}

vector<string> Users::GetAuths()
{
    //boost::mutex::scoped_lock  lock(UsersGetAuths_mutex);
    return authlist;
}

bool Users::AddAuth(string data)
{
    //boost::mutex::scoped_lock  lock(User_mutex);
    for ( unsigned int i = 0 ; i < authlist.size(); i++ )
    {
        if (boost::iequals(authlist[i],data))
        {
            return false;
        }
    }
    authlist.push_back(data);
    cout << "addauth " << data << endl;
    return true;
}

string Users::GetLanguage(string nick)
{
    return "en";
}



int Users::GetWidth(string nick)
{
    return 20;
}



int Users::GetWidthLength(string nick)
{
    return 5;
}

vector<string> Users::GetChannels(string data)
{
    //boost::mutex::scoped_lock  lock(UsersGetChannels_mutex);
    cout << "vector<string> Users::GetChannels(string data)    data: " << data << endl;
    int i = GetNickIndex(data);
    if (i >= 0)
    {
        return u[i]->GetChannels();
    }
    return NULLvector;
}

int Users::GetNickIndex(string data)
{
    for ( unsigned int i = 0 ; i < nick.size(); i++ )
    {
        if (boost::iequals(nick[i],data))
        {
            return i;
        }
    }
    return -1;
}

int Users::GetWhoisIndex(string data)
{
    for ( unsigned int i = 0 ; i < whoislist.size(); i++ )
    {
        if (boost::iequals(whoislist[i],data))
        {
            return i;
        }
    }
    return -1;
}
