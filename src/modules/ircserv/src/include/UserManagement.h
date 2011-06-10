//
//
//  @ Project : ircppbot
//  @ File Name : UserManagement.h
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

#ifndef UserManagement_h
#define UserManagement_h

#include <interfaces/ModuleInterface.h>
#include <interfaces/DataInterface.h>
#include <string>
#include <vector>
#include <map>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

class DataInterface;
class UserManagement : public ModuleInterface
{
public:
    UserManagement();
    ~UserManagement();
    void read();
    void stop();
    void Init(DataInterface* pData);
    void timerrun(){}

private:
    void ParseData(std::vector< std::string > data);
    void RefreshChannel(std::string sChannel);

    bool NickServ;
    //bool WhoExtra;
    bool Run;
    DataInterface* mpDataInterface;

    void WHO(std::vector< std::string > data);
    void WHOEXTRA(std::vector< std::string > data);
    void JOIN(std::vector< std::string > data);
    void PART(std::vector< std::string > data);
    void KICK(std::vector< std::string > data);
    void NICK(std::vector< std::string > data);
    void MODE(std::vector< std::string > data);
    void QUIT(std::vector< std::string > data);

    std::string HostmaskToNick(std::vector< std::string > data);
    void UserAuth(std::string mNick, std::string mAuth);
    bool Send(std::string data);

    std::string convertInt(int);
    int convertString(std::string);

    void GetChannelInfo(std::string data);
    void GetUserInfo(std::string data);
    void GetAuths();

    void parse_raw();
    boost::shared_ptr<boost::thread> raw_parse_thread;

    std::multimap< std::string, std::string > NoWhoisUsers;
};


#endif // UserManagement_h
