#ifndef AppelBot_IrcSocket_h
#define AppelBot_IrcSocket_h

#include <string>
#include <boost/thread/mutex.hpp>


class IrcSocket
{
 public:
    // Todo: add exception class for every excepion type
    class Exception
    {
     public:
        Exception( std::string description, int errornr=0 ) { m_description = description; m_errornr = errornr; }
        ~Exception() {}

        const std::string& Description() { return m_description; }
        int Errornr() { return m_errornr; }

     private:
        std::string m_description;
        int m_errornr;
    };

 public:
    IrcSocket();
    virtual ~IrcSocket();

    virtual void Connect(std::string address, std::string service);
    virtual void Disconnect();

    virtual void Send(const std::string data);
    virtual void Recv(std::string &data);

    bool  IsConnected()      { return m_socket;   }
    const std::string& Hostname() { return m_hostname; }
    const std::string& Service()  { return m_service;  }

 private:

    std::string    m_hostname;
    std::string    m_service;
    int       m_socket; // socket handle

    boost::mutex io_mutex;
};

#endif // AppelBot_IrcSocket_h
