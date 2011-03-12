#ifndef IrcData_H
#define IrcData_H

#include "../interfaces/IrcDataInterface.h"
#include "../interfaces/IrcSocketInterface.h"
#include "../interfaces/DataInterface.h"

#include <queue>
#include <vector>
#include <string>

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

class IrcSocketInterface;
class DataInterface;
class IrcData : public IrcDataInterface
{
public:
    IrcData();
    ~IrcData();
    void init(IrcSocketInterface *s);
    void run();
    void stop();

    //consumer
    void AddConsumer(DataInterface *d);
    void DelConsumer(DataInterface *d);

    //data
    void AddSendQueue(std::string data);
    void AddHighPrioritySendQueue(std::string data);
    void AddLowPrioritySendQueue(std::string data);

private:
    //vars
    unsigned int buffersize;
    IrcSocketInterface *S;

    //floodvars
    bool floodprotect;
    int floodbuffer;
    double floodtime;
    int buffer;

    //consumer lists
    std::vector< DataInterface * > Consumers;
    std::vector< DataInterface * > RawConsumers;
    std::vector< DataInterface * > ModeConsumers;
    std::vector< DataInterface * > WhoisConsumers;
    std::vector< DataInterface * > PrivmsgConsumers;

    //irc thread bools
    bool send;
    bool recv;
    bool parse;

    //threadvars
    boost::shared_ptr<boost::thread> send_thread;
    boost::shared_ptr<boost::thread> recv_thread;
    boost::shared_ptr<boost::thread> parse_thread;
    boost::shared_ptr<boost::thread> flood_thread;

    boost::condition SendAvailable;
    boost::condition RecvAvailable;
    boost::condition floodcondition;

    boost::mutex SendMutex;
    boost::mutex RecvMutex;
    boost::mutex floodmutex;

    //irc queues
    std::queue< std::string > HighPrioritySendQueue;
    std::queue< std::string > LowPrioritySendQueue;
    std::queue< std::string > SendQueue;
    std::queue< std::string > RecvQueue;

    //functions
    //thread functions
    void sendloop();
    void recvloop();
    void Parse();
    void flood_timer();

    //irc functions
    void Send();
    void Recv();

    //queue functions
    std::string GetSendQueue();
    std::string GetRecvQueue();
    void AddRecvQueue(std::string data);

};

#endif // IrcData_h
