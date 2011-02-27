#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Socket.h"
#include "Users.h"
#include "Channels.h"
#include <vector>
#include <string>

using namespace std;

class UsersInterface
{
public:
	UsersInterface() { }

	virtual void ParseData(vector<string> data)=0;

//private:
	vector<string> nick;
	vector<User*> u;
	vector<string> whoislist;
	vector<string> NULLvector;
	vector<string> authlist;

};

typedef ChannelBotInterface* create_t();
typedef void destroy_t(ChannelBotInterface*);

#endif // USERINTERFACE_H
