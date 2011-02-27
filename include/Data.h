#ifndef Tran_Data_H
#define Tran_Data_H

#include <vector>
#include <queue>
#include <string>

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>


class Data
{
public:
    //functions
    Data();
    ~Data();
    void Init(bool getraw, bool getmode, bool getwhois, bool getprivmsg);
    void AddRawQueue(std::vector< std::string > data);
    void AddModeQueue(std::vector< std::string > data);
    void AddWhoisQueue(std::vector< std::string > data);
    void AddPrivmsgQueue(std::vector< std::string > data);

    std::vector< std::string > GetRawQueue();
    std::vector< std::string > GetModeQueue();
    std::vector< std::string > GetWhoisQueue();
    std::vector< std::string > GetPrivmsgQueue();

    //vars
    bool get_raw;
    bool get_mode;
    bool get_whois;
    bool get_privmsg;

private:
    //vars
    //dummy vars
    std::vector< std::string > NULLvector;

    //irc data queues
    std::queue< std::vector< std::string > > RawQueue;
    std::queue< std::vector< std::string > > ModeQueue;
    std::queue< std::vector< std::string > > WhoisQueue;
    std::queue< std::vector< std::string > > PrivmsgQueue;

    //threadvars
    boost::condition RawAvailable;
    boost::condition ModeAvailable;
    boost::condition WhoisAvailable;
    boost::condition PrivmsgAvailable;

    boost::mutex RawMutex;
    boost::mutex ModeMutex;
    boost::mutex WhoisMutex;
    boost::mutex PrivmsgMutex;
};

#endif
