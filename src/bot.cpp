//
//
//  @ Project : ircppbot
//  @ File Name : bot.cpp
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


#include "include/bot.h"

#include <dlfcn.h>
#include <gframe/glib.h>
#include <gframe/versions.h>
#include <gframe/output.h>
#include <gframe/configreader.h>
#include <gframe/database.h>
#include <gframe/database/mysqldatabase.h>

#include "include/reply.h"
#include "include/binds.h"
#include "include/irc.h"
#include "include/management/managementscontainer.h"
/*#include "include/management/auth.h"
#include "include/management/user.h"
#include "include/management/channel.h"*/
//#include "include/management/Whois.h"

#include <iostream>

bot::bot()
// Initialize all member vars! std::string's will just use their default constructor
: m_IrcSocket(NULL),
 m_Management(NULL)
{
    //Whois::Instance();
}

bot::~bot()
{
    output::instance().addOutput("bot::~bot()");
    m_Run = false;
    m_TimerThread->join();
    output::instance().addOutput("m_TimerThread stopped", 2);

    // stop modules
    std::vector< std::string > tmpm_ModuleList = m_ModuleList;
    for (size_t i = 0; i < tmpm_ModuleList.size(); i++)
    {
        std::string sModuleName = tmpm_ModuleList[i];
        output::instance().addStatus(true, unLoadModule(sModuleName) + "Unloading " + sModuleName);
    }

    // stop management
    m_Management->stop();
    m_ManagementThread->join();
    output::instance().addOutput("m_ManagementThread stopped", 2);
    delete m_Management;
    output::instance().addOutput("m_Management deleted");

    irc::instance().stop();

    // stop socket
    m_IrcSocket->disconnect();
    output::instance().addOutput("m_IrcSocket disconnected");
    delete m_IrcSocket;
    output::instance().addOutput("m_IrcSocket deleted");
}

void bot::init()
{
    output::instance().addOutput("void bot::init()", 10);
    //time_t t= time(0);
    databasedata::instance().init(
                                  new mysqldatabase(),
                                  configreader::instance().getString("mysqlserver"),
                                  configreader::instance().getString("database"),
                                  configreader::instance().getString("username"),
                                  configreader::instance().getString("password"));

    reply::instance().init();
    binds::instance().init();
    ircInit();
    moduleInit();
    ircRun();
}

void bot::ircInit()
{
    output::instance().addOutput("void bot::ircInit()", 10);
    //G.set_BotNick(configreader::instance().getString("botnick"));
    m_IrcSocket = new tcpsocket();
    m_IrcSocket->create();
    m_IrcSocket->connect(configreader::instance().getString("ircserver"), glib::intFromString(configreader::instance().getString("ircport")));
    output::instance().addOutput("before irc::instance().init(*m_IrcSocket);");
    irc::instance().init(*m_IrcSocket);
    m_Management = new management();
    m_Management->init();
    m_ManagementThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&management::read, m_Management)));
}

void bot::moduleInit()
{
    output::instance().addOutput("void bot::moduleInit()", 10);
    irc::instance().run();
    std::vector< std::string > vLoadMods = glib::split(configreader::instance().getString("loadmods"));
    for (size_t uiModuleIndex = 0; uiModuleIndex < vLoadMods.size(); uiModuleIndex++)
    {
        output::instance().addStatus(loadModule(vLoadMods[uiModuleIndex]), "Loading " + vLoadMods[uiModuleIndex]);
    }
    m_Run = true;
    m_TimerThread = std::shared_ptr<std::thread>(new std::thread(std::bind(&bot::timer, this)));
}

void bot::run()
{
    output::instance().addOutput("void bot::run()", 10);
    usleep(1000000);
    std::vector< std::string > versionsVector;
    versionsVector = versions::instance().getVersions();
    for (size_t versionsVectorIterator = 0; versionsVectorIterator < versionsVector.size(); versionsVectorIterator++)
    {
        *m_IrcSocket << reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), versionsVector[versionsVectorIterator]) + "\r\n";
    }
    while (m_Run)
    {
        std::string input_string = "";
        std::cin.clear();
        // getline crashes on ctrl + c,
        std::getline(std::cin, input_string);
        output::instance().addOutput(" [" + input_string + "]");
        irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "[" + input_string + "]"));
        std::vector< std::string > args;
        args = glib::split(input_string);
        parseCommands(args);
        //irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), parseCommands(args)));
    }
}

bool bot::loadModule(std::string moduleName)
{
    output::instance().addOutput("bool bot::loadModule(std::string moduleName): " + moduleName, 10);
    bool bLoaded = false;
    for (size_t uiModuleListIndex = 0; uiModuleListIndex < m_ModuleList.size(); uiModuleListIndex++)
    {
        if (m_ModuleList[uiModuleListIndex] == moduleName)
        {
            bLoaded = true;
        }
    }
    if (!bLoaded)
    {
        moduleinterface* pModuleInterface;
        void* module;
        createModuleInterface* create_module;
        destroyModuleInterface* destroy_module;
        std::string modulepath = configreader::instance().getString("moduledir") + moduleName + ".so";
        //std::string modulepath = moduleName;
        // load the library
        module = dlopen(modulepath.c_str(), RTLD_LAZY);
        if (!module)
        {
            std::string errorstring = dlerror();
            output::instance().addStatus(false, "Cannot load library: " + errorstring);
            return false;
        }

        // load the symbols
        create_module = (createModuleInterface*) dlsym(module, "create");
        destroy_module = (destroyModuleInterface*) dlsym(module, "destroy");
        if (!create_module || !destroy_module)
        {
            std::string errorstring = dlerror();
            output::instance().addStatus(false, "Cannot load symbols: " + errorstring);
            return false;
        }
        // create an instance of the class
        pModuleInterface = create_module();
        pModuleInterface->init();
        m_ModuleList.push_back(moduleName);
        m_ModuleVector.push_back(module);
        m_ModuleInterfaceVector.push_back(pModuleInterface);
        m_CreateVector.push_back(create_module);
        m_DestroyVector.push_back(destroy_module);
        std::shared_ptr<std::thread> tmp_thread;
        for (size_t m_ModuleListIndex = 0; m_ModuleListIndex < m_ModuleList.size(); m_ModuleListIndex++)
        {
            if (m_ModuleList[m_ModuleListIndex] == moduleName)
            {
                size_t moduleIndex = m_ModuleListIndex;
                //if (moduleIndex >= 0)
                {
                    //assert(!tmp_thread);
                    tmp_thread = std::shared_ptr<std::thread>(new std::thread(std::bind(&bot::moduleRun, this, moduleIndex)));
                    m_ModuleThreadVector.push_back(tmp_thread);
                }
                output::instance().addStatus(true, "Module " + moduleName + " loaded succesfully");
                return true;
            }
        }
    }
    output::instance().addStatus(false, "Module " + moduleName + " already loaded");
    return false;
}

bool bot::unLoadModule(std::string moduleName)
{
    output::instance().addOutput("bool bot::unLoadModule(std::string moduleName): " + moduleName, 10);
    for (size_t m_ModuleListIndex = 0; m_ModuleListIndex < m_ModuleList.size(); m_ModuleListIndex++)
    {
        if (m_ModuleList[m_ModuleListIndex] == moduleName)
        {
            size_t moduleIndex = m_ModuleListIndex;
            //if (moduleIndex >= 0)
            {
                output::instance().addOutput("m_ModuleListIndex=" + glib::stringFromInt(m_ModuleListIndex) + " m_ModuleThreadVector.size()=" + glib::stringFromInt(m_ModuleThreadVector.size()), 11);
                std::shared_ptr<std::thread> tmp_thread = m_ModuleThreadVector[moduleIndex];
                m_ModuleThreadVector.erase(m_ModuleThreadVector.begin()+moduleIndex);
                m_DestroyVector[moduleIndex](m_ModuleInterfaceVector[moduleIndex]);
                m_ModuleInterfaceVector.erase(m_ModuleInterfaceVector.begin()+moduleIndex);
                dlclose(m_ModuleVector[moduleIndex]);
                tmp_thread->join();
                //tmp_thread.reset();
                m_ModuleList.erase(m_ModuleList.begin()+moduleIndex);
                m_ModuleVector.erase(m_ModuleVector.begin()+moduleIndex);
                m_CreateVector.erase(m_CreateVector.begin()+moduleIndex);
                m_DestroyVector.erase(m_DestroyVector.begin()+moduleIndex);
                output::instance().addStatus(true, "bool bot::unLoadModule(std::string moduleName) done : " + moduleName);
                return true;
            }
        }
    }
    output::instance().addStatus(false, "bool bot::unLoadModule(std::string moduleName) done : " + moduleName);
    return false;
}

void bot::ircRun()
{
    output::instance().addOutput("void bot::ircRun()", 10);
    std::string ident = configreader::instance().getString("ident");
    std::string realname = configreader::instance().getString("realname");
    std::string ircpass = configreader::instance().getString("ircpass");
    std::string botnick = configreader::instance().getString("botnick");
    std::string USER = "USER " + ident + " * * :" + realname + "\r\n";
    std::string PASS = "PASS " + ircpass + "\r\n";
    std::string NICK = "NICK " + botnick + "\r\n";
    *m_IrcSocket << USER;
    *m_IrcSocket << PASS;
    *m_IrcSocket << NICK;
    *m_IrcSocket << "JOIN " + configreader::instance().getString("debugchannel") + "\r\n";
    //m_Management->read();
}

void bot::moduleRun(size_t moduleIndex)
{
    output::instance().addOutput("void bot::moduleRun(size_t moduleIndex)", 10);
    m_ModuleInterfaceVector[moduleIndex]->read();
}

void bot::timer()
{
    output::instance().addOutput("void bot::timer()", 10);
    while (m_Run)
    {
        usleep(1000000);
        for (size_t i = 0; i < m_ModuleInterfaceVector.size(); i++)
        {
            m_ModuleInterfaceVector[i]->timerrun();
        }
    }
}

std::string bot::parseCommands(std::vector<std::string> args)
{
    std::string returnString = "";
    std::string command = "";
    if (args.size() >= 1)
    {
        binds::bindelement bindElement;
        if (binds::instance().getBind(bindElement, args[0], "core"))
        {
            command = bindElement.command;
        }
    }
    if (args.size() == 1)
    {

        if (glib::iequals(command, "version"))
        {
            std::vector< std::string > versionsVector;
            versionsVector = versions::instance().getVersions();
            /*std::map< std::string, channel > channelList;
            channels::instance().getChannels(channelList);
            std::map< std::string, channel >::iterator channelListIterator;
            for (channelListIterator = channelList.begin(); channelListIterator != channelList.end(); channelListIterator++)
            {
                for (size_t versionsVectorIterator = 0; versionsVectorIterator < versionsVector.size(); versionsVectorIterator++)
                {
                    // *m_IrcSocket << reply::instance().ircPrivmsg((*channelListIterator).first, versionsVector[versionsVectorIterator]);
                    irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*channelListIterator).first + " " + versionsVector[versionsVectorIterator]));
                }
            }*/
            for (size_t versionsVectorIterator = 0; versionsVectorIterator < versionsVector.size(); versionsVectorIterator++)
            {
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), reply::instance().ircBold() + versionsVector[versionsVectorIterator]));
            }
            //returnString += "channels: " + glib::stringFromInt(channelList.size()) + " version lines: " + glib::stringFromInt(versionsVector.size()) + "\r\n";
        }
        if (glib::iequals(command, "rehash"))
        {
            /*std::string returnstring = "PRIVMSG " + chan + " :reading config file now\r\n";
            Send(returnstring);*/
            configreader::instance().clearSettings();
            if(!configreader::instance().readFile())
            {
                output::instance().addStatus(false, "reading config file failed");
                exit(EXIT_FAILURE);
            }
            reply::instance().init();
            binds::instance().init();
            botBindsInit();
        }
        if (glib::iequals(command, "stop"))
        {
            irc::instance().addHighPrioritySendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), reply::instance().ircBold() + "Stopping the bot"));
            m_Run = false;
        }
        if (glib::iequals(command, "restart"))
        {
            /*std::string returnstring = "PRIVMSG " + chan + " :restarting now\r\n";
            Send(returnstring);*/
        }
        if (glib::iequals(command, "listbinds"))
        {
            std::map< std::string, std::map< std::string, binds::bindelement > > bindElements;
            std::map< std::string, std::map< std::string, binds::bindelement > >::iterator bindElementsIterator;
            if (binds::instance().getBinds(bindElements))
            {
                std::map< std::string, binds::bindelement > bindElement;
                std::map< std::string, binds::bindelement >::iterator bindElementIterator;
                for ( bindElementsIterator = bindElements.begin() ; bindElementsIterator != bindElements.end(); bindElementsIterator++ )
                {
                    irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), bindElementsIterator->first));
                    bindElement.clear();
                    bindElement = bindElementsIterator->second;
                    for ( bindElementIterator = bindElement.begin() ; bindElementIterator != bindElement.end(); bindElementIterator++ )
                    {
                        irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), bindElementIterator->first + " " + bindElementIterator->second.command + " " + glib::stringFromInt(bindElementIterator->second.access)));
                    }
                }
            }
        }
        if (glib::iequals(command, "listmodules"))
        {
            for (size_t i = 0; i < m_ModuleList.size(); i++)
            {
                std::string modname = m_ModuleList[i];
                returnString += output::instance().addOutput("[" + glib::stringFromInt(i) + "]" + modname);
                returnString += "\r\n";
                //ai->AddSendQueue(modname);
            }
        }
        if (glib::iequals(command, "reloadall"))
        {
            std::vector< std::string > tmpm_ModuleList = m_ModuleList;
            //std::string reply_string;
            for (size_t i = 0; i < tmpm_ModuleList.size(); i++)
            {
                std::string modname = tmpm_ModuleList[i];
                unLoadModule(modname);
            }
            for (size_t i = 0; i < tmpm_ModuleList.size(); i++)
            {
                std::string modname = tmpm_ModuleList[i];
                loadModule(modname);
            }
        }
        if (glib::iequals(command, "listchannels"))
        {
            std::map< std::string, std::shared_ptr<channel> > l_Channels = channels::instance().get();
            std::map< std::string, std::shared_ptr<channel> >::iterator l_ChannelsIterator;
            for (l_ChannelsIterator = l_Channels.begin(); l_ChannelsIterator != l_Channels.end(); l_ChannelsIterator++)
            {
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_ChannelsIterator).first));
            }
            irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# channels: " + glib::stringFromInt(l_Channels.size())));
        }
        if (glib::iequals(command, "listusers"))
        {
            std::map< std::string, std::shared_ptr<user> > l_Users = users::instance().get();
            std::map< std::string, std::shared_ptr<user> >::iterator l_UsersIterator;
            for (l_UsersIterator = l_Users.begin(); l_UsersIterator != l_Users.end(); l_UsersIterator++)
            {
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_UsersIterator).first));
            }
            irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# users: " + glib::stringFromInt(l_Users.size())));
        }
        if (glib::iequals(command, "listauths"))
        {
            std::map< std::string, std::shared_ptr<auth> > l_Auths = auths::instance().get();
            std::map< std::string, std::shared_ptr<auth> >::iterator l_AuthsIterator;
            for (l_AuthsIterator = l_Auths.begin(); l_AuthsIterator != l_Auths.end(); l_AuthsIterator++)
            {
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_AuthsIterator).first));
            }
            irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# auths: " + glib::stringFromInt(l_Auths.size())));
        }
    }
    if (args.size() == 2)
    {
        if (glib::iequals(command, "reload"))
        {
            unLoadModule(args[1]);
            loadModule(args[1]);
        }
        if (glib::iequals(command, "load"))
        {
            loadModule(args[1]);
        }
        if (glib::iequals(command, "unload"))
        {
            unLoadModule(args[1]);
        }
        if (glib::iequals(command, "listuserchannels"))
        {
            if (users::instance().find(args[1]))
            {
                std::map< std::string, std::shared_ptr<channel> > l_Channels = users::instance().get(args[1])->getChannels();
                std::map< std::string, std::shared_ptr<channel> >::iterator l_ChannelsIterator;
                for (l_ChannelsIterator = l_Channels.begin(); l_ChannelsIterator != l_Channels.end(); l_ChannelsIterator++)
                {
                    irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_ChannelsIterator).first));
                }
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# channels: " + glib::stringFromInt(l_Channels.size())));
            }
        }
        if (glib::iequals(command, "listchannelusers"))
        {
            if (channels::instance().find(args[1]))
            {
                std::map< std::string, std::shared_ptr<user> > l_Users = channels::instance().get(args[1])->getUsers();
                std::map< std::string, std::shared_ptr<user> >::iterator l_UsersIterator;
                for (l_UsersIterator = l_Users.begin(); l_UsersIterator != l_Users.end(); l_UsersIterator++)
                {
                    irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_UsersIterator).first));
                }
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# users: " + glib::stringFromInt(l_Users.size())));
            }
        }
        if (glib::iequals(command, "listauthusers"))
        {
            if (auths::instance().find(args[1]))
            {
                std::map< std::string, std::shared_ptr<user> > l_Users = auths::instance().get(args[1])->getUsers();
                std::map< std::string, std::shared_ptr<user> >::iterator l_UsersIterator;
                for (l_UsersIterator = l_Users.begin(); l_UsersIterator != l_Users.end(); l_UsersIterator++)
                {
                    irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), (*l_UsersIterator).first));
                }
                irc::instance().addSendQueue(reply::instance().ircPrivmsg(configreader::instance().getString("debugchannel"), "# users: " + glib::stringFromInt(l_Users.size())));
            }
        }
        if (glib::iequals(command, "join"))
        {
            irc::instance().addSendQueue(reply::instance().ircJoin(args[1]));
        }
    }
    if (args.size() >= 2)
    {
        if (glib::iequals(command, "part"))
        {
            std::string partReason = "";
            for (size_t argsIterator = 2; argsIterator < (args.size() - 1); argsIterator++)
            {
                partReason = partReason + args[argsIterator] + " ";
            }
            if (args.size() >= 3)
            {
                partReason = partReason + args[args.size()-1];
            }
            irc::instance().addSendQueue(reply::instance().ircPart(args[1], partReason));
        }
        if (glib::iequals(command, "raw"))
        {
            std::string command = "";
            for (size_t argsIterator = 1; argsIterator < (args.size() - 1); argsIterator++)
            {
                command = command + args[argsIterator] + " ";
            }
            if (args.size() >= 2)
            {
                command = command + args[args.size()-1];
            }
            irc::instance().addSendQueue(command);
        }
    }
    if (args.size() == 3)
    {
        if (glib::iequals(command, "delbind"))
        {
            binds::instance().delBind(args[1], args[2]);
        }
    }
    if (args.size() >= 3)
    {
        if (glib::iequals(command, "kick"))
        {
            std::string kickReason = "";
            for (size_t argsIterator = 3; argsIterator < (args.size() - 1); argsIterator++)
            {
                kickReason = kickReason + args[argsIterator] + " ";
            }
            if (args.size() >= 4)
            {
                kickReason = kickReason + args[args.size()-1];
            }
            irc::instance().addSendQueue(reply::instance().ircKick(args[1], args[2], kickReason));
        }
    }
    if (args.size() == 5)
    {
        if (glib::iequals(command, "setbind"))
        {
            binds::instance().setBind(args[1], args[2], glib::intFromString(args[3]), args[4]);
        }
    }
    return returnString;
}
