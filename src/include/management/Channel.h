#ifndef AppelBot_Channel_h
#define AppelBot_Channel_h

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "ChannelAuth.h"

using namespace std;

//namespace Bot {

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
	virtual int GetCid();
	virtual bool SetCid(int);
	
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
	int cid;
	int GetChannelAuthIndex(string data);
	int GetNicksIndex(string data);
	bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
};

//} /* End of namespace Bot */

#endif // AppelBot_Channel_h
