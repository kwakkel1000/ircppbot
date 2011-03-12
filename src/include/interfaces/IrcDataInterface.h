#ifndef IrcDataInterface_H
#define IrcDataInterface_H

#include "IrcSocketInterface.h"
#include "DataInterface.h"

#include <string>

class IrcSocketInterface;
class DataInterface;
class IrcDataInterface
{
public:
    virtual void init(IrcSocketInterface *s)=0;
    virtual void run()=0;
    virtual void stop()=0;

    //consumer
    virtual void AddConsumer(DataInterface *d)=0;
    virtual void DelConsumer(DataInterface *d)=0;

    //data
    virtual void AddSendQueue(std::string data)=0;
    virtual void AddHighPrioritySendQueue(std::string data)=0;
    virtual void AddLowPrioritySendQueue(std::string data)=0;
};

#endif // IrcDataInterface_H


