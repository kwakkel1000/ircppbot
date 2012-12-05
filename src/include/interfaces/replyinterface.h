//
//
//  @ Project : ircppbot
//  @ File Name : replyinterface.h
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

#ifndef SRC_INCLUDE_INTERFACES_REPLYINTERFACE_H
#define SRC_INCLUDE_INTERFACES_REPLYINTERFACE_H

#include <string>

class replyinterface
{
public:
    virtual void init()=0;

    //reply
    virtual std::string irc_reply(std::string reply_name, std::string reply_language)=0;
    virtual std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)=0;

    virtual std::string irc_privmsg(std::string target, std::string text)=0;
    virtual std::string irc_notice(std::string target, std::string text)=0;
    virtual std::string irc_mode(std::string target, std::string mode)=0;
    virtual std::string irc_join(std::string channel)=0;
    virtual std::string irc_part(std::string channel, std::string reason)=0;
    virtual std::string irc_kick(std::string channel, std::string target, std::string reason)=0;
    virtual std::string irc_invite(std::string channel, std::string target)=0;

    virtual std::string irc_bold()=0;
    virtual std::string irc_underline()=0;
    virtual std::string irc_italic()=0;
    virtual std::string irc_normal()=0;
    virtual std::string irc_color()=0;

};


#endif  // SRC_INCLUDE_INTERFACES_REPLYINTERFACE_H

