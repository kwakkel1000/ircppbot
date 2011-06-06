//
//
//  @ Project : ircppbot
//  @ File Name : Output.cpp
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


#include "../include/core/Output.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>

#include <fstream>

void Output::setDebugLevel(int level)
{
    iOutputLevel = level;
}

void Output::addOutput(std::string output)
{
    addOutput(output, 5);
}

void Output::addOutput(std::string output, int level)
{
    if (level <= iOutputLevel)
    {
        std::cout << "[" << sFormatTime("%d-%m-%Y %H:%M:%S") << "] [" << StringFromInt(level) << "] " << output << std::endl;
    }
    appendLog(output, level);
}

void Output::appendLog(std::string output)
{
    appendLog(output, 5);
}

void Output::appendLog(std::string output, int level)
{
    if (level <= iLogLevel)
    {
        fLogFile << "[" << sFormatTime("%d-%m-%Y %H:%M:%S") << "] [" << StringFromInt(level) << "] " << output << std::endl;
    }
}

std::string Output::StringFromInt(int number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}


std::string Output::sFormatTime(std::string format)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(buffer, 80, format.c_str(), timeinfo);
    return buffer;
}



Output::Output()
{
    iLogLevel = 10;
    iOutputLevel = 5;
    fLogFile.open("bot.log", std::ios::app);
    fLogFile << "\r\n\r\n\r\n\r\nopen logfile\r\n";
}

Output::~Output()
{
    fLogFile << "closing logfile \r\n\r\n\r\n\r\n";
    fLogFile.close();
}
