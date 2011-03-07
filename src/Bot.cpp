#include "../include/Bot.h"

#include "../include/ServerSocket.h"
#include "../include/IrcSocket.h"
#include "../include/SocketException.h"
#include "../include/Parse.h"
#include "../include/ConfigReader.h"
#include "../include/IrcData.h"
#include "../include/Global.h"
#include <iostream>
#include <vector>

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
    parse_sock->Disconnect();
	delete P;
	delete parse_sock;
}

void Bot::Init(string configfile)
{
    Global::Instance().set_ConfigReader(new ConfigReader());
    if (Global::Instance().get_ConfigReader().ReadFile(configfile))
    {
        cout << "W00p config is gelezen \\o/" << endl;
        read = true;
    }
    else
    {
        read = false;
        cout << "Kon niet lezen :/" << endl;
    }
    parseinit();
//    LoadAdmin();
//    admininit();
}

void Bot::admininit()
{
    cout << "admininit" << endl;
    // Create the socket
    ServerSocket *server = new ServerSocket( 3344 );
    ai->Init(P, server);
    assert(!admin_thread);
    admin_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::adminrun, this)));
}

void Bot::adminrun()
{
    cout << "adminrun" << endl;
    ai->Run();
}

void Bot::parseinit()
{
    Global& G = Global::Instance();
    G.set_IrcData(new IrcData());
    cout << "parseinit" << endl;
    std::string nickserv = G.get_ConfigReader().GetString("nickserv");
    std::string botnick = G.get_ConfigReader().GetString("botnick");
    std::string ircserver = G.get_ConfigReader().GetString("ircserver");
    std::string ircport = G.get_ConfigReader().GetString("ircport");
    //floodprotect = reader->GetString("floodprotect");
    //floodbuffer = reader->GetString("floodbuffer");
    //floodtime = reader->GetString("floodtime");
    bool ns(nickserv == "true");
    try
    {
        parse_sock = new IrcSocket();
        parse_sock->Connect( ircserver, ircport );
    }
    catch (IrcSocket::Exception& e)
    {
        cout << "Exception caught: " << e.Description() << " (" << e.Errornr() << ")" << endl;
        exit(1);
    }
    P = new Parse(botnick, parse_sock, ns);
    assert(!parse_thread);
    parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::parserun, this)));
}

void Bot::parserun()
{
    cout << "parserun" << endl;
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
            cout << "Exception caught: " << e.Description() << endl;
        }
    }
}

void Bot::Run()
{
    //admin_thread->join();
    parse_thread->join();
}

void Bot::LoadAdmin()
{
    // load the admin library
    admin = dlopen("./Admin.so", RTLD_LAZY);
    if (!admin) {
        cerr << "Cannot load library: " << dlerror() << '\n';
        //return 1;
    }

    // load the symbols
    create_admin = (create_tai*) dlsym(admin, "create");
    destroy_admin = (destroy_tai*) dlsym(admin, "destroy");
    if (!create_admin || !destroy_admin) {
        cerr << "Cannot load symbols: " << dlerror() << '\n';
        //return 1;
    }
    cout << "admin Loaded" << endl;
    // create an instance of the class
    ai = create_admin();
}


void Bot::UnLoadAdmin()
{
    // destroy the class
    destroy_admin(ai);

    // unload the channelbot library
    dlclose(admin);
    cout << "admin UnLoaded" << endl;

}
