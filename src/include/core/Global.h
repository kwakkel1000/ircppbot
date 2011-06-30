//
//
//  @ Project : ircppbot
//  @ File Name : Global.h
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


#ifndef SRC_INCLUDE_CORE_GLOBAL_H_
#define SRC_INCLUDE_CORE_GLOBAL_H_

#include <string>

#include "../interfaces/UsersInterface.h"
#include "../interfaces/ChannelsInterface.h"
#include "../interfaces/IrcDataInterface.h"
#include "../interfaces/ReplyInterface.h"
#include "../interfaces/ConfigReaderInterface.h"

class UsersInterface;
class ChannelsInterface;
class IrcDataInterface;
class ReplyInterface;
class ConfigReaderInterface;
class Global
{
    public:
        static Global& Instance()
        {
            static Global obj;
            return obj;
        }

        void delete_all();

        virtual void set_Run(bool _Run);
        virtual bool get_Run();

        virtual void set_StartTime(int _Time);
        virtual int get_StartTime();

        virtual void set_BotNick(std::string _BotNick);
        virtual std::string get_BotNick();

        virtual void set_ConfigFile(std::string _ConfigFile);
        virtual std::string get_ConfigFile();

        virtual void set_Users(UsersInterface* _Users);
        virtual UsersInterface& get_Users();

        virtual void set_Channels(ChannelsInterface* _Channels);
        virtual ChannelsInterface& get_Channels();

        virtual void set_IrcData(IrcDataInterface* _IrcData);
        virtual IrcDataInterface& get_IrcData();

        virtual void set_Reply(ReplyInterface* _Reply);
        virtual ReplyInterface& get_Reply();

        virtual void set_ConfigReader(ConfigReaderInterface* _ConfigReader);
        virtual ConfigReaderInterface& get_ConfigReader();

    private:
        Global() {}
        ~Global() {}

        bool m_Run;

        int m_Time;
        std::string m_BotNick;
        std::string m_ConfigFile;
        UsersInterface* m_Users;
        ChannelsInterface* m_Channels;
        IrcDataInterface* m_IrcData;
        ReplyInterface* m_Reply;
        ConfigReaderInterface* m_ConfigReader;
};

#endif  // SRC_INCLUDE_CORE_GLOBAL_H_



