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
    virtual void Init(string nick, IrcSocket *s, Users *u, Channels *c, ConfigReader& reader, IrcData *id)=0;

protected:
    IrcData *ID;
    IrcSocket *S;
    Users *U;
    Channels *C;

    string botnick;
    string hostname_str;
    string databasename_str;
    string username_str;
    string pass_str;
    string trigger;

};

typedef UserManagementInterface* create_tUMI();

typedef void destroy_tUMI(UserManagementInterface*);

#endif // UserManagementInterface_H


