#ifndef IrcSocketInterface_h
#define IrcSocketInterface_h

#include <string>

class IrcSocketInterface
{
 public:
    // Todo: add exception class for every excepion type
    class ExceptionInterface
    {
     public:
        virtual const std::string& Description()=0;
        virtual int Errornr()=0;
    };

 public:

    virtual void Connect(std::string address, std::string service)=0;
    virtual void Disconnect()=0;

    virtual void Send(const std::string data)=0;
    virtual void Recv(std::string &data)=0;

    virtual bool  IsConnected()=0;
    virtual const std::string& Hostname()=0;
    virtual const std::string& Service()=0;
};

#endif // IrcSocketInterface_h

