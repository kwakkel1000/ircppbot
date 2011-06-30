//
//
//  @ Project : ircppbot
//  @ File Name : BotLib.h
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

#ifndef SRC_INCLUDE_CORE_BOTLIB_H_
#define SRC_INCLUDE_CORE_BOTLIB_H_

#include <string>
#include <vector>

#include "../interfaces/ReplyInterface.h"

class BotLib : public ReplyInterface
{
    public:

        // time
        static std::string StringTimeFromIntSeconds(int miSeconds);
        static std::vector< int > VectorTimeFromSecondsTime(int miTime);

        // Conversion
        static std::string StringFromInt(int miInput);
        static int IntFromString(std::string msInput);

    private:
        BotLib();
        ~BotLib();
};

#endif  // SRC_INCLUDE_CORE_BOTLIB_H_
