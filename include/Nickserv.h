#ifndef Nickserv_h
#define Nickserv_h

#include "UserManagement.h"
#include <string>
#include <vector>

using namespace std;

class Nickserv : public UserManagement
{
public:
    //Authserv();

    void ParseData(vector<string> data);

private:
    void auth(vector<string> data);
};

#endif // Nickserv_h


