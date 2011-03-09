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
    run = true;
    get_raw = getraw;
    get_mode = getmode;
    get_whois = getwhois;
    get_privmsg = getprivmsg;
}

void Data::stop()
{
    run = false;
    RawAvailable.notify_all();
    ModeAvailable.notify_all();
    WhoisAvailable.notify_all();
    PrivmsgAvailable.notify_all();
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
    while(RawQueue.empty() && run)
    {
        RawAvailable.wait(lock);
    }
    if (!RawQueue.empty())
    {
        std::vector< std::string > temp = RawQueue.front();
        RawQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetModeQueue()
{
    boost::mutex::scoped_lock lock(ModeMutex);
    while(ModeQueue.empty() && run)
    {
        ModeAvailable.wait(lock);
    }
    if (!ModeQueue.empty())
    {
        std::vector< std::string > temp = ModeQueue.front();
        ModeQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetWhoisQueue()
{
    boost::mutex::scoped_lock lock(WhoisMutex);
    while(WhoisQueue.empty() && run)
    {
        WhoisAvailable.wait(lock);
    }
    if (!WhoisQueue.empty())
    {
        std::vector< std::string > temp = WhoisQueue.front();
        WhoisQueue.pop();
        return temp;
    }
    return NULLvector;
}

std::vector< std::string > Data::GetPrivmsgQueue()
{
    boost::mutex::scoped_lock lock(PrivmsgMutex);
    while(PrivmsgQueue.empty() && run)
    {
        PrivmsgAvailable.wait(lock);
    }
    if (!PrivmsgQueue.empty())
    {
        std::vector< std::string > temp = PrivmsgQueue.front();
        PrivmsgQueue.pop();
        return temp;
    }
    std::vector< std::string > temp;
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    temp.push_back("");
    return temp;
}
