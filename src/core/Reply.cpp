//
//
//  @ Project : ircppbot
//  @ File Name : Reply.cpp
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


#include "../include/core/Reply.h"

#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>

#include "../include/core/DatabaseData.h"
#include "../include/core/Global.h"

// public
Reply::Reply()
{
}

Reply::~Reply()
{
}

// init
void Reply::Init()
{
    DBreplyinit();
}

// reload
void Reply::Reload()
{
    DBreplyinit();
}

// private
// init
void Reply::DBreplyinit()
{
    reply_name_vector.clear();
    reply_vector.clear();
    reply_language_vector.clear();
    std::vector< std::vector< std::string > > sql_result;
    std::string sql_string = "select reply_name, reply, language FROM reply;";
    //sql_result = DatabaseData::Instance().RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        // cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << endl;
        reply_name_vector.push_back(sql_result[i][0]);
        reply_vector.push_back(sql_result[i][1]);
        reply_language_vector.push_back(sql_result[i][2]);
    }
}

// reply
std::string Reply::irc_reply(std::string reply_name, std::string reply_language)
{
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (boost::iequals(reply_name, reply_name_vector[i]) && boost::iequals(reply_language, reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (boost::iequals(reply_name, reply_name_vector[i]) && boost::iequals(Global::Instance().get_ConfigReader().GetString("defaultlanguage"), reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    return "NO SUCH REPLY " + reply_name;
}

// irc_reply_replace
std::string Reply::irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)
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
std::string Reply::irc_privmsg(std::string msTarget, std::string msText)
{
    return "PRIVMSG " + msTarget + " :" + msText;
}

// irc_notice
std::string Reply::irc_notice(std::string msTarget, std::string msText)
{
    return "NOTICE " + msTarget + " :" + msText;
}

// irc_mode
std::string Reply::irc_mode(std::string msTarget, std::string msMode)
{
    return "MODE " + msTarget + " " + msMode;
}

// irc_join
std::string Reply::irc_join(std::string msChannel)
{
    return "JOIN " + msChannel;
}

// irc_part
std::string Reply::irc_part(std::string msChannel, std::string msReason)
{
    return "PART " + msChannel + " :" + msReason;
}

// irc_kick
std::string Reply::irc_kick(std::string msChannel, std::string msTarget, std::string msReason)
{
    return "KICK " + msChannel + " " + msTarget + " :" + msReason;
}

// irc_invite
std::string Reply::irc_invite(std::string msChannel, std::string msTarget)
{
    return "INVITE " + msTarget + " " + msChannel;
}



// parse_tags
//std::string Reply::parse_tags(std::string msSourceString)
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
std::string Reply::irc_bold()
{
    return "" + char(2);
}

// irc_underline
std::string Reply::irc_underline()
{
    return "" + char(31);
}

// irc_italic
std::string Reply::irc_italic()
{
    return "" + char(22);
}

// irc_normal
std::string Reply::irc_normal()
{
    return "" + char(15);
}

// irc_color
std::string Reply::irc_color()
{
    return "" + char(3);
}
