//
//
//  @ Project : ircppbot
//  @ File Name : GlobalInterface.h
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


#ifndef GlobalInterface_H
#define GlobalInterface_H

#include <string>
#include "UsersInterface.h"
#include "ChannelsInterface.h"
#include "IrcDataInterface.h"
#include "ReplyInterface.h"
#include "ConfigReaderInterface.h"
class GlobalInterface {

    public:

        virtual GlobalInterface& Instance()=0;

        virtual void delete_all();

        virtual void set_Run(bool _Run)=0;
        virtual bool get_Run()=0;

        virtual void set_StartTime(int _Time)=0;
        virtual int get_StartTime()=0;

        virtual void set_BotNick(std::string _BotNick)=0;
        virtual std::string get_BotNick()=0;

        virtual void set_ConfigFile(std::string _ConfigFile)=0;
        virtual std::string get_ConfigFile()=0;

        virtual void set_Users(UsersInterface* _Users)=0;
        virtual UsersInterface& get_Users()=0;

        virtual void set_Channels(ChannelsInterface* _Channels)=0;
        virtual ChannelsInterface& get_Channels()=0;

        virtual void set_IrcData(IrcDataInterface* _IrcData)=0;
        virtual IrcDataInterface& get_IrcData()=0;

        virtual void set_Reply(ReplyInterface* _Reply)=0;
        virtual ReplyInterface& get_Reply()=0;

        virtual void set_ConfigReader(ConfigReaderInterface* _ConfigReader)=0;
        virtual ConfigReaderInterface& get_ConfigReader()=0;
};
#endif // GlobalInterface_H




