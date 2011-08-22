//
//
//  @ Project : ircppbot
//  @ File Name : Users.h
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


#ifndef Users_h
#define Users_h
#include "../interfaces/UsersInterface.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "User.h"
#include <boost/thread/mutex.hpp>

class Users : public UsersInterface
{
public:
    Users();
    virtual ~Users();

    virtual bool AddUser(std::string);
    virtual bool DelUser(std::string);

    virtual bool ChangeNick(std::string, std::string);

    virtual bool AddWhois(std::string);
    virtual bool DelWhois(std::string);
    virtual std::string GetWhois();

    virtual void Debug();

    virtual bool SetAuth(std::string, std::string);
    virtual std::string GetAuth(std::string);
    virtual std::vector< std::string> GetNicks(std::string);
    virtual bool AddAuth(std::string);
    virtual std::vector< std::string> GetAuths();

    virtual bool SetOaccess(std::string, int);
    virtual int GetOaccess(std::string);

    virtual bool God(std::string);
    virtual bool SetGod(std::string, int);
    virtual int GetGod(std::string);

    virtual bool AddChannel(std::string, std::string);
    virtual bool DelChannel(std::string, std::string);
    virtual std::vector< std::string > GetChannels(std::string);

    std::string GetUid(std::string);

    bool SetGone(std::string, bool);
    bool GetGone(std::string);

    bool SetX(std::string, bool);
    bool GetX(std::string);

    bool SetD(std::string, bool);
    bool GetD(std::string);

    bool SetIrcop(std::string, bool);
    bool GetIrcop(std::string);

    bool SetLanguage(std::string, std::string);
    std::string GetLanguage(std::string);

    int GetWidthLength(std::string); // deprecated  replaced by GetCollums
    int GetCollums(std::string);

    int GetWidth(std::string);

private:
    std::vector< std::string > nick;
    std::vector< User* > u;
    std::vector< std::string > whoislist;
    std::vector< std::string > NULLvector;
    std::vector< std::string > authlist;

    boost::mutex User_mutex;
    boost::mutex UsersGetNicks_mutex;
    boost::mutex UsersGetAuths_mutex;
    boost::mutex UsersGetChannels_mutex;
    boost::mutex UsersNickIndexMutex;

    int GetNickIndex(std::string data);
    int GetWhoisIndex(std::string data);
};

#endif // Users_h
