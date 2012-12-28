//
//
//  @ Project : ircppbot
//  @ File Name : BotLib.cpp
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

#include "../include/core/BotLib.h"

#include <iostream>
#include <cstring>
#include <string>
#include <vector>

std::string BotLib::HostmaskToNick(std::vector<std::string> mvRawIrcData)
{
    std::vector< std::string > _vWho;
    boost::split(_vWho, mvRawIrcData[0], boost::is_any_of("!"), boost::token_compress_on);
    std::string _sNick = _vWho[0];
    boost::erase_all(_sNick, ":");
    return _sNick;
}

std::string BotLib::Centre(std::string msInputString, unsigned int muiRowAmount, unsigned int muiWidth)
{
    std::string _sReturnString = "";
    for (unsigned int l = 0; l < (((muiWidth * muiRowAmount) / 2) - msInputString.size()/2); l++)
    {
        _sReturnString = _sReturnString + " ";
    }
    _sReturnString = _sReturnString + msInputString;
    return _sReturnString;
}

std::string BotLib::FillSpace(std::string data, unsigned int length)
{
    std::string tmpstr = data;
    for (unsigned int l = data.size(); l < length; l++)
    {
        tmpstr = tmpstr + " ";
    }
    return tmpstr;
}

std::vector<std::string> BotLib::LineOut(std::vector<std::string> data, unsigned int rowamount, unsigned int length)
{
    std::vector< std::string > return_vector;
    std::string tmpvector;
    unsigned int k = 0;
    for (unsigned int j = 0; j < data.size(); j++)
    {
        if (k < rowamount)
        {
            tmpvector = tmpvector + FillSpace(data[j], length);
            k++;
        }
        if (k >= rowamount)
        {
            return_vector.push_back(tmpvector);
            tmpvector = "";
            k = 0;
        }
    }
    if (k != 0)
    {
        return_vector.push_back(tmpvector);
    }
    return return_vector;
}
