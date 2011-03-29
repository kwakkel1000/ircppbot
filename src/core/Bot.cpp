#include "../include/core/Bot.h"

#include "../include/core/Parse.h"
#include "../include/core/ConfigReader.h"
#include "../include/core/IrcData.h"
#include "../include/core/Global.h"
#include "../include/socket/IrcSocket.h"
#include "../include/socket/SocketException.h"
#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <boost/bind.hpp>

Bot::Bot()
// Initialize all member vars! std::string's will just use their default constructor
: parse_sock( NULL )
, P( NULL )
{
    read = false;
}

Bot::~Bot()
{
    // For every new you should call a delete (manualy calling destructors is not-done)
    // delete accepts null pointers, no checking needed \o/
    std::cout << "Bot::~Bot()" << std::endl;
    parse_sock->Disconnect();
    std::cout << "parse_sock disconnected" << std::endl;
	delete P;
    std::cout << "P deleted" << std::endl;
	delete parse_sock;
    std::cout << "parse_sock deleted" << std::endl;
}

void Bot::Init(std::string configfile)
{
    Global::Instance().set_ConfigReader(new ConfigReader());
    Global::Instance().set_ConfigFile(configfile);
    Global::Instance().get_ConfigReader().ClearSettings();
    if (Global::Instance().get_ConfigReader().ReadFile(configfile))
    {
        std::cout << "W00p config is gelezen \\o/" << std::endl;
        read = true;
    }
    else
    {
        read = false;
        std::cout << "Kon niet lezen :/" << std::endl;
    }
    parseinit();
    LoadAdmin();
    admininit();
}

void Bot::admininit()
{
    std::cout << "admininit" << std::endl;
    ai->Init(P, 2345);
    assert(!admin_thread);
    admin_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::adminrun, this)));
}

void Bot::adminrun()
{
    std::cout << "adminrun" << std::endl;
    ai->Run();
}


void Bot::parseinit()
{
    Global& G = Global::Instance();
    G.set_IrcData(new IrcData());
    std::cout << "parseinit" << std::endl;
    G.set_BotNick(G.get_ConfigReader().GetString("botnick"));
    std::string nickserv = G.get_ConfigReader().GetString("nickserv");
    std::string ircserver = G.get_ConfigReader().GetString("ircserver");
    std::string ircport = G.get_ConfigReader().GetString("ircport");
    try
    {
        parse_sock = new IrcSocket();
        parse_sock->Connect( ircserver, ircport );
    }
    catch (IrcSocket::Exception& e)
    {
        std::cout << "Exception caught: " << e.Description() << " (" << e.Errornr() << ")" << std::endl;
        exit(1);
    }
    G.get_IrcData().init(parse_sock);
    P = new Parse();
    assert(!parse_thread);
    parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::parserun, this)));
}

void Bot::parserun()
{
    std::cout << "parserun" << std::endl;
    Global& G = Global::Instance();
    std::string ident = G.get_ConfigReader().GetString("ident");
    std::string realname = G.get_ConfigReader().GetString("realname");
    std::string ircpass = G.get_ConfigReader().GetString("ircpass");
    std::string botnick = G.get_ConfigReader().GetString("botnick");
    std::string USER = "USER " + ident + " * * :" + realname + "\r\n";
    std::string PASS = "PASS " + ircpass + "\r\n";
    std::string NICK = "NICK " + botnick + "\r\n";
    // In theory it is possible to call Run before Init. S and P would not be initialized then ;)
    if (P)
    {
        try
        {
            parse_sock->Send(USER);
            parse_sock->Send(PASS);
            parse_sock->Send(NICK);
            P->read();
        }
        catch (IrcSocket::Exception& e)
        {
            std::cout << "Exception caught: " << e.Description() << std::endl;
        }
    }
}

void Bot::Run()
{
    std::cout << "void Bot::Run()" << std::endl;
    admin_thread->join();
    parse_thread->join();
    std::cout << "parse_thread stopped" << std::endl;
}

void Bot::LoadAdmin()
{
    // load the admin library
    admin = dlopen(".libs/SslAdmin.so", RTLD_LAZY);
    if (!admin) {
        std::cerr << "Cannot load library: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }

    // load the symbols
    create_admin = (create_tai*) dlsym(admin, "create");
    destroy_admin = (destroy_tai*) dlsym(admin, "destroy");
    if (!create_admin || !destroy_admin) {
        std::cerr << "Cannot load symbols: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }
    std::cout << "admin Loaded" << std::endl;
    // create an instance of the class
    ai = create_admin();
}


void Bot::UnLoadAdmin()
{
    // destroy the class
    destroy_admin(ai);

    // unload the channelbot library
    dlclose(admin);
    std::cout << "admin UnLoaded" << std::endl;

}
