//
//
//  @ Project : ircppbot
//  @ File Name : moduleinterface.h
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


#ifndef SRC_INCLUDE_INTERFACES_MODULEINTERFACE_H
#define SRC_INCLUDE_INTERFACES_MODULEINTERFACE_H

#include "../ircdata.h"
class moduleinterface
{
public:
    moduleinterface() { }
    virtual void run()=0;
    virtual void stop()=0;
    virtual void init(ircdata* id)=0;
    virtual void timerrun()=0;
};

typedef moduleinterface* createModuleInterface();

typedef void destroyModuleInterface(moduleinterface*);


#endif // SRC_INCLUDE_INTERFACES_MODULEINTERFACE_H
