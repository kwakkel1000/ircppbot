#ifndef Data_H
#define Data_H
#include "../interfaces/DataInterface.h"

#include <queue>

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>


class Data : public DataInterface
{
public:
    //functions
    Data();
    ~Data();
    void Init(bool getRaw, bool getMode, bool getWhois, bool getPrivmsg);
    void stop();
    void AddRawQueue(std::vector< std::string > data);
    void AddModeQueue(std::vector< std::string > data);
    void AddWhoisQueue(std::vector< std::string > data);
    void AddPrivmsgQueue(std::vector< std::string > data);

    std::vector< std::string > GetRawQueue();
    std::vector< std::string > GetModeQueue();
    std::vector< std::string > GetWhoisQueue();
    std::vector< std::string > GetPrivmsgQueue();

    bool GetRaw();
    bool GetMode();
    bool GetWhois();
    bool GetPrivmsg();

private:
    //vars
    bool mRun;
    bool mGetRaw;
    bool mGetMode;
    bool mGetWhois;
    bool mGetPrivmsg;

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
