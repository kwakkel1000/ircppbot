//
//
//  @ Project : ircppbot
//  @ File Name : IrcSocketInterface.h
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


#ifndef IrcSocketInterface_h
#define IrcSocketInterface_h

#include <string>

class IrcSocketInterface
{
 public:
    // Todo: add exception class for every excepion type
    class ExceptionInterface
    {
     public:
        virtual const std::string& Description()=0;
        virtual int Errornr()=0;
    };

 public:

    virtual void Connect(std::string address, std::string service)=0;
    virtual void Disconnect()=0;
    virtual void set_non_blocking ( const bool )=0;

    virtual void Send(const std::string data)=0;
    virtual void Recv(std::string &data)=0;

    virtual bool  IsConnected()=0;
    virtual const std::string& Hostname()=0;
    virtual const std::string& Service()=0;
};

#endif // IrcSocketInterface_h

