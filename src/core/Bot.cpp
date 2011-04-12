#include "../include/core/Bot.h"

#include "../include/core/ConfigReader.h"
#include "../include/core/IrcData.h"
#include "../include/core/Global.h"
#include "../include/core/DatabaseData.h"
#include "../include/core/Reply.h"
#include "../include/core/Data.h"
#include "../include/socket/IrcSocket.h"
#include "../include/socket/SocketException.h"
#include "../include/management/Users.h"
#include "../include/management/Channels.h"

#include <iostream>
#include <vector>
#include <dlfcn.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

Bot::Bot()
// Initialize all member vars! std::string's will just use their default constructor
: mpIrcSocket( NULL )
{
}

Bot::~Bot()
{
	//old parse
	std::cout << "Bot::~Bot()" << std::endl;
    timeron = false;
    timer_thread->join();
    std::cout << "timer_thread stopped" << std::endl;
	std::vector< std::string > tmpmodulelist = modulelist;
	for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
	{
		std::string modname = tmpmodulelist[i];
		std::cout << "unloading " << modname << std::endl;
		UnLoadModule(modname);
	}


    // For every new you should call a delete (manualy calling destructors is not-done)
    // delete accepts null pointers, no checking needed \o/
    std::cout << "Bot::~Bot()" << std::endl;
    mpIrcSocket->Disconnect();
    std::cout << "mpIrcSocket disconnected" << std::endl;
	delete mpIrcSocket;
    std::cout << "mpIrcSocket deleted" << std::endl;
}

void Bot::Init(std::string configfile)
{
    Global::Instance().set_ConfigReader(new ConfigReader());
    Global::Instance().set_ConfigFile(configfile);
    Global::Instance().get_ConfigReader().ClearSettings();
    if (Global::Instance().get_ConfigReader().ReadFile(configfile))
    {
        std::cout << "W00p config is gelezen \\o/" << std::endl;
    }
    else
    {
        std::cout << "Kon niet lezen :/" << std::endl;
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

void Bot::ModuleInit()
{
	Global& G = Global::Instance();
	ConfigReaderInterface& reader = G.get_ConfigReader();

    G.set_Users(new Users());
    G.set_Channels(new Channels());
    G.set_Reply(new Reply());
    G.get_Reply().Init();
    G.get_IrcData().run();

    std::string loadmodsstr;
    loadmodsstr = reader.GetString("loadmods");
    moduledir = reader.GetString("moduledir");
    LoadModule(reader.GetString("ircserv"));
    std::vector< std::string > loadmods;
    boost::split( loadmods, loadmodsstr, boost::is_any_of(" "), boost::token_compress_on );
    for (unsigned int i = 0; i < loadmods.size(); i++)
    {
        LoadModule(loadmods[i]);
    }
    timeron = true;
    assert(!timer_thread);
    timer_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::TimerRun, this)));
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
        mpIrcSocket->Connect( ircserver, ircport );
    }
    catch (IrcSocket::Exception& e)
    {
        std::cout << "Exception caught: " << e.Description() << " (" << e.Errornr() << ")" << std::endl;
        exit(1);
    }
    G.get_IrcData().init(mpIrcSocket);
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

bool Bot::LoadModule(std::string modulename)
{
    bool loaded = false;
    for (unsigned int i = 0; i < modulelist.size(); i++)
    {
        if (modulelist[i] == modulename)
        {
            loaded = true;
        }
    }
    if (!loaded)
    {
        ModuleInterface* mi;
        void* module;
        create_tmi* create_module;
        destroy_tmi* destroy_module;
        std::string modulepath = "./" + moduledir + modulename + ".so";
        // load the library
        module = dlopen(modulepath.c_str(), RTLD_LAZY);
        if (!module) {
            cerr << "Cannot load library: " << dlerror() << '\n';
            return false;
			exit(1);
            //return 1;
        }

        // load the symbols
        create_module = (create_tmi*) dlsym(module, "create");
        destroy_module = (destroy_tmi*) dlsym(module, "destroy");
        if (!create_module || !destroy_module) {
            cerr << "Cannot load symbols: " << dlerror() << '\n';
            return false;
			exit(1);
            //return 1;
        }
        std::cout << "Module " << modulename << " Loaded" << std::endl;
        // create an instance of the class
        mi = create_module();
        mi->Init(new Data());
        modulelist.push_back(modulename);
        modulevector.push_back(module);
        moduleinterfacevector.push_back(mi);
        createvector.push_back(create_module);
        destroyvector.push_back(destroy_module);
        boost::shared_ptr<boost::thread> tmp_thread;
        int modi = -1;
        for (unsigned int i = 0; i < modulelist.size(); i++)
        {
            if (modulelist[i] == modulename)
            {
                modi = i;
            }
        }
        if (modi >= 0)
        {
            assert(!tmp_thread);
            tmp_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Bot::ModuleRun, this, modi)));
            module_thread_vector.push_back(tmp_thread);
        }
        return true;
    }
    std::cout << "module " << modulename  << " already loaded" << std::endl;
    return false;
}

bool Bot::UnLoadModule(std::string modulename)
{
    int modi = -1;
    for (unsigned int i = 0; i < modulelist.size(); i++)
    {
        if (modulelist[i] == modulename)
        {
            modi = i;
        }
    }
    if (modi >= 0)
    {
        moduleinterfacevector[modi]->stop();
        //module_thread_vector[modi]->join();
        module_thread_vector.erase(module_thread_vector.begin()+modi);
        destroyvector[modi](moduleinterfacevector[modi]);
        moduleinterfacevector.erase(moduleinterfacevector.begin()+modi);
        dlclose(modulevector[modi]);
        modulelist.erase(modulelist.begin()+modi);
        modulevector.erase(modulevector.begin()+modi);
        createvector.erase(createvector.begin()+modi);
        destroyvector.erase(destroyvector.begin()+modi);
        std::cout << modulename << " UnLoaded" << std::endl;
        return true;
    }
    return false;
}

bool Bot::UnLoadModuleId(unsigned int moduleid)
{
    int modi = -1;
    if (moduleid < modulelist.size())
    {
        modi = moduleid;
    }
    if (modi >= 0)
    {
        destroyvector[modi](moduleinterfacevector[modi]);
        dlclose(modulevector[modi]);
        modulelist.erase(modulelist.begin()+modi);
        modulevector.erase(modulevector.begin()+modi);
        moduleinterfacevector.erase(moduleinterfacevector.begin()+modi);
        createvector.erase(createvector.begin()+modi);
        destroyvector.erase(destroyvector.begin()+modi);
        std::cout << modulelist[modi] << " UnLoaded" << std::endl;
        return true;
    }
    return false;
}

void Bot::ModuleRun(int i)
{
    moduleinterfacevector[i]->read();
}

void Bot::AdminCommands(std::string command, std::vector< std::string > args)
{

	if (args.size() == 0)
	{
		if (boost::iequals(command,"rehash"))
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
		if (boost::iequals(command,"stop"))
		{
			/*std::string returnstring = "PRIVMSG " + chan + " :stopping now\r\n";
			Send(returnstring);*/
			Global::Instance().set_Run(false);
		}
		if (boost::iequals(command,"restart"))
		{
			/*std::string returnstring = "PRIVMSG " + chan + " :restarting now\r\n";
			Send(returnstring);*/
			//returnstring = "QUIT \r\n";
			//Send(returnstring);
		}
		if (boost::iequals(command,"listmodules"))
		{
			for (unsigned int i = 0; i < modulelist.size(); i++)
			{
				std::string modname = modulelist[i];
				/*std::cout << "[" << i << "]" << modname << std::endl;*/
				ai->AddSendQueue(modname);
			}
		}
		if (boost::iequals(command,"reloadall"))
		{
			std::vector< std::string > tmpmodulelist = modulelist;
			std::string reply_string;
			/*reply_string = "NOTICE " + nick + " :unloading " + convertInt(tmpmodulelist.size()) + " modules\r\n";
			Send(reply_string);*/
			//UnLoadIrcserv();
			for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
			{
				std::string modname = tmpmodulelist[i];
				/*std::string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " unloading\r\n";
				Send(returnstring);*/
				UnLoadModule(modname);
			}
			//ReloadAll();
			for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
			{
				std::string modname = tmpmodulelist[i];
				LoadModule(modname);
				/*std::string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " loaded\r\n";
				Send(returnstring);*/
			}
			//LoadIrcserv(Global::Instance().get_ConfigReader().GetString("ircserv"));
			/*reply_string = "NOTICE " + nick + " :Reloaded " + convertInt(tmpmodulelist.size()) + " modules\r\n";
			Send(reply_string);*/
		}
	}
	if (args.size() == 1)
	{
		if (boost::iequals(command,"reload"))
		{
			UnLoadModule(args[0]);
			/*std::string returnstring = "NOTICE " + nick + " :" + args[0] + " unloading\r\n";
			Send(returnstring);*/
			LoadModule(args[0]);
			/*returnstring = "NOTICE " + nick + " :" + args[0] + " loaded\r\n";
			Send(returnstring);*/
		}
		if (boost::iequals(command,"load"))
		{
			LoadModule(args[0]);
			/*std::string returnstring = "NOTICE " + nick + " :" + args[0] + " loaded\r\n";
			Send(returnstring);*/
		}
		if (boost::iequals(command,"unload"))
		{
			UnLoadModule(args[0]);
			/*std::string returnstring = "NOTICE " + nick + " :" + args[0] + " unloading\r\n";
			Send(returnstring);*/
		}
	}
}

void Bot::Run()
{
    std::cout << "void Bot::Run()" << std::endl;
    //need a mainthread loop;
    while(1)
    {
    	usleep(1000000);
    }
    //admin_thread->join();
}

void Bot::AdminRun()
{
    std::cout << "AdminRun" << std::endl;
    ai->Run();
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
		//P->read();
	}
	catch (IrcSocket::Exception& e)
	{
		std::cout << "Exception caught: " << e.Description() << std::endl;
	}
}

void Bot::TimerRun()
{
    while(timeron)
    {
        usleep(1000000);
        for (unsigned int i = 0; i < moduleinterfacevector.size(); i++)
        {
            moduleinterfacevector[i]->timerrun();
        }
    }
}


int Bot::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}
