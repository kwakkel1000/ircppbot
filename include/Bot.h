#ifndef Bot_h
#define Bot_h

#include "AdminInterface.h"
#include <string>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;

// Predefinitions
class ServerSocket;
class IrcSocket;
class Parse;
class ConfigReader;
class IrcData;
class ScopedReadLock;
class ScopedWriteLock;

class Bot
{
public:
    Bot();
    virtual ~Bot();

    virtual void Init (string);
    virtual void parseinit();
    virtual void admininit();
    virtual void parserun();
    virtual void adminrun();
    virtual void Run();

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
