//
//
//  @ Project : ircppbot
//  @ File Name : Bot.cpp
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


#include "../include/core/Bot.h"
#include <dlfcn.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "../include/core/ConfigReader.h"
#include "../include/core/IrcData.h"
#include "../include/core/Global.h"
#include "../include/core/Output.h"
#include "../include/core/DatabaseData.h"
#include "../include/core/Reply.h"
#include "../include/core/Data.h"
#include "../include/socket/IrcSocket.h"
#include "../include/socket/SocketException.h"
#include "../include/management/Users.h"
#include "../include/management/Channels.h"
#include "../include/management/Whois.h"


Bot::Bot()
// Initialize all member vars! std::string's will just use their default constructor
: mpIrcSocket(NULL)
{
    Whois::Instance();
}

Bot::~Bot()
{
    // old parse
    std::cout << "Bot::~Bot()" << std::endl;
    timeron = false;
    timer_thread->join();
    Output::Instance().addOutput("timer_thread stopped", 2);
    std::vector< std::string > tmpmodulelist = modulelist;
    for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
    {
        std::string sModuleName = tmpmodulelist[i];
        Output::Instance().addStatus(UnLoadModule(sModuleName), "Unloading " + sModuleName);
    }
    std::cout << "Bot::~Bot()" << std::endl;
    mpIrcSocket->Disconnect();
    std::cout << "mpIrcSocket disconnected" << std::endl;
    delete mpIrcSocket;
    std::cout << "mpIrcSocket deleted" << std::endl;
}

void Bot::Init(std::string configfile)
{
    time_t t= time(0);
    Global::Instance().set_StartTime(t);
    Global::Instance().set_ConfigReader(new ConfigReader());
    Global::Instance().set_ConfigFile(configfile);
    Global::Instance().get_ConfigReader().ClearSettings();
    if (Global::Instance().get_ConfigReader().ReadFile(configfile))
    {
        Output::Instance().addStatus(true, "Opening Config [" + configfile + "]");
    }
    else
    {
        Output::Instance().addStatus(false, "Failed Opening Config [" + configfile + "]");
        exit(1);
    }
    DatabaseData::Instance().init();
    DatabaseData::Instance().DatabaseInit();
    IrcInit();
    if (Global::Instance().get_ConfigReader().GetString("admin_enable") == "true")
    {
        LoadAdmin();
    }
    ModuleInit();
    IrcRun();
}

void Bot::IrcInit()
{
    Global& G = Global::Instance();
    G.set_IrcData(new IrcData());
    std::cout << "IrcInit" << std::endl;
    G.set_BotNick(G.get_ConfigReader().GetString("botnick"));
    std::string nickserv = G.get_ConfigReader().GetString("nickserv");
    std::string ircserver = G.get_ConfigReader().GetString("ircserver");
    std::string ircport = G.get_ConfigReader().GetString("ircport");
    try
    {
        mpIrcSocket = new IrcSocket();
        mpIrcSocket->Connect(ircserver, ircport);
    }
    catch(IrcSocket::Exception& e)
    {
        std::cout << "Exception caught: " << e.Description() << " (" << e.Errornr() << ")" << std::endl;
        exit(1);
    }
    G.get_IrcData().init(mpIrcSocket);
}

void Bot::ModuleInit()
{
    Global& G = Global::Instance();
    ConfigReaderInterface& reader = G.get_ConfigReader();

    G.set_Users(new Users());
    G.set_Channels(new Channels());
    G.set_Reply(new Reply());
    G.get_Reply().Init();
    G.get_IrcData().run();

    std::string sLoadMods;
    sLoadMods = reader.GetString("loadmods");
    moduledir = reader.GetString("moduledir");
    Output::Instance().addStatus(LoadModule(reader.GetString("ircserv")), "Loading " + reader.GetString("ircserv"));
    std::vector< std::string > vLoadMods;
    boost::split(vLoadMods, sLoadMods, boost::is_any_of(" "), boost::token_compress_on);
    for (unsigned int uiModuleIndex = 0; uiModuleIndex < vLoadMods.size(); uiModuleIndex++)
    {
        Output::Instance().addStatus(LoadModule(vLoadMods[uiModuleIndex]), "Loading " + vLoadMods[uiModuleIndex]);
    }
    timeron = true;
    assert(!console_thread);
    console_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::ConsoleRun, this)));
    assert(!timer_thread);
    timer_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::TimerRun, this)));
}

void Bot::Run()
{
    std::cout << "void Bot::Run()" << std::endl;
    // need a mainthread loop;
    while (1)
    {
        usleep(1000000);
    }
    // admin_thread->join();
}

void Bot::LoadAdmin()
{
    // load the admin library
    std::string modulepath = "./" + Global::Instance().get_ConfigReader().GetString("moduledir") + Global::Instance().get_ConfigReader().GetString("admin") + ".so";
    // load the library
    admin = dlopen(modulepath.c_str(), RTLD_LAZY);
    if (!admin) {
        std::cerr << "Cannot load library: " << dlerror() << '\n';
        exit(1);
        // return 1;
    }

    // load the symbols
    create_admin = (create_tai*) dlsym(admin, "create");
    destroy_admin = (destroy_tai*) dlsym(admin, "destroy");
    if (!create_admin || !destroy_admin) {
        std::cerr << "Cannot load symbols: " << dlerror() << '\n';
        exit(1);
        // return 1;
    }
    std::cout << "Admin Loaded" << std::endl;
    // create an instance of the class
    ai = create_admin();
    ai->Init(this, convertString(Global::Instance().get_ConfigReader().GetString("admin_port")));
    assert(!admin_thread);
    admin_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::AdminRun, this)));
}

void Bot::UnLoadAdmin()
{
    // destroy the class
    destroy_admin(ai);

    // unload the channelbot library
    dlclose(admin);
    std::cout << "Admin UnLoaded" << std::endl;
}

bool Bot::LoadModule(std::string sModuleName)
{
    bool bLoaded = false;
    for (unsigned int uiModuleListIndex = 0; uiModuleListIndex < modulelist.size(); uiModuleListIndex++)
    {
        if (modulelist[uiModuleListIndex] == sModuleName)
        {
            bLoaded = true;
        }
    }
    if (!bLoaded)
    {
        ModuleInterface* pModuleInterface;
        void* module;
        create_tmi* create_module;
        destroy_tmi* destroy_module;
        std::string modulepath = "./" + moduledir + sModuleName + ".so";
        // load the library
        module = dlopen(modulepath.c_str(), RTLD_LAZY);
        if (!module)
        {
            cerr << "Cannot load library: " << dlerror() << '\n';
            return false;
            exit(1);
            // return 1;
        }

        // load the symbols
        create_module = (create_tmi*) dlsym(module, "create");
        destroy_module = (destroy_tmi*) dlsym(module, "destroy");
        if (!create_module || !destroy_module)
        {
            cerr << "Cannot load symbols: " << dlerror() << '\n';
            return false;
            exit(1);
            // return 1;
        }
        // create an instance of the class
        pModuleInterface = create_module();
        pModuleInterface->Init(new Data());
        modulelist.push_back(sModuleName);
        modulevector.push_back(module);
        moduleinterfacevector.push_back(pModuleInterface);
        createvector.push_back(create_module);
        destroyvector.push_back(destroy_module);
        boost::shared_ptr<boost::thread> tmp_thread;
        int iModuleIndex = -1;
        for (unsigned int i = 0; i < modulelist.size(); i++)
        {
            if (modulelist[i] == sModuleName)
            {
                iModuleIndex = i;
            }
        }
        if (iModuleIndex >= 0)
        {
            assert(!tmp_thread);
            tmp_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::ModuleRun, this, iModuleIndex)));
            module_thread_vector.push_back(tmp_thread);
        }
        return true;
    }
    return false;
}

bool Bot::UnLoadModule(std::string sModuleName)
{
    int iModuleIndex = -1;
    for (unsigned int i = 0; i < modulelist.size(); i++)
    {
        if (modulelist[i] == sModuleName)
        {
            iModuleIndex = i;
        }
    }
    if (iModuleIndex >= 0)
    {
        moduleinterfacevector[iModuleIndex]->stop();
        // module_thread_vector[iModuleIndex]->join();
        module_thread_vector.erase(module_thread_vector.begin()+iModuleIndex);
        destroyvector[iModuleIndex](moduleinterfacevector[iModuleIndex]);
        moduleinterfacevector.erase(moduleinterfacevector.begin()+iModuleIndex);
        dlclose(modulevector[iModuleIndex]);
        modulelist.erase(modulelist.begin()+iModuleIndex);
        modulevector.erase(modulevector.begin()+iModuleIndex);
        createvector.erase(createvector.begin()+iModuleIndex);
        destroyvector.erase(destroyvector.begin()+iModuleIndex);
        return true;
    }
    return false;
}

bool Bot::UnLoadModuleId(unsigned int moduleid)
{
    int iModuleIndex = -1;
    if (moduleid < modulelist.size())
    {
        iModuleIndex = moduleid;
    }
    if (iModuleIndex >= 0)
    {
        destroyvector[iModuleIndex](moduleinterfacevector[iModuleIndex]);
        dlclose(modulevector[iModuleIndex]);
        modulelist.erase(modulelist.begin()+iModuleIndex);
        modulevector.erase(modulevector.begin()+iModuleIndex);
        moduleinterfacevector.erase(moduleinterfacevector.begin()+iModuleIndex);
        createvector.erase(createvector.begin()+iModuleIndex);
        destroyvector.erase(destroyvector.begin()+iModuleIndex);
        std::cout << modulelist[iModuleIndex] << " UnLoaded" << std::endl;
        return true;
    }
    return false;
}

void Bot::IrcRun()
{
    std::cout << "IrcRun" << std::endl;
    Global& G = Global::Instance();
    std::string ident = G.get_ConfigReader().GetString("ident");
    std::string realname = G.get_ConfigReader().GetString("realname");
    std::string ircpass = G.get_ConfigReader().GetString("ircpass");
    std::string botnick = G.get_ConfigReader().GetString("botnick");
    std::string USER = "USER " + ident + " * * :" + realname + "\r\n";
    std::string PASS = "PASS " + ircpass + "\r\n";
    std::string NICK = "NICK " + botnick + "\r\n";
    try
    {
        mpIrcSocket->Send(USER);
        mpIrcSocket->Send(PASS);
        mpIrcSocket->Send(NICK);
    }
    catch(IrcSocket::Exception& e)
    {
        std::cout << "Exception caught: " << e.Description() << std::endl;
    }
}

void Bot::AdminRun()
{
    std::cout << "AdminRun" << std::endl;
    ai->Run();
}

void Bot::ModuleRun(int i)
{
    moduleinterfacevector[i]->read();
}

void Bot::ConsoleRun()
{
    while (true)
    {
        std::string input_string;
        std::cin >> input_string;
        std::vector< std::string > split_data;
        std::vector< std::string > args;
        std::string command;
        boost::split(split_data, input_string, boost::is_any_of(" "), boost::token_compress_on);
        if (split_data.size() >= 1)
        {
            std::cout << "command ";
            command = split_data[0];
            std::cout << command << std::endl;
        }
        if (split_data.size() >= 2)
        {
            std::cout << "args";
            for (unsigned int i = 1; i < split_data.size(); i++)
            {
                args.push_back(split_data[i]);
                std::cout << " [" << split_data[i] << "]";
            }
            std::cout << std::endl;
        }
        AdminCommands(command, args);
    }
}

void Bot::TimerRun()
{
    while (timeron)
    {
        usleep(1000000);
        for (unsigned int i = 0; i < moduleinterfacevector.size(); i++)
        {
            moduleinterfacevector[i]->timerrun();
        }
    }
}

void Bot::AdminCommands(std::string command, std::vector< std::string > args)
{
    if (args.size() == 0)
    {
        if (boost::iequals(command, "rehash"))
        {
            /*std::string returnstring = "PRIVMSG " + chan + " :reading config file now\r\n";
            Send(returnstring);*/
            Global::Instance().get_ConfigReader().ClearSettings();
            std::string configfile = Global::Instance().get_ConfigFile();
            if (Global::Instance().get_ConfigReader().ReadFile(configfile))
            {
                std::cout << "W00p config is gelezen \\o/" << std::endl;
                /*std::string returnstring = "PRIVMSG " + chan + " :new info read\r\n";
                Send(returnstring);*/
            }
            else
            {
                std::cout << "Kon niet lezen :/" << std::endl;
                /*std::string returnstring = "PRIVMSG " + chan + " :couldnt read file\r\n";
                Send(returnstring);*/
            }
        }
        if (boost::iequals(command, "stop"))
        {
            /*std::string returnstring = "PRIVMSG " + chan + " :stopping now\r\n";
            Send(returnstring);*/
            Global::Instance().set_Run(false);
        }
        if (boost::iequals(command, "restart"))
        {
            /*std::string returnstring = "PRIVMSG " + chan + " :restarting now\r\n";
            Send(returnstring);*/
        }
        if (boost::iequals(command, "listmodules"))
        {
            for (unsigned int i = 0; i < modulelist.size(); i++)
            {
                std::string modname = modulelist[i];
                /*std::cout << "[" << i << "]" << modname << std::endl;*/
                ai->AddSendQueue(modname);
            }
        }
        if (boost::iequals(command, "reloadall"))
        {
            std::vector< std::string > tmpmodulelist = modulelist;
            std::string reply_string;
            for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
            {
                std::string modname = tmpmodulelist[i];
                UnLoadModule(modname);
            }
            for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
            {
                std::string modname = tmpmodulelist[i];
                LoadModule(modname);
            }
        }
    }
    if (args.size() == 1)
    {
        if (boost::iequals(command, "reload"))
        {
            UnLoadModule(args[0]);
            LoadModule(args[0]);
        }
        if (boost::iequals(command, "load"))
        {
            LoadModule(args[0]);
        }
        if (boost::iequals(command, "unload"))
        {
            UnLoadModule(args[0]);
        }
    }
}

int Bot::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);
    ss >> i;
    return i;
}
