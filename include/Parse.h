#ifndef Parse_h
#define Parse_h


#include "UserManagementInterface.h"
#include "ModuleInterface.h"
#include "ConfigReader.h"
#include "Reply.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <dlfcn.h>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

// Predefinitions
class Global;
class ClientSocket;
class Users;
class Channels;
class database;
class Reply;
class Data;
class Parse
{
public:
    Parse(string, IrcSocket*, bool, ConfigReader&);
    virtual ~Parse();

    virtual void read( );
    virtual void LoadAuthserv();
    virtual void UnLoadAuthserv();
    virtual void LoadNickserv();
    virtual void UnLoadNickserv();
    virtual bool LoadModule(string);
    virtual bool UnLoadModule(string);
    virtual bool UnLoadModuleId(unsigned int);
    void ReloadAll();
    void ModuleParse(int, std::vector< std::string >);

private:
    //vars
    string configfile;
    bool NS;

    //classes
    ConfigReader& reader;
    //IrcData *ID;
    Data *D;
    IrcSocket *S;
    Users *U;
    Channels *C;
    //Reply *R;

    //service modules
    UserManagementInterface* umi;
    void* authserv;
    void* nickserv;
    create_tUMI* create_authserv;
    destroy_tUMI* destroy_authserv;
    create_tUMI* create_nickserv;
    destroy_tUMI* destroy_nickserv;

    //modules
    std::vector< std::string > modulelist;
    std::vector< void* > modulevector;
    std::vector< ModuleInterface* > moduleinterfacevector;
    std::vector< create_tmi* > createvector;
    std::vector< destroy_tmi* > destroyvector;

    //functions
    bool WhoisSend;
    void ParseData(std::vector< std::string > data);
    void PING(std::vector< std::string > data);
    void PRIVMSG(std::vector< std::string > data);
    std::string convertInt(int);
    int convertString(std::string);
    void DBinit();
    bool Send(std::string data);

    void timerrun();
    std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
    bool RawSql(std::string data);
    std::string HostmaskToNick(std::vector<std::string> data);

    //config vars
    std::string botnick;
    bool chandebug;
    std::string debugchannel;
    std::string hostname_str;
    std::string databasename_str;
    std::string username_str;
    std::string pass_str;
    std::string trigger;
    std::string moduledir;

    void LoadThreadLoop(int i);
    bool timeron;
    boost::shared_ptr<boost::thread> timer_thread;
    vector< boost::shared_ptr<boost::thread> > module_thread_vector;
};


#endif // Parse_h
