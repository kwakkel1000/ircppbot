#ifndef Parse_h
#define Parse_h


#include "UserManagementInterface.h"
#include "ModuleInterface.h"
#include <string>
#include <vector>
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
    Parse(IrcSocket*, bool);
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
    std::string configfile;
    bool NS;
    bool run;

    //classes
    Data *D;

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
    void parse_raw();
    void parse_privmsg();
    bool WhoisSend;
    void ParseData(std::vector< std::string > data);
    void PING(std::vector< std::string > data);
    void PRIVMSG(std::vector< std::string > data);
    std::string convertInt(int);
    int convertString(std::string);
    void DBinit();
    bool Send(std::string data);
    bool SendHighPriority(std::string data);
    bool SendLowPriority(std::string data);

    void timerrun();
    std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
    bool RawSql(std::string data);
    std::string HostmaskToNick(std::vector< std::string > data);

    //config vars
    bool chandebug;
    std::string debugchannel;
    std::string hostname_str;
    std::string databasename_str;
    std::string username_str;
    std::string pass_str;
    std::string trigger;
    std::string moduledir;

    void LoadUserThreadLoop();
    void LoadThreadLoop(int i);
    bool timeron;
    boost::shared_ptr< boost::thread > raw_parse_thread;
    boost::shared_ptr< boost::thread > privmsg_parse_thread;
    boost::shared_ptr< boost::thread > timer_thread;
    std::vector< boost::shared_ptr< boost::thread > > module_thread_vector;
};


#endif // Parse_h
