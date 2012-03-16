//
//
//  @ Project : ircppbot
//  @ File Name : IrcDataInterface.h
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


#ifndef IrcDataInterface_H
#define IrcDataInterface_H

#include "IrcSocketInterface.h"
#include "DataInterface.h"

#include <string>

class IrcSocketInterface;
class DataInterface;
class IrcDataInterface
{
public:
    virtual void init(IrcSocketInterface *s)=0;
    virtual void run()=0;
    virtual void stop()=0;

    //consumer
    virtual bool AddConsumer(DataInterface *d)=0;
    virtual bool DelConsumer(DataInterface *d)=0;

    //data
    virtual void AddSendQueue(std::string data)=0;
    virtual void AddHighPrioritySendQueue(std::string data)=0;
    virtual void AddLowPrioritySendQueue(std::string data)=0;
};

#endif // IrcDataInterface_H


