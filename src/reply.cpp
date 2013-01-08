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


#include "include/reply.h"

#include <vector>
#include <gframe/configreader.h>
#include <gframe/database.h>
#include <gframe/glib.h>


reply::reply()
{
    m_Reply.clear();
}

reply::~reply()
{
}

// init
void reply::init()
{
    m_Reply.clear();
    std::vector< std::string > keys;
    keys.push_back("reply_name");
    keys.push_back("reply");
    keys.push_back("language");
    std::vector< std::map< std::string, std::string > > result;
    result = databasedata::instance().get("reply", keys);
    for (unsigned int _uiResultIndex = 0; _uiResultIndex < result.size(); _uiResultIndex++)
    {
        m_Reply[result[_uiResultIndex]["language"]][result[_uiResultIndex]["reply_name"]] = result[_uiResultIndex]["reply"];
    }
}

// private

// reply
std::string reply::ircReply(std::string reply_name, std::string reply_language)
{
    if (m_Reply.find(reply_language) != m_Reply.end() && m_Reply[reply_language].find(reply_name) != m_Reply[reply_language].end())
    {
        return m_Reply[reply_language][reply_name];
    }
    else if (m_Reply.find(configreader::instance().getString("defaultlanguage")) != m_Reply.end() && m_Reply[configreader::instance().getString("defaultlanguage")].find(reply_name) != m_Reply[configreader::instance().getString("defaultlanguage")].end())
    {
        return m_Reply[configreader::instance().getString("defaultlanguage")][reply_name];
    }
    else
    {
        return "NO SUCH REPLY " + reply_name;
    }
}

// irc_reply_replace
std::string reply::ircReplyReplace(std::string sourceString, std::string searchString, std::string replaceString)
{
    size_t l_SearchPos;
    l_SearchPos = sourceString.find(searchString);
    if (l_SearchPos != std::string::npos)
    {
        sourceString.replace(l_SearchPos, searchString.length(), replaceString);
    }
    else
    {
        sourceString = sourceString + " " + searchString + ">" + replaceString;
    }
    return sourceString;
}


// irc_privmsg
std::string reply::ircPrivmsg(std::string target, std::string msText)
{
    return "PRIVMSG " + target + " :" + msText;
}

// irc_notice
std::string reply::ircNotice(std::string target, std::string msText)
{
    return "NOTICE " + target + " :" + msText;
}

// irc_mode
std::string reply::ircMode(std::string target, std::string msMode)
{
    return "MODE " + target + " " + msMode;
}

// irc_join
std::string reply::ircJoin(std::string channel)
{
    return "JOIN " + channel;
}

// irc_part
std::string reply::ircPart(std::string channel, std::string reason)
{
    return "PART " + channel + " :" + reason;
}

// irc_kick
std::string reply::ircKick(std::string channel, std::string target, std::string reason)
{
    return "KICK " + channel + " " + target + " :" + reason;
}

// irc_invite
std::string reply::ircInvite(std::string channel, std::string target)
{
    return "INVITE " + target + " " + channel;
}

// irc_who
std::string reply::ircWho(std::string channel)
{
    return "WHO " + channel;
}
// irc_whoextra
std::string reply::ircWhoExtra(std::string channel)
{
    return "WHO " + channel + " %ncaf";
}

// irc_whois
std::string reply::ircWhois(std::string target)
{
    return "WHOIS " + target;
}
// irc_whois2
std::string reply::ircWhois2(std::string target)
{
    return "WHOIS " + target + " " + target;
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
std::string reply::ircBold()
{
    return "\x02";
//    return "" + char(2);
}

// irc_underline
std::string reply::ircUnderline()
{
    return "\x1f";
//    return "" + char(31);
}

// irc_italic
std::string reply::ircItalic()
{
    return "\x15";
//    return "" + char(22);
}

// irc_normal
std::string reply::ircNormal()
{
    return "\x0f";
//    return "" + char(15);
}

// irc_color
std::string reply::ircColor()
{
    return "\x03";
//    return "" + char(3);
}
