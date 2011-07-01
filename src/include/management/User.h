//
//
//  @ Project : ircppbot
//  @ File Name : User.h
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


#ifndef User_h
#define User_h
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/thread/mutex.hpp>

using namespace std;

class User
{
public:

    User();
    virtual ~User();

    virtual void SetNick(std::string);
    virtual std::string GetNick();

    virtual void SetAuth(std::string);
    virtual std::string GetAuth();

    virtual bool AddChannel(std::string);
    virtual bool DelChannel(std::string);

    virtual vector< std::string > GetChannels();

    virtual void SetOaccess(int);
    virtual int GetOaccess();

    virtual void God();
    virtual void SetGod(int);
    virtual int GetGod();

    virtual bool SetGone(bool);
    virtual bool GetGone();

    virtual bool SetX(bool);
    virtual bool GetX();

    virtual bool SetD(bool);
    virtual bool GetD();

    virtual bool SetIrcop(bool);
    virtual bool GetIrcop();

    virtual bool SetLanguage(std::string);
    virtual std::string GetLanguage();



private:
    std::string nick;
    std::string auth;
    std::vector< std::string > channels;
    std::vector< std::string > NULLvector;
    bool gone;
    bool x;
    bool d;
    bool ircop;
    std::string lang;
    int GetChannelIndex(std::string sChannel);
    int oaccess;
    int god;

    boost::mutex User_mutex;
};

#endif // User_h
