#ifndef Channel_h
#define Channel_h

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ChannelAuth.h"

#include <boost/thread/mutex.hpp>

using namespace std;

class Channel
{
public:
	Channel();
	virtual ~Channel();

	virtual bool AddNick(string);
	virtual bool DelNick(string);
	virtual bool AddAuth(string);
	virtual bool DelAuth(string);
	virtual bool SetAccess(string, int);
	virtual int GetAccess(string);
	virtual bool SetOp(string, bool);
	virtual bool GetOp(string);
	virtual bool SetVoice(string, bool);
	virtual bool GetVoice(string);

	virtual bool SetGiveops(int);
	virtual int GetGiveops();
	virtual bool SetGivevoice(int);
	virtual int GetGivevoice();
	virtual std::string GetCid();
	virtual bool SetCid(std::string);

	virtual vector<string> GetNicks();
	virtual vector<string> GetAuths();

private:
	vector<string> channelauthlist;
	vector<ChannelAuth*> ca;
	vector<string> nicks;
	vector<bool> op;
	vector<bool> voice;
	int giveops;
	int givevoice;
	std::string cid;
	int GetChannelAuthIndex(string data);
	int GetNicksIndex(string data);


	boost::mutex Channel_mutex;
    /*boost::mutex User_mutex;
    boost::mutex UsersGetNicks_mutex;
    boost::mutex UsersGetAuths_mutex;
    boost::mutex UsersGetChannels_mutex;
    boost::mutex UsersWhois_mutex;*/
};


#endif // Channel_h
