#ifndef Authserv_h
#define Authserv_h

#include "UserManagement.h"
#include <string>
#include <vector>


using namespace std;

class Authserv : public UserManagement {

    public:

                void ParseData(vector<string> data);

    private:

                void auth(vector<string> data);
};

#endif // Authserv_h

