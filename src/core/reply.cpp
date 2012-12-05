//
//
//  @ Project : ircppbot
//  @ File Name : reply.cpp
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


#include "../include/core/reply.h"

#include <vector>
#include <gframe/configreader.h>
#include <gframe/database.h>
#include <gframe/glib.h>

// init
void reply::init()
{
    mReply.clear();
    std::vector< std::string > keys;
    keys.push_back("reply_name");
    keys.push_back("reply");
    keys.push_back("language");
    std::vector< std::map< std::string, std::string > > result;
    result = databasedata::instance().get("reply", keys, "1=1");
    for (unsigned int _uiResultIndex = 0; _uiResultIndex < result.size(); _uiResultIndex++)
    {
        mReply[result[_uiResultIndex]["language"]][result[_uiResultIndex]["reply_name"]] = result[_uiResultIndex]["reply"];
    }
}

// private

// reply
std::string reply::irc_reply(std::string reply_name, std::string reply_language)
{
    if (mReply.find(reply_language) != mReply.end() && mReply[reply_language].find(reply_name) != mReply[reply_language].end())
    {
        return mReply[reply_language][reply_name];
    }
    else if (mReply.find(configreader::instance().getString("defaultlanguage")) != mReply.end() && mReply[configreader::instance().getString("defaultlanguage")].find(reply_name) != mReply[configreader::instance().getString("defaultlanguage")].end())
    {
        return mReply[configreader::instance().getString("defaultlanguage")][reply_name];
    }
    else
    {
        return "NO SUCH REPLY " + reply_name;
    }
}

// irc_reply_replace
std::string reply::irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)
{
    size_t search_pos;
    search_pos = source_string.find(search_string);
    if (search_pos != std::string::npos)
    {
        source_string.replace(search_pos, search_string.length(), replace_string);
    }
    return source_string;
}


// irc_privmsg
std::string reply::irc_privmsg(std::string msTarget, std::string msText)
{
    return "PRIVMSG " + msTarget + " :" + msText;
}

// irc_notice
std::string reply::irc_notice(std::string msTarget, std::string msText)
{
    return "NOTICE " + msTarget + " :" + msText;
}

// irc_mode
std::string reply::irc_mode(std::string msTarget, std::string msMode)
{
    return "MODE " + msTarget + " " + msMode;
}

// irc_join
std::string reply::irc_join(std::string msChannel)
{
    return "JOIN " + msChannel;
}

// irc_part
std::string reply::irc_part(std::string msChannel, std::string msReason)
{
    return "PART " + msChannel + " :" + msReason;
}

// irc_kick
std::string reply::irc_kick(std::string msChannel, std::string msTarget, std::string msReason)
{
    return "KICK " + msChannel + " " + msTarget + " :" + msReason;
}

// irc_invite
std::string reply::irc_invite(std::string msChannel, std::string msTarget)
{
    return "INVITE " + msTarget + " " + msChannel;
}



// parse_tags
//std::string reply::parse_tags(std::string msSourceString)
//{
//    size_t search_pos;
//    search_pos = source_string.find(search_string);
//    while (search_pos != std::string::npos)
//    {
//        source_string.replace(search_pos, search_string.length(), replace_string);
//        search_pos = source_string.find(search_string);
//    }
//    return source_string;
//} // nog niet af

// irc_bold
std::string reply::irc_bold()
{
    return "" + char(2);
}

// irc_underline
std::string reply::irc_underline()
{
    return "" + char(31);
}

// irc_italic
std::string reply::irc_italic()
{
    return "" + char(22);
}

// irc_normal
std::string reply::irc_normal()
{
    return "" + char(15);
}

// irc_color
std::string reply::irc_color()
{
    return "" + char(3);
}
