#include "../include/Parse.h"

#include "../include/IrcSocket.h"
#include "../include/Users.h"
#include "../include/Channels.h"
#include "../include/Database.h"

#include <boost/algorithm/string.hpp>


Parse::Parse(std::string nick, IrcSocket *s, bool ns, ConfigReader& cf, IrcData *id)
: reader( cf )
{
    NS=ns;
    S=s;
    ID=id;
    ID->init(S);
    R = new Reply();
    R->Init(reader);
    U = new Users();
    C = new Channels();
    D = new Data();
    D->Init(true, false, false, true);
    ID->AddConsumer(D);
    ID->run();
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


    vector<string> loadmods;
    boost::split( loadmods, loadmodsstr, boost::is_any_of(" "), boost::token_compress_on );
    for (unsigned int i = 0; i < loadmods.size(); i++)
    {
        LoadModule(loadmods[i]);
    }
    botnick = nick;
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
    timeron = false;
    timer_thread->join();
    // For every new you should call a delete (manualy calling destructors is not-done)
    // delete accepts null pointers, no checking needed \o/
    delete U;
    delete C;
    delete S;
    for (unsigned int i = modulelist.size(); i >= 0; i--)
    {
        UnLoadModule(modulelist[i]);
    }
    if (NS)
    {
        UnLoadNickserv();
    }
    else
    {
        UnLoadAuthserv();
    }
}

void Parse::LoadAuthserv()
{
    // load the authserv library
    string modulepath = "./" + moduledir + "Authserv.so";
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
    cout << "authserv Loaded" << endl;
    // create an instance of the class
    umi = create_authserv();
    umi->Init(botnick, S, U, C, reader, ID);
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
    string modulepath = "./" + moduledir + "Nickserv.so";
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
    cout << "nickserv Loaded" << endl;
    // create an instance of the class
    umi = create_nickserv();
    umi->Init(botnick, S, U, C, reader, ID);
}

void Parse::UnLoadNickserv()
{
    // destroy the class
    destroy_nickserv(umi);

    // unload the channelbot library
    dlclose(nickserv);
    cout << "nickserv UnLoaded" << endl;
}

void Parse::LoadThreadLoop(int i)
{
    moduleinterfacevector[i]->threadloop();
}

bool Parse::LoadModule(string modulename)
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
        string modulepath = "./" + moduledir + modulename + ".so";
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
        cout << "Module " << modulename << " Loaded" << endl;
        // create an instance of the class
        mi = create_module();
        mi->BaseInit(botnick, U, C, reader, ID, R);
        mi->Init();
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
    cout << "module " << modulename  << " already loaded" << endl;
    return false;
}


bool Parse::UnLoadModule(string modulename)
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
        moduleinterfacevector[modi]->stopthreadloop();
        module_thread_vector[modi]->join();
        module_thread_vector.erase(module_thread_vector.begin()+modi);
        destroyvector[modi](moduleinterfacevector[modi]);
        dlclose(modulevector[modi]);
        modulelist.erase(modulelist.begin()+modi);
        modulevector.erase(modulevector.begin()+modi);
        moduleinterfacevector.erase(moduleinterfacevector.begin()+modi);
        createvector.erase(createvector.begin()+modi);
        destroyvector.erase(destroyvector.begin()+modi);
        cout << modulename << " UnLoaded" << endl;
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
        cout << modulelist[modi] << " UnLoaded" << endl;
        return true;
    }
    return false;
}

void Parse::read()
{
    std::vector< std::string > raw_result;
    std::vector< std::string > privmsg_result;
    while (1)
    {
        try
        {
            raw_result = D->GetRawQueue();
            privmsg_result = D->GetPrivmsgQueue();
            ParseData(privmsg_result);
            umi->ParseData(privmsg_result);
            //PRIVMSG(privmsg_result);
        }
        catch (string e)
        {
          std::cout << "Exception was caught:" << e << "\n";
        }
    }
}

/*void Parse::ModuleParse(int i, std::vector< std::string > result)
{
    moduleinterfacevector[i]->ParseData(result);
}*/

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
            botnick = data[2];
        }
        if (data[1] == "PRIVMSG")   //PRIVMSG
        {
            PRIVMSG(data);
        }
    }
}

void Parse::PING(std::vector< std::string > data)
{
    std::string returnstr = "PONG " + data[1] + "\r\n";
    Send(returnstr);
}


void Parse::PRIVMSG(std::vector< std::string > data)
{
    std::vector< std::string > args;
    //vector<string> data3;
    std::string data3;
    size_t chanpos1;
    size_t chanpos2;
    size_t triggerpos;
    chanpos1 = data[2].find("#");
    chanpos2 = data[3].find("#");
    triggerpos = data[3].find(trigger);
    string chan = "NULL";
    string command = "NULL";
    int triggered = 0;
    int triggertype = -1;
    int chantrigger = -1;
    if (data.size() >= 4)
    {
        data3 = data[3];
        boost::erase_all(data3, ":");
        //data3 = Split(data[3], ":",true,true);
    }
    if (triggerpos != string::npos)
    {
        triggertype = 1; //PRIVMSG ... :!;
        if (data3 != "")
        {
            if (chanpos1 != string::npos && chanpos2 != string::npos)
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
            else if (chanpos1 != string::npos && chanpos2 == string::npos)
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
            else if (chanpos1 == string::npos && chanpos2 == string::npos)
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
            else if (chanpos1 == string::npos && chanpos2 != string::npos)
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
            if (chanpos1 == string::npos && chanpos2 != string::npos)
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
            if (chanpos1 == string::npos && chanpos2 == string::npos)
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
        string nick = HostmaskToNick(data);
        string auth = U->GetAuth(nick);
        if (args.size() == 0)
        {
            if (boost::iequals(command,"listmodules"))
            {
                for (unsigned int i = 0; i < modulelist.size(); i++)
                {
                    string modname = modulelist[i];
                    string returnstring = "PRIVMSG " + chan + " :[" + convertInt(i) + "] " + modname + "\r\n";
                    Send(returnstring);
                }
            }
            if (boost::iequals(command,"reloadall"))
            {
                vector<string> tmpmodulelist = modulelist;
                string reply_string;
                reply_string = "NOTICE " + nick + " :unloading " + convertInt(tmpmodulelist.size()) + " modules\r\n";
                Send(reply_string);
                for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
                {
                    string modname = tmpmodulelist[i];
                    UnLoadModule(modname);
                    string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " unloading\r\n";
                    Send(returnstring);
                }
                //ReloadAll();
                for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
                {
                    string modname = tmpmodulelist[i];
                    LoadModule(modname);
                    string returnstring = "NOTICE " + nick + " :[" + convertInt(i) + "] " + modname + " loaded\r\n";
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
                LoadModule(args[0]);
            }
            if (boost::iequals(command,"load"))
            {
                LoadModule(args[0]);
            }
            if (boost::iequals(command,"unload"))
            {
                /*for (int i = 0; i < thread_vector.size(); i++)
                {
                    thread_vector[i]->join();
                }*/
                UnLoadModule(args[0]);
                //UnLoadModuleId(convertString(args[0]));
            }
        }
    }
}

void Parse::ReloadAll()
{
    vector<string> tmpmodulelist = modulelist;
    for (unsigned int i = tmpmodulelist.size(); i >= 0; i--)
    {
        string modname = tmpmodulelist[i];
        UnLoadModule(modname);
    }
    for (unsigned int i = 0; i < tmpmodulelist.size(); i++)
    {
        string modname = tmpmodulelist[i];
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

string Parse::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int Parse::convertString(string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

void Parse::DBinit()
{
    vector< vector<string> > sql_result;
    string sql_string = "select auth from auth";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        U->AddAuth(sql_result[i][0]);
        cout << sql_result[i][0] << endl;
    }
}

vector< vector<string> > Parse::RawSqlSelect(string data)
{
    cout << data << endl;
    database *db;
    vector< vector<string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    int tmp = db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    if (tmp == 200)
    {
        sql_result = db->sql_query( data.c_str() );
    }
    else
    {
        cout << "database error" << endl;
        exit(0);
    }
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
    delete db;
    return sql_result;
}

bool Parse::RawSql(string data)
{
    cout << data << endl;
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

string Parse::HostmaskToNick(vector<string> data)
{
    vector<string> who;
    boost::split( who, data[0], boost::is_any_of("!"), boost::token_compress_on );
    string nick = who[0];
    boost::erase_all(nick, ":");
    return nick;
}

bool Parse::Send(string data)
{
    ID->AddSendQueue(data);
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
