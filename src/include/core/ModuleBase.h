//
//
//  @ Project : ircppbot
//  @ File Name : ModuleBase.h
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

#ifndef SRC_INCLUDE_CORE_MODULEBASE_H_
#define SRC_INCLUDE_CORE_MODULEBASE_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <vector>
#include <string>

#include "../interfaces/ModuleInterface.h"

class ModuleBase : public ModuleInterface
{
    public:
        ModuleBase();
        ~ModuleBase();

    protected:
        // global functions

        // irc
        void overwatch(std::string bind, std::string command, std::string chan, std::string nick, std::string auth, std::vector< std::string > args);
        void PRIVMSG(std::vector<std::string> data, std::string trigger);
        virtual void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger){}
        bool Send(std::string data);
        bool SendHighPriority(std::string data);
        bool SendLowPriority(std::string data);
        void simulate(std::string nick, std::string auth, std::string chan, std::string simulate_nick, std::string simulate_command, std::vector< std::string > args, int oa);

        // database
        std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
        bool RawSql(std::string data);

        // reply
        std::string irc_reply(std::string reply_name, std::string reply_language);
        std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string);
        void replyReload();

        // other
        std::string HostmaskToNick(std::vector<std::string> data);
        std::string centre(int cmdsize, unsigned int rowamount, unsigned int length);
        std::string fillspace(std::string data, unsigned int length);
        std::vector< std::string > lineout(std::vector<std::string> data, unsigned int rowamount, unsigned int length);
        std::string convertInt(int number);
        int convertString(std::string data);
        std::vector< int > VectorTimeFromSecondsTime(int _time);
};

#endif  // SRC_INCLUDE_CORE_MODULEBASE_H_
