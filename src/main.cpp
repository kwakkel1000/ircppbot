//
//
//  @ Project : ircppbot
//  @ File Name : main.cpp
//  @ Date : 25-12-2012
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

#include <gframe/mainbase.h>
#include <gframe/versions.h>
#include "include/config.h"

#include "include/bot.h"

std::string mNAME = PACKAGE;
std::string mVERSION = VERSION;
std::string mGITVERSION = __GIT_VERSION;

bool forever = true;

int main(int argc, char *argv[])
{
    std::vector< std::string > args;
    for (int nArg = 0; nArg < argc; nArg++)
    {
        args.push_back(argv[nArg]);
    }
    //while(forever)
    {
        mainbase MainBase(mNAME);
        versions::instance().addVersion(mNAME + " " + mVERSION + " " + mGITVERSION);
        MainBase.parseArgs(args);
        // parse args
        for (uint nArg = 0; nArg < args.size(); nArg++)
        {
        }
        int MBRun = MainBase.run();
        if (MBRun != -1)
        {
            return MBRun;
        }
        bot m_Bot;
        m_Bot.init();
        m_Bot.run();
    }
    return 0;
}
