#ifndef UserManagementInterface_H
#define UserManagementInterface_H

#include <vector>
#include <string>
#include "ConfigReader.h"
#include "IrcData.h"

using namespace std;

class IrcSocket;
class Users;
class Channels;

class UserManagementInterface
{
public:
    UserManagementInterface() { }

    virtual void ParseData(vector<string> data)=0;
    virtual void Init()=0;
    virtual void read()=0;
    virtual void stop()=0;

};

typedef UserManagementInterface* create_tUMI();

typedef void destroy_tUMI(UserManagementInterface*);

#endif // UserManagementInterface_H


