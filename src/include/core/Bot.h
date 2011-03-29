#ifndef Bot_h
#define Bot_h

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include "../interfaces/AdminInterface.h"
#include "../interfaces/ModuleInterface.h"

// Predefinitions
class IrcSocket;
class AdminInterface;
class Bot
{
public:
    Bot();
    ~Bot();

    void Init (std::string);
    void IrcInit();
    void ModuleInit();
    void Run();

	void LoadAdmin();
	void UnLoadAdmin();
    bool LoadModule(std::string);
    bool UnLoadModule(std::string);
    bool UnLoadModuleId(unsigned int);

    friend class SslAdmin;

private:
    void IrcRun();
    void AdminRun();
    void ModuleRun(int i);
    void TimerRun();

	//classes
    IrcSocket *mpIrcSocket;

    //modules
    //admin module
    AdminInterface* ai;
    void* admin;
    create_tai* create_admin;
    destroy_tai* destroy_admin;

	//other modules
    std::vector< std::string > modulelist;
    std::vector< void* > modulevector;
    std::vector< ModuleInterface* > moduleinterfacevector;
    std::vector< create_tmi* > createvector;
    std::vector< destroy_tmi* > destroyvector;

    //functions
    void AdminCommands(std::string command, std::vector< std::string > args);
    int convertString(std::string data);

    //config vars
    std::string moduledir;
    bool timeron;

    //thread vars
    boost::shared_ptr< boost::thread > timer_thread;
    boost::shared_ptr< boost::thread > admin_thread;
    std::vector< boost::shared_ptr< boost::thread > > module_thread_vector;
};


#endif // Bot_h
