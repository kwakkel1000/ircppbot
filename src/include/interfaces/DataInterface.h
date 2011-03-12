#ifndef DataInterface_H
#define DataInterface_H

#include <vector>
#include <string>

class DataInterface
{
public:

    virtual void Init(bool getraw, bool getmode, bool getwhois, bool getprivmsg)=0;
    virtual void stop()=0;
    virtual void AddRawQueue(std::vector< std::string > data)=0;
    virtual void AddModeQueue(std::vector< std::string > data)=0;
    virtual void AddWhoisQueue(std::vector< std::string > data)=0;
    virtual void AddPrivmsgQueue(std::vector< std::string > data)=0;

    virtual std::vector< std::string > GetRawQueue()=0;
    virtual std::vector< std::string > GetModeQueue()=0;
    virtual std::vector< std::string > GetWhoisQueue()=0;
    virtual std::vector< std::string > GetPrivmsgQueue()=0;

    virtual bool GetRaw()=0;
    virtual bool GetMode()=0;
    virtual bool GetWhois()=0;
    virtual bool GetPrivmsg()=0;
};

#endif // DataInterface_h
