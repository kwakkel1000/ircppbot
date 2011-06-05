//
//
//  @ Project : ircppbot
//  @ File Name : Output.h
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


#ifndef SRC_INCLUDE_CORE_OUTPUT_H_
#define SRC_INCLUDE_CORE_OUTPUT_H_

#include <string>

#include <fstream>

class Output
{
    public:
        static Output& Instance()
        {
            static Output obj;
            return obj;
        }

        void setDebugLevel(int level);

        virtual void addOutput(std::string output);
        virtual void addOutput(std::string output, int level);
        virtual void appendLog(std::string output);
        virtual void appendLog(std::string output, int level);

        std::string StringFromInt(int number);
        std::string sFormatTime(std::string format);

    private:
        Output();
        ~Output();

        int iLogLevel;
        int iOutputLevel;

        std::ofstream fLogFile;
};

#endif  // SRC_INCLUDE_CORE_OUTPUT_H_




