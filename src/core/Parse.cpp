#include "../include/core/Parse.h"

#include "../include/management/Users.h"
#include "../include/management/Channels.h"
#include "../include/core/Database.h"
#include "../include/core/Global.h"
#include "../include/core/Reply.h"
#include "../include/interfaces/ConfigReaderInterface.h"
#include "../include/interfaces/UsersInterface.h"

#include <boost/algorithm/string.hpp>
#include <dlfcn.h>


Parse::Parse(bool ns)
{
    NS=ns;
    Global& G = Global::Instance();
	ConfigReaderInterface& reader = G.get_ConfigReader();

    D = new Data();
    D->Init(true, false, false, true);
    G.set_Reply(new Reply());
    G.set_Users(new Users());
    G.set_Channels(new Channels());

    G.get_Reply().Init();

    G.get_IrcData().AddConsumer(D);
    G.get_IrcData().run();
    /*ID->
    ID->run();*/
    std::string chandebugstr;
    std::string loadmodsstr;
    hostname_str = reader.GetString("hostname");
    databasename_str = reader.GetString("databasename");
    username_str = reader.GetString("username");
    pass_str = reader.GetString("password");
    trigger = reader.GetString("trigger");
    debugchannel = reader.GetString("debugchannel");
    chandebugstr = reader.GetString("chandebug");
    loadmodsstr = reader.GetString("loadmods");
    moduledir = reader.GetString("moduledir");

    chandebug = (chandebugstr == "true");


    std::vector< std::string > loadmods;
    boost::split( loadmods, loadmodsstr, boost::is_any_of(" "), boost::token_compress_on );
    for (unsigned int i = 0; i < loadmods.size(); i++)
    {
        LoadModule(loadmods[i]);
    }
    DBinit();
    if (NS)
    {
        LoadNickserv();
    }
    else
    {
        LoadAuthserv();
    }
    timeron = true;
    assert(!timer_thread);
    timer_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::timerrun, this)));
}

Parse::~Parse()
{
    std::cout << "Parse::~Parse()" << std::endl;
    timeron = false;
    timer_thread->join();
    std::cout << "timer_thread stopped" << std::endl;
    // For every new you should call a delete (manualy calling destructors is not-done)
    // delete accepts null pointers, no checking needed \o/
    /*delete U;
    delete C;*/
	std::vector< std::string > tmpmodulelist = modulelist;
	for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
	{
		std::string modname = tmpmodulelist[i];
		std::cout << "unloading " << modname << std::endl;
		UnLoadModule(modname);
	}
	if (NS == true)
	{
		UnLoadNickserv();
	}
	else
	{
		UnLoadAuthserv();
	}
	/*Global::Instance().get_IrcData().DelConsumer(D);
    delete D;*/
}

void Parse::LoadAuthserv()
{
    // load the authserv library
    std::string modulepath = "./" + moduledir + "Authserv.so";
    authserv = dlopen(modulepath.c_str(), RTLD_LAZY);
    if (!authserv) {
        cerr << "Cannot load library: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }

    // load the symbols
    create_authserv = (create_tUMI*) dlsym(authserv, "create");
    destroy_authserv = (destroy_tUMI*) dlsym(authserv, "destroy");
    if (!create_authserv || !destroy_authserv) {
        cerr << "Cannot load symbols: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }
    std::cout << "authserv Loaded" << std::endl;
    // create an instance of the class
    umi = create_authserv();
    umi->Init();
    umi->read();
    /*assert(!user_thread);
    user_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::LoadUserThreadLoop, this)));*/
}

void Parse::UnLoadAuthserv()
{
    // destroy the class
    destroy_authserv(umi);

    // unload the channelbot library
    dlclose(authserv);
    cout << "authserv UnLoaded" << endl;

}

void Parse::LoadNickserv()
{
    // load the nickserv library
    std::string modulepath = "./" + moduledir + "Nickserv.so";
    nickserv = dlopen(modulepath.c_str(), RTLD_LAZY);
    if (!nickserv) {
        cerr << "Cannot load library: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }

    // load the symbols
    create_nickserv = (create_tUMI*) dlsym(nickserv, "create");
    destroy_nickserv = (destroy_tUMI*) dlsym(nickserv, "destroy");
    if (!create_nickserv || !destroy_nickserv) {
        cerr << "Cannot load symbols: " << dlerror() << '\n';
        exit(1);
        //return 1;
    }
    std::cout << "nickserv Loaded" << std::endl;
    // create an instance of the class
    umi = create_nickserv();
    umi->Init();
    umi->read();
    /*assert(!user_thread);
    user_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::LoadUserThreadLoop, this)));*/
}

void Parse::UnLoadNickserv()
{
    // destroy the class
    destroy_nickserv(umi);

    // unload the channelbot library
    dlclose(nickserv);
    cout << "nickserv UnLoaded" << endl;
}

void Parse::LoadUserThreadLoop()
{
    umi->read();
}

void Parse::LoadThreadLoop(int i)
{
    moduleinterfacevector[i]->read();
}

bool Parse::LoadModule(std::string modulename)
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
            tmp_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::LoadThreadLoop, this, modi)));
            module_thread_vector.push_back(tmp_thread);
        }
        return true;
    }
    std::cout << "module " << modulename  << " already loaded" << std::endl;
    return false;
}


bool Parse::UnLoadModule(std::string modulename)
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


bool Parse::UnLoadModuleId(unsigned int moduleid)
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

void Parse::read()
{
    run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::parse_raw, this)));
    assert(!privmsg_parse_thread);
    privmsg_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Parse::parse_privmsg, this)));
    raw_parse_thread->join();
    privmsg_parse_thread->join();
}

void Parse::parse_raw()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetRawQueue();
        ParseData(data);
    }
}

void Parse::parse_privmsg()
{
    std::vector< std::string > data;
    while(run)
    {
        data = D->GetPrivmsgQueue();
        PRIVMSG(data);
    }
}

void Parse::ParseData(std::vector< std::string > data)
{
    if (data.size() == 2)
    {
        if (data[0] == "PING")      //PING
        {
            PING(data);
        }
    }
    if (data.size() >= 4)
    {
        if (data[1] == "001")   //welcome
        {
            //Global::Instance().set_BotNick(data[2]);
        }
    }
}

void Parse::PING(std::vector< std::string > data)
{
    std::string returnstr = "PONG " + data[1] + "\r\n";
    SendHighPriority(returnstr);
}


void Parse::PRIVMSG(std::vector< std::string > data)
{
    std::vector< std::string > args;
    std::string data3;
    size_t chanpos1;
    size_t chanpos2;
    size_t triggerpos;
    chanpos1 = data[2].find("#");
    chanpos2 = data[3].find("#");
    triggerpos = data[3].find(trigger);
    std::string chan = "NULL";
    std::string command = "NULL";
    int triggered = 0;
    int triggertype = -1;
    int chantrigger = -1;
    if (data.size() >= 4)
    {
        data3 = data[3];
        boost::erase_all(data3, ":");
        //data3 = Split(data[3], ":",true,true);
    }
    if (triggerpos != std::string::npos)
    {
        triggertype = 1; //PRIVMSG ... :!;
        if (data3 != "")
        {
            if (chanpos1 != std::string::npos && chanpos2 != std::string::npos)
            {
                chantrigger = 1;    //PRIVMSG nick #channel :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    boost::erase_all(chan, trigger);
                    if (chan != "")
                    {
                        triggered = 1;
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 != std::string::npos && chanpos2 == std::string::npos)
            {
                chantrigger = 0;    //PRIVMSG nick #chan :!command
                cout << "channel: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    command = data3;
                    boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        chan = data[2];
                        triggered = 1;
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 == std::string::npos && chanpos2 == std::string::npos)
            {
                chantrigger = -1;   //PRIVMSG nick user :!command
                if (data.size() >= 4)
                {
                    command = data3;
                    chan = "NULL";
                    boost::erase_all(command, trigger);
                    if (command != "")
                    {
                        triggered = 1;
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 == std::string::npos && chanpos2 != std::string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :!#chan command
                if (data.size() >= 5)
                {
                    chan = data3;
                    boost::erase_all(chan, trigger);
                    if (chan != "")
                    {
                        triggered = 1;
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (data3 != "")
        {
            if (chanpos1 == std::string::npos && chanpos2 != std::string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :#chan command
                if (data.size() >= 5)
                {
                    triggered = 1;
                    chan = data3;
                    //chan = data3[0];
                    command = data[4];
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                }
            }
            if (chanpos1 == std::string::npos && chanpos2 == std::string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :command
                if (data.size() >= 5)
                {
                    chan == "NULL";
                    triggered = 1;
                    //command = data3[0];
                    command = data3;
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                }
            }
        }
    }
    if (triggered == 1)
    {
        std::string nick = HostmaskToNick(data);
        std::string auth = Global::Instance().get_Users().GetAuth(nick);
        if (args.size() == 0)
        {
            if (boost::iequals(command,"stop"))
            {
                std::string returnstring = "PRIVMSG " + chan + " :stopping now\r\n";
                Send(returnstring);
                Global::Instance().set_Run(false);
                run = false;
            }
            if (boost::iequals(command,"restart"))
            {
                std::string returnstring = "PRIVMSG " + chan + " :restarting now\r\n";
                Send(returnstring);
                //returnstring = "QUIT \r\n";
                //Send(returnstring);
                run = false;
            }
            if (boost::iequals(command,"listmodules"))
            {
                for (unsigned int i = 0; i < modulelist.size(); i++)
                {
                    std::string modname = modulelist[i];
                    std::string returnstring = "PRIVMSG " + chan + " :[" + convertInt(i) + "] " + modname + "\r\n";
                    Send(returnstring);
                }
            }
            if (boost::iequals(command,"reloadall"))
            {
                std::vector< std::string > tmpmodulelist = modulelist;
                std::string reply_string;
                reply_string = "NOTICE " + nick + " :unloading " + convertInt(tmpmodulelist.size()) + " modules\r\n";
                Send(reply_string);
                for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
                {
                    std::string modname = tmpmodulelist[i];
                    UnLoadModule(modname);
                    std::string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " unloading\r\n";
                    Send(returnstring);
                }
                //ReloadAll();
                for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
                {
                    std::string modname = tmpmodulelist[i];
                    LoadModule(modname);
                    std::string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " loaded\r\n";
                    Send(returnstring);
                }
                if (NS == true)
                {
                    UnLoadNickserv();
                    LoadNickserv();
                }
                else
                {
                    UnLoadAuthserv();
                    LoadAuthserv();
                }
                reply_string = "NOTICE " + nick + " :Reloaded " + convertInt(tmpmodulelist.size()) + " modules\r\n";
                Send(reply_string);
            }
        }
        if (args.size() == 1)
        {
            if (boost::iequals(command,"reload"))
            {
                UnLoadModule(args[0]);
                std::string returnstring = "NOTICE " + nick + " :" + args[0] + " unloading\r\n";
                Send(returnstring);
                LoadModule(args[0]);
                returnstring = "NOTICE " + nick + " :" + args[0] + " loaded\r\n";
                Send(returnstring);
            }
            if (boost::iequals(command,"load"))
            {
                LoadModule(args[0]);
                std::string returnstring = "NOTICE " + nick + " :" + args[0] + " loaded\r\n";
                Send(returnstring);
            }
            if (boost::iequals(command,"unload"))
            {
                UnLoadModule(args[0]);
                std::string returnstring = "NOTICE " + nick + " :" + args[0] + " unloading\r\n";
                Send(returnstring);
            }
        }
    }
}

void Parse::ReloadAll()
{
    std::vector< std::string > tmpmodulelist = modulelist;
    for (unsigned int i = tmpmodulelist.size(); i >= 0; i--)
    {
        std::string modname = tmpmodulelist[i];
        UnLoadModule(modname);
    }
    for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
    {
        std::string modname = tmpmodulelist[i];
        LoadModule(modname);
    }
    if (NS == true)
    {
        UnLoadNickserv();
        LoadNickserv();
    }
    else
    {
        UnLoadAuthserv();
        LoadAuthserv();
    }
}

std::string Parse::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int Parse::convertString(std::string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

void Parse::DBinit()
{
    std::vector< std::vector< std::string > > sql_result;
    std::string sql_string = "select auth from auth";
    sql_result = RawSqlSelect(sql_string);
    UsersInterface& U = Global::Instance().get_Users();
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        U.AddAuth(sql_result[i][0]);
        std::cout << sql_result[i][0] << std::endl;
    }
}

std::vector< std::vector< std::string > > Parse::RawSqlSelect(std::string data)
{
    std::cout << data << std::endl;
    database *db;
    std::vector< std::vector< std::string > > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    int tmp = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (tmp == 200)
    {
        sql_result = db->sql_query( data.c_str() );
    }
    else
    {
        std::cout << "database error" << std::endl;
        exit(0);
    }
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
    delete db;
    return sql_result;
}

bool Parse::RawSql(std::string data)
{
    std::cout << data << std::endl;
    database *db;
    db = new database();    // lol whut... connecting for each query? :'D
    int tmp = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (tmp == 200)
    {
        db->updateQuery( data.c_str() );
    }
    else
    {
        cout << "database error" << endl;
        exit(0);
    }
    db->disconnect();
    delete db;
    return true;
}

std::string Parse::HostmaskToNick(std::vector< std::string > data)
{
    std::vector< std::string > who;
    boost::split( who, data[0], boost::is_any_of("!"), boost::token_compress_on );
    std::string nick = who[0];
    boost::erase_all(nick, ":");
    return nick;
}

bool Parse::Send(std::string data)
{
    Global::Instance().get_IrcData().AddSendQueue(data);
    return true;
}

bool Parse::SendHighPriority(std::string data)
{
    Global::Instance().get_IrcData().AddHighPrioritySendQueue(data);
    return true;
}

bool Parse::SendLowPriority(std::string data)
{
    Global::Instance().get_IrcData().AddLowPrioritySendQueue(data);
    return true;
}

void Parse::timerrun()
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
