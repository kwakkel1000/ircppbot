#ifndef Channels_h
#define Channels_h
#include "../interfaces/ChannelsInterface.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Channel.h"

using namespace std;


class Channels : public ChannelsInterface
{

public:

	Channels();
	virtual ~Channels();

	void RegistrateChannel(std::string mChannelUuid, std::string mChannel);
	void UnregistrateChannel(std::string mChannelUuid);

	void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess);
	void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid);

	virtual bool AddChannel(string);
	virtual bool DelChannel(string);

	virtual bool AddNick(string, string);
	virtual bool DelNick(string, string);

	virtual bool AddAuth(string, string);
	virtual bool DelAuth(string, string);

	virtual bool SetAccess(string, string, int);
	virtual int GetAccess(string, string);

	virtual bool SetGiveops(string, int);
	virtual int GetGiveops(string);

	virtual bool SetGivevoice(string, int);
	virtual int GetGivevoice(string);

	virtual std::string GetCid(std::string);
	virtual bool SetCid(std::string, std::string);

	virtual bool GetOp(string, string);
	virtual bool SetOp(string, string, bool);

	virtual bool GetVoice(string, string);
	virtual bool SetVoice(string, string, bool);

	virtual vector<string> GetChannels();
	virtual vector<string> GetNicks(string);
	virtual vector<string> GetAuths(string);
	virtual void Debug();


private:

	vector<string> channellist;
	vector<Channel*> c;
	unsigned int GetChannelIndex(string data);
};

#endif // Channels_h
