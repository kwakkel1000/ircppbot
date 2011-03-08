#ifndef Nickserv_h
#define Nickserv_h

#include "UserManagement.h"
#include <string>
#include <vector>

class Nickserv : public UserManagement
{
public:
    //Authserv();

    void ParseData(std::vector< std::string > data);
    void read();
    void stop();

private:
    void parse_raw();
    void auth(std::vector< std::string > data);

    bool run;
    boost::shared_ptr<boost::thread> raw_parse_thread;
};

#endif // Nickserv_h


