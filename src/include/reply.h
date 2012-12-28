//
//
//  @ Project : ircppbot
//  @ File Name : reply.h
//  @ Date : 27-12-2012
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

#ifndef SRC_INCLUDE_REPLY_H
#define SRC_INCLUDE_REPLY_H

#include <string>
#include <map>

class reply
{
    public:
        static reply& instance()
        {
            static reply replyobj;
            return replyobj;
        }
        // init
        void init();

        // reply
        std::string ircReply(std::string reply_name, std::string reply_language);
        std::string ircReplyReplace(std::string source_string, std::string search_string, std::string replace_string);

        std::string ircPrivmsg(std::string target, std::string text);
        std::string ircNotice(std::string target, std::string text);
        std::string ircMode(std::string target, std::string mode);
        std::string ircJoin(std::string channel);
        std::string ircPart(std::string channel, std::string reason);
        std::string ircKick(std::string channel, std::string target, std::string reason);
        std::string ircInvite(std::string channel, std::string target);
        std::string ircWho(std::string channel);
        std::string ircWhoExtra(std::string channel);
        std::string ircWhois(std::string target);
        std::string ircWhois2(std::string target);

        std::string ircBold();
        std::string ircUnderline();
        std::string ircItalic();
        std::string ircNormal();
        std::string ircColor();

    private:
        reply();
        ~reply();
        // vars

        // reply vars
        std::map< std::string, std::map< std::string, std::string > > m_Reply;
};

#endif  // SRC_INCLUDE_REPLY_H
