#include "../include/Data.h"
#include <iostream>
Data::Data()
{
}

Data::~Data()
{
}

void Data::Init(bool getraw, bool getmode, bool getwhois, bool getprivmsg)
{
    get_raw = getraw;
    get_mode = getmode;
    get_whois = getwhois;
    get_privmsg = getprivmsg;
}

//producers
void Data::AddRawQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(RawMutex);
    RawQueue.push(data);
    RawAvailable.notify_one();
}

void Data::AddModeQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(ModeMutex);
    ModeQueue.push(data);
    ModeAvailable.notify_one();
}

void Data::AddWhoisQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    WhoisQueue.push(data);
    WhoisAvailable.notify_one();
}

void Data::AddPrivmsgQueue(std::vector<std::string> data)
{
    boost::mutex::scoped_lock lock(PrivmsgMutex);
    PrivmsgQueue.push(data);
    PrivmsgAvailable.notify_one();
}

//consumers
std::vector< std::string > Data::GetRawQueue()
{
    boost::mutex::scoped_lock lock(RawMutex);
    //std::cout << "GetRawQueue before lock " << std::endl;
    while(RawQueue.empty())
    {
        RawAvailable.wait(lock);
    }
    std::vector< std::string > temp = RawQueue.front();
    RawQueue.pop();
    //std::cout << "GetRawQueue: " << temp[0] << std::endl;
    return temp;
}

std::vector< std::string > Data::GetModeQueue()
{
    boost::mutex::scoped_lock lock(ModeMutex);
    while(ModeQueue.empty())
    {
        ModeAvailable.wait(lock);
    }
    std::vector< std::string > temp = ModeQueue.front();
    ModeQueue.pop();
    return temp;
}

std::vector< std::string > Data::GetWhoisQueue()
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    while(WhoisQueue.empty())
    {
        WhoisAvailable.wait(lock);
    }
    std::vector< std::string > temp = WhoisQueue.front();
    WhoisQueue.pop();
    return temp;
}

std::vector< std::string > Data::GetPrivmsgQueue()
{
    boost::mutex::scoped_lock lock(PrivmsgMutex);
    while(PrivmsgQueue.empty())
    {
        PrivmsgAvailable.wait(lock);
    }
    std::vector< std::string > temp = PrivmsgQueue.front();
    PrivmsgQueue.pop();
    return temp;
}
