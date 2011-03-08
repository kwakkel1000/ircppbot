#include "../include/IrcData.h"

#include <iostream>
#include <boost/algorithm/string.hpp>

IrcData::IrcData()
{
    send = true;
    recv = true;
    parse = true;
}

IrcData::~IrcData()
{
}

void IrcData::stop()
{
    send = false;
    recv = false;
    parse = false;
    SendAvailable.notify_all();
    RecvAvailable.notify_all();
    send_thread->join();
    recv_thread->join();
    parse_thread->join();
}

void IrcData::init(IrcSocket *s)
{
    S=s;
}

void IrcData::AddConsumer(Data *d)
{
    Consumers.push_back(d);
    if (d->get_raw == true)
    {
        RawConsumers.push_back(d);
    }
    if (d->get_mode == true)
    {
        ModeConsumers.push_back(d);
    }
    if (d->get_whois == true)
    {
        WhoisConsumers.push_back(d);
    }
    if (d->get_privmsg == true)
    {
        PrivmsgConsumers.push_back(d);
    }
}

void IrcData::DelConsumer(Data *d)
{
    unsigned int consumer_iterator;
    for (consumer_iterator = (Consumers.size() - 1); consumer_iterator >= 0; consumer_iterator--)
    {
        if (Consumers[consumer_iterator] == d)
        {
            Consumers.erase(Consumers.begin() + consumer_iterator);
        }
    }
    for (consumer_iterator = (RawConsumers.size() - 1); consumer_iterator >= 0; consumer_iterator--)
    {
        if (RawConsumers[consumer_iterator] == d)
        {
            RawConsumers.erase(RawConsumers.begin() + consumer_iterator);
        }
    }
    for (consumer_iterator = (ModeConsumers.size() - 1); consumer_iterator >= 0; consumer_iterator--)
    {
        if (ModeConsumers[consumer_iterator] == d)
        {
            ModeConsumers.erase(ModeConsumers.begin() + consumer_iterator);
        }
    }
    for (consumer_iterator = (WhoisConsumers.size() - 1); consumer_iterator >= 0; consumer_iterator--)
    {
        if (WhoisConsumers[consumer_iterator] == d)
        {
            WhoisConsumers.erase(WhoisConsumers.begin() + consumer_iterator);
        }
    }
    for (consumer_iterator = (PrivmsgConsumers.size() - 1); consumer_iterator >= 0; consumer_iterator--)
    {
        if (PrivmsgConsumers[consumer_iterator] == d)
        {
            PrivmsgConsumers.erase(PrivmsgConsumers.begin() + consumer_iterator);
        }
    }
}

void IrcData::run()
{
    assert(!send_thread);
    send_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::sendloop, this)));
    assert(!recv_thread);
    recv_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::recvloop, this)));
    assert(!parse_thread);
    parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&IrcData::Parse, this)));
}

void IrcData::AddSendQueue(std::string data)
{
    boost::mutex::scoped_lock lock(SendMutex);
    SendQueue.push(data);
    SendAvailable.notify_one();
}

//private
void IrcData::sendloop()
{
    while(send==true)
    {
        Send();
    }
}

void IrcData::recvloop()
{
    while(recv==true)
    {
        Recv();
    }
}

void IrcData::Send()
{
    if (send == true)
    {
        std::string data;
        data = GetSendQueue();
        std::cout << "IrcData::Send >> " << data;
        try
        {
            if(S)
            {
                S->Send(data);
            }
        }
        catch (IrcSocket::Exception& e)
        {
            //cout << "Exception caught: " << e.Description() << endl;
        }
    }
}

void IrcData::Recv()
{
    if (recv == true)
    {
        std::string buf;
        if (S)
        {
            S->Recv(buf);
            std::cout << "Recv: " << buf << std::endl;
            AddRecvQueue(buf);
        }
    }
}

std::string IrcData::GetSendQueue()
{
    boost::mutex::scoped_lock lock(SendMutex);
    std::string temp = "";
    while(SendQueue.empty() && send == true)
    {
        SendAvailable.wait(lock);
    }
    if (send == true)
    {
        temp = SendQueue.front();
        SendQueue.pop();
    }
    return temp;
}

std::string IrcData::GetRecvQueue()
{
    boost::mutex::scoped_lock lock(RecvMutex);
    std::string temp = "";
    while(RecvQueue.empty() && recv == true)
    {
        RecvAvailable.wait(lock);
    }
    if (recv == true)
    {
        temp = RecvQueue.front();
        RecvQueue.pop();
    }
    return temp;
}

void IrcData::AddRecvQueue(std::string data)
{
    boost::mutex::scoped_lock lock(RecvMutex);
    RecvQueue.push(data);
    RecvAvailable.notify_one();
}

void IrcData::Parse()
{
    while(parse)
    {
        std::string data;
        std::vector< std::string > result;
        data = GetRecvQueue();
        boost::split( result, data, boost::is_any_of(" "), boost::token_compress_on );
        unsigned int consumer_iterator;
        //std::cout << RawConsumers.size() << std::endl;
        std::cout << "void IrcData::Parse()  Raw";
        for (consumer_iterator = 0; consumer_iterator < RawConsumers.size(); consumer_iterator++)
        {
            std::cout << " | ";
            RawConsumers[consumer_iterator]->AddRawQueue(result);
        }
        std::cout << std::endl;
        for (consumer_iterator = 0; consumer_iterator < ModeConsumers.size(); consumer_iterator++)
        {
            //ModeConsumers[consumer_iterator]->AddModeQueue(result);
        }
        for (consumer_iterator = 0; consumer_iterator < WhoisConsumers.size(); consumer_iterator++)
        {
            //WhoisConsumers[consumer_iterator]->AddWhoisQueue(result);
        }
        if (result.size() >= 4)
        {
            if (result[1] == "PRIVMSG")   //PRIVMSG
            {
                std::cout << "void IrcData::Parse()  PRIVMSG";
                for (consumer_iterator = 0; consumer_iterator < PrivmsgConsumers.size(); consumer_iterator++)
                {
                    std::cout << " | ";
                    PrivmsgConsumers[consumer_iterator]->AddPrivmsgQueue(result);
                }
                std::cout << std::endl;
            }
        }
    }
}
