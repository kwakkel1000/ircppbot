//
//
//  @ Project : ircppbot
//  @ File Name : UsersInterface.h
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


#ifndef UsersInterface_h
#define UsersInterface_h

#include <string>
#include <vector>

class UsersInterface
{
public:
    virtual void Debug()=0;                                     // Modules

    virtual bool FirstJoin(std::string msNick)=0;

    virtual std::string GetAuth(std::string)=0;                 // Modules
    virtual std::vector< std::string> GetNicks(std::string)=0;  // Modules  // new function needed: GetAuthNicks
    virtual std::vector< std::string> GetAuths()=0;             // Modules

    virtual bool SetOaccess(std::string, int)=0;                // Modules
    virtual int GetOaccess(std::string)=0;                      // Modules

    virtual bool God(std::string)=0;
    virtual bool SetGod(std::string, int)=0;                    // Modules
    virtual int GetGod(std::string)=0;                          // Modules

    virtual bool DelChannel(std::string, std::string)=0;        // Modules  // new function needed: dont know which yet

    virtual bool SetLanguage(std::string, std::string)=0;       // Modules
    virtual std::string GetLanguage(std::string)=0;             // Modules

    virtual int GetCollums(std::string)=0;                      // Modules
    virtual int GetWidthLength(std::string)=0;                  // Modules  Deprecated  replaced by GetCollums
    virtual int GetWidth(std::string)=0;                        // Modules


    virtual std::string GetUid(std::string)=0;


    friend class UserManagement;

private:
    virtual bool AddUser(std::string)=0;
    virtual bool DelUser(std::string)=0;

    virtual bool ChangeNick(std::string, std::string)=0;

    virtual bool AddChannel(std::string, std::string)=0;
    virtual std::vector< std::string > GetChannels(std::string)=0;

    virtual bool AddAuth(std::string)=0;
    virtual bool SetAuth(std::string, std::string)=0;

    virtual bool SetGone(std::string, bool)=0;

    virtual bool SetX(std::string, bool)=0;

    virtual bool SetD(std::string, bool)=0;

    virtual bool SetIrcop(std::string, bool)=0;

};

#endif // UsersInterface_h

