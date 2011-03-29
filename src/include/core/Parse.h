#ifndef Parse_h
#define Parse_h

#include "../interfaces/UserManagementInterface.h"
#include "../interfaces/ModuleInterface.h"
#include <boost/shared_ptr.hpp>

#include "../core/Data.h"

// Predefinitions
class Data;
class Parse
{
public:
    Parse();
    virtual ~Parse();

    virtual void read( );
    virtual void LoadIrcserv(std::string);
    virtual void UnLoadIrcserv();
    virtual bool LoadModule(std::string);
    virtual bool UnLoadModule(std::string);
    virtual bool UnLoadModuleId(unsigned int);
    void ReloadAll();
    void ModuleParse(int, std::vector< std::string >);

    friend class AdminInterface;
    friend class SslAdmin;

private:
    //vars
    bool run;

    //classes
    Data *D;

    //service modules
    UserManagementInterface* umi;
    void* ircserv;
    create_tUMI* create_ircserv;
    destroy_tUMI* destroy_ircserv;

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

    void LoadThreadLoop(int i);
    bool timeron;
    boost::shared_ptr< boost::thread > raw_parse_thread;
    boost::shared_ptr< boost::thread > privmsg_parse_thread;
    boost::shared_ptr< boost::thread > timer_thread;
    std::vector< boost::shared_ptr< boost::thread > > module_thread_vector;
};


#endif // Parse_h
