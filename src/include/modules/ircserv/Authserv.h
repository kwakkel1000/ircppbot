#ifndef Authserv_h
#define Authserv_h

#include "UserManagement.h"
#include <string>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

class Authserv : public UserManagement {

public:

    void ParseData(std::vector< std::string > data);
    void read();
    void stop();

private:
    void parse_raw();
    void auth(std::vector< std::string > data);

    bool run;
    boost::shared_ptr<boost::thread> raw_parse_thread;
};

#endif // Authserv_h

