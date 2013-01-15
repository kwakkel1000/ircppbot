//
//
//  @ Project : ircppbot
//  @ File Name : bot.h
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

#ifndef SRC_INCLUDE_BOT_H
#define SRC_INCLUDE_BOT_H

#include <gframe/socket/tcpsocket.h>

#include <thread>
#include <memory>
#include <string>
#include <vector>

#include "moduleinterface.h"
#include "management/management.h"


class bot
{
    public:
        bot();
        ~bot();

        void init();
        void ircInit();
        void moduleInit();
        void run();

        bool loadModule(std::string moduleName);
        bool unLoadModule(std::string moduleName);

    private:
        tcpsocket *m_IrcSocket;
        std::vector< std::string > m_ModuleList;
        std::vector< void* > m_ModuleVector;
        std::vector< moduleinterface* > m_ModuleInterfaceVector;
        std::vector< createModuleInterface* > m_CreateVector;
        std::vector< destroyModuleInterface* > m_DestroyVector;

        management* m_Management;

        std::atomic<bool> m_Run;
        std::shared_ptr< std::thread > m_TimerThread;
        std::shared_ptr< std::thread > m_ManagementThread;
        std::vector< std::shared_ptr< std::thread > > m_ModuleThreadVector;

        std::string parseCommands(std::vector<std::string> args);

        void botBindsInit();
        void ircRun();
        void moduleRun(size_t moduleIndex);
        void timer();
};

#endif  // SRC_INCLUDE_BOT_H
