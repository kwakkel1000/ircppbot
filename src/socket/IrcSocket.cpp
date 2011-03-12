#include "../include/socket/IrcSocket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#include <string.h> // for memset and bzero

IrcSocket::IrcSocket()
{
    m_socket = 0;
}

IrcSocket::~IrcSocket()
{
    Disconnect();
}

// Note: pass connection data here. not in constructor :+
void IrcSocket::Connect(std::string host, std::string service)
{
    if(m_socket)
    {
        Disconnect();
    }

    struct addrinfo *res, *aip;
    struct addrinfo hints;
    //int sock = -1;
    int error;

    // Get host address. Any type of address will do
    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_ALL|AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_STREAM;

    error = getaddrinfo(host.c_str(), service.c_str(), &hints, &res);
    if (error != 0)
    {
        throw Exception("Could not get address info", errno);
        return;
    }

    // Try all returned addresses until one works
    for (aip = res; aip != NULL; aip = aip->ai_next)
    {
        // Open socket. The address type depends on what getaddrinfo() gave us
        m_socket = socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
        if (m_socket == -1)
        {
            m_socket = 0;
            freeaddrinfo(res);
            throw Exception("Could not ceate socket", errno);
            return;
        }

        // Connect to the host
        if (connect(m_socket, aip->ai_addr, aip->ai_addrlen) == -1)
        {
            close(m_socket);
            m_socket = 0;
        }
        else
        {
            // Store connectiond data
            /*
            cout << "Connected to " << aip->ai_canonname << "! flags=" << aip->ai_flags
                << " family=" << aip->ai_family << " socktype=" << aip->ai_socktype
                << " protocol=" << aip->ai_protocol << endl;
            */
            /*
            struct addrinfo {
                int              ai_flags;
                int              ai_family;
                int              ai_socktype;
                int              ai_protocol;
                size_t           ai_addrlen;
                struct sockaddr *ai_addr;
                char            *ai_canonname;
                struct addrinfo *ai_next;
            };
            m_hostname = phost->h_name;
            m_port = port;
            */

            break; // break out of for loop, because we managed to connect!
        }
    }

    // Clean up :+
    freeaddrinfo(res);

    if (!m_socket)
    {
        throw Exception("Could not connect to host");
    }
}

void IrcSocket::Disconnect()
{
    if(m_socket)
    {
        close(m_socket);
        m_socket = 0;
    }
}

void IrcSocket::Send(const std::string data)
{
    boost::mutex::scoped_lock  lock(io_mutex);
    if (m_socket)
    {
        if (!send(m_socket, data.c_str(), data.size(), MSG_NOSIGNAL))
        {
            Disconnect();
            throw Exception("Could not send data");
        }
    }
}

void IrcSocket::Recv(std::string& data)
{
    static const int buffersize( 64 );
    static char buffer[buffersize];
    static int length;
    memset( buffer, '\0', buffersize );
    length = 0;
    data = "";

    char c = '\0';
    while((c != '\n') && (length < buffersize))
    {
        if (!recv(m_socket, &c, sizeof(char), 0))
        {
            Disconnect();
            throw Exception("Connection lost", errno);
        }

        if (c != '\r' && c != '\n') // This will not work when sending non-ascii data
        {
            buffer[length] = c;
            length++;

            // In the very weird case the buffer does not have enough space...
            if (length == buffersize-1)
            {
                data += std::string(buffer);
                memset( buffer, '\0', buffersize );
                length = 0;
            }
        }
    }

    if (length > 0)
    {
        data += std::string(buffer);
    }
}
