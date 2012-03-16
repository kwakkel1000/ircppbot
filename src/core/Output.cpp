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
#include "../include/core/BotLib.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>

#include <fstream>

void Output::setDebugLevel(int iLevel)
{
    iOutputLevel = iLevel;
}
void Output::setLogFile(std::string msLogFile)
{
    sLogFile = msLogFile;
}

void Output::addStatus(bool bSuccess, std::string sOutput)
{
    if (bSuccess)
    {
        if (2 <= iOutputLevel)
        {
            std::cout << "\033[1m\033[34m[\033[32m ok \033[34m]\033[39m\033[22m" << sOutput << std::endl;
            appendLog("[ ok ] " + sOutput, 2);
        }
    }
    else
    {
        std::cout << "\033[1m\033[34m[\033[31m !! \033[34m]\033[39m\033[22m" << sOutput << std::endl;
        appendLog("[ !! ] " + sOutput, 1);
    }
}

void Output::addOutput(std::string sOutput)
{
    addOutput(sOutput, 5);
}

void Output::addOutput(std::string sOutput, int iLevel)
{
    if (iLevel <= iOutputLevel)
    {
        std::cout << "\033[1m\033[34m[\033[33m ** \033[34m] [\033[32m" << BotLib::StringFromInt(iLevel) << "\033[34m] \033[0m" << sOutput << std::endl;
    }
    appendLog("[ ** ] " + sOutput, iLevel);
}

void Output::appendLog(std::string sOutput)
{
    appendLog(sOutput, 5);
}

void Output::appendLog(std::string sOutput, int iLevel)
{
    if (iLevel <= iLogLevel)
    {
        fLogFile << "[" << sFormatTime("%d-%m-%Y %H:%M:%S") << "] [" << BotLib::StringFromInt(iLevel) << "] " << sOutput << std::endl;
    }
}

std::string Output::sFormatTime(std::string sFormat)
{
    time_t rawtime;
    struct tm * timeinfo;
    char cBuffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime(cBuffer, 80, sFormat.c_str(), timeinfo);
    return cBuffer;
}

void Output::init()
{
    fLogFile.open(sLogFile.c_str(), std::ios::app);
    fLogFile << "\r\n\r\n\r\n\r\nopen logfile\r\n";
}

Output::Output()
{
    iLogLevel = 10;
    iOutputLevel = 5;
}

Output::~Output()
{
    fLogFile << "closing logfile \r\n\r\n\r\n\r\n";
    fLogFile.close();
}
