#ifndef Bot_h
#define Bot_h

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include "../interfaces/AdminInterface.h"

// Predefinitions
class IrcSocket;
class Parse;
class AdminInterface;
class Bot
{
public:
    Bot();
    ~Bot();

    void Init (std::string);
    void parseinit();
    void admininit();
    void parserun();
    void adminrun();
    void Run();

private:
	virtual void LoadAdmin();
	virtual void UnLoadAdmin();
    AdminInterface* ai;
    void* admin;
    create_tai* create_admin;
    destroy_tai* destroy_admin;


    IrcSocket *parse_sock;
    Parse *P;

    bool read;

    boost::shared_ptr<boost::thread> parse_thread;
    boost::shared_ptr<boost::thread> admin_thread;
};


#endif // Bot_h
