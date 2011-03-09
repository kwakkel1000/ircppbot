#ifndef Tran_IrcData_H
#define Tran_IrcData_H

#include "IrcSocket.h"
#include "Data.h"

#include <queue>
#include <vector>
#include <string>

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

class IrcSocket;
class Data;
class IrcData
{
public:
    unsigned int buffersize;

    IrcData();
    ~IrcData();
    void init(IrcSocket *s);
    void run();
    void stop();

    //consumer
    void AddConsumer(Data *d);
    void DelConsumer(Data *d);

    //data
    void AddSendQueue(std::string data);
    void AddHighPrioritySendQueue(std::string data);
    void AddLowPrioritySendQueue(std::string data);

private:
    //vars
    IrcSocket *S;

    //floodvars
    bool floodprotect;
    int floodbuffer;
    double floodtime;
    int buffer;

    //consumer lists
    std::vector< Data * > Consumers;
    std::vector< Data * > RawConsumers;
    std::vector< Data * > ModeConsumers;
    std::vector< Data * > WhoisConsumers;
    std::vector< Data * > PrivmsgConsumers;

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

#endif

