//
//
//  @ Project : ircppbot
//  @ File Name : DataInterface.h
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


#ifndef SRC_INCLUDE_INTERFACES_DATAINTERFACE_H_
#define SRC_INCLUDE_INTERFACES_DATAINTERFACE_H_

#include <vector>
#include <string>

class DataInterface
{
    public:
        virtual void Init(bool getraw, bool getmode, bool getwhois, bool getprivmsg)=0;
        virtual void stop()=0;
        virtual void AddRawQueue(std::vector< std::string > data)=0;
        virtual void AddModeQueue(std::vector< std::string > data)=0;
        virtual void AddWhoisQueue(std::vector< std::string > data)=0;
        virtual void AddPrivmsgQueue(std::vector< std::string > data)=0;

        virtual std::vector< std::string > GetRawQueue()=0;
        virtual std::vector< std::string > GetModeQueue()=0;
        virtual std::vector< std::string > GetWhoisQueue()=0;
        virtual std::vector< std::string > GetPrivmsgQueue()=0;

        virtual bool GetRaw()=0;
        virtual bool GetMode()=0;
        virtual bool GetWhois()=0;
        virtual bool GetPrivmsg()=0;
};

#endif  // SRC_INCLUDE_INTERFACES_DATAINTERFACE_H_
