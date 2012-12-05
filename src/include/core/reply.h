//
//
//  @ Project : ircppbot
//  @ File Name : reply.h
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

#ifndef SRC_INCLUDE_CORE_REPLY_H
#define SRC_INCLUDE_CORE_REPLY_H

#include <string>
#include <map>

#include "../interfaces/replyinterface.h"

class reply : public replyinterface
{
    public:
        static reply& instance()
        {
            static reply obj;
            return obj;
        }
        // init
        void init();

        // reply
        std::string irc_reply(std::string reply_name, std::string reply_language);
        std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string);

        std::string irc_privmsg(std::string target, std::string text);
        std::string irc_notice(std::string target, std::string text);
        std::string irc_mode(std::string target, std::string mode);
        std::string irc_join(std::string channel);
        std::string irc_part(std::string channel, std::string reason);
        std::string irc_kick(std::string channel, std::string target, std::string reason);
        std::string irc_invite(std::string channel, std::string target);

        std::string irc_bold();
        std::string irc_underline();
        std::string irc_italic();
        std::string irc_normal();
        std::string irc_color();

    private:
        reply();
        ~reply();
        // vars

        // reply vars
        std::map< std::string, std::map< std::string, std::string > > mReply;
};

#endif  // SRC_INCLUDE_CORE_REPLY_H
