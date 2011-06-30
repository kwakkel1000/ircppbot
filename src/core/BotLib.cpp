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

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>

#include "../include/core/Global.h"
#include "../include/core/Output.h"
#include "../include/core/Database.h"


std::string BotLib::StringTimeFromIntSeconds(int miSeconds)
{
    std::vector< int > miTimevector = VectorTimeFromSecondsTime(miSeconds);
    if (miTimevector.size() == 6)
    {
        int iYear = miTimevector[0];
        int iWeek = miTimevector[1];
        int iDay = miTimevector[2];
        int iHour = miTimevector[3];
        int iMinutes = miTimevector[4];
        int iSeconds = miTimevector[5];
        std::string sYear = StringFromInt(iYear);
        std::string sWeek = StringFromInt(iWeek);
        std::string sDay = StringFromInt(iDay);
        std::string sHour = StringFromInt(iHour);
        std::string sMinutes = StringFromInt(iMinutes);
        std::string sSeconds = StringFromInt(iSeconds);
        std::string sTimeString = "";
        if (iYear > 0)
        {
            sTimeString = sTimeString + sYear + " Year";
            if (iYear > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        if (iWeek > 0)
        {
            sTimeString = sTimeString + sWeek + " Week";
            if (iWeek > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        if (iDay > 0)
        {
            sTimeString = sTimeString + sDay + " Day";
            if (iDay > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        if (iHour > 0)
        {
            sTimeString = sTimeString + sHour + " Hour";
            if (iHour > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        if (iMinutes > 0)
        {
            sTimeString = sTimeString + sMinutes + " Minute";
            if (iMinutes > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        if (iSeconds > 0)
        {
            sTimeString = sTimeString + sSeconds + " Second";
            if (iSeconds > 1)
            {
                sTimeString = sTimeString + "s";
            }
            sTimeString = sTimeString + " ";
        }
        return sTimeString;
    }
    return "";
}



std::vector< int > BotLib::VectorTimeFromSecondsTime(int miTime)
{
    // Year, Week, Day, Hour, Minutes, Seconds;
    // year (day * 365)
    // week (day * 7)
    // day (hour * 24)
    // hour (minutes * 60)
    // minutes (seconds * 60)
    // seconds (left)

    std::vector< int > vTime;
    int iYear = (miTime - (miTime % (365 * 24 * 60 * 60))) / (365 * 24 * 60 * 60);
    miTime = miTime - (iYear * 365 * 24 * 60 * 60);
    int iWeek = (miTime - (miTime % (7 * 24 * 60 * 60))) / (7 * 24 * 60 * 60);
    miTime = miTime - (iWeek * 7 * 24 * 60 * 60);
    int iDay = (miTime - (miTime % (24 * 60 * 60))) / (24 * 60 * 60);
    miTime = miTime - (iDay * 24 * 60 * 60);
    int iHour = (miTime - (miTime % (60 * 60))) / (60 * 60);
    miTime = miTime - (iHour * 60 * 60);
    int iMinutes = (miTime - (miTime % 60)) / 60;
    miTime = miTime - (iMinutes * 60);
    int iSeconds = miTime;
    std::cout << "year " << iYear << " week " << iWeek << " day " << iDay << " hour " << iHour << " minutes " << iMinutes << " seconds " << iSeconds << std::endl;
    vTime.push_back(iYear);
    vTime.push_back(iWeek);
    vTime.push_back(iDay);
    vTime.push_back(iHour);
    vTime.push_back(iMinutes);
    vTime.push_back(iSeconds);
    return vTime;
}





std::string BotLib::StringFromInt(int miInput)
{
    std::stringstream ssConversion;
    ssConversion << miInput;
    return ssConversion.str();
}

int BotLib::IntFromString(std::string msInput)
{
    int iOutput;
    std::stringstream ssConversion(msInput);
    ssConversion >> iOutput;
    return iOutput;
}
