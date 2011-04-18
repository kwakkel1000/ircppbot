//
//
//  @ Project : ircppbot
//  @ File Name : Bot.h
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

#ifndef SRC_INCLUDE_CORE_BOT_H_
#define SRC_INCLUDE_CORE_BOT_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include <string>
#include <vector>

#include "../interfaces/AdminInterface.h"
#include "../interfaces/ModuleInterface.h"

// Predefinitions
class IrcSocket;
class AdminInterface;
class Bot
{
    public:
        Bot();
        ~Bot();

        void Init(std::string);
        void IrcInit();
        void ModuleInit();
        void Run();

        void LoadAdmin();
        void UnLoadAdmin();
        bool LoadModule(std::string);
        bool UnLoadModule(std::string);
        bool UnLoadModuleId(unsigned int);

        friend class SslAdmin;

    private:
        IrcSocket *mpIrcSocket;
        AdminInterface *ai;
        void *admin;
        create_tai *create_admin;
        destroy_tai *destroy_admin;
        std::vector< std::string > modulelist;
        std::vector< void* > modulevector;
        std::vector< ModuleInterface* > moduleinterfacevector;
        std::vector< create_tmi* > createvector;
        std::vector< destroy_tmi* > destroyvector;
        std::string moduledir;
        bool timeron;
        boost::shared_ptr< boost::thread > timer_thread;
        boost::shared_ptr< boost::thread > console_thread;
        boost::shared_ptr< boost::thread > admin_thread;
        std::vector< boost::shared_ptr< boost::thread > > module_thread_vector;

        void IrcRun();
        void AdminRun();
        void ModuleRun(int i);
        void ConsoleRun();
        void TimerRun();
        void AdminCommands(std::string command, std::vector< std::string > args);
        int convertString(std::string data);
};

#endif  // SRC_INCLUDE_CORE_BOT_H_
