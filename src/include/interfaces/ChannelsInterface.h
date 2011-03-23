#ifndef ChannelsInterface_h
#define ChannelsInterface_h

#include <string>
#include <vector>

class ChannelsInterface {

public:

	virtual void RegistrateChannel(std::string mChannelUuid, std::string mChannel)=0;
	virtual void UnregistrateChannel(std::string mChannelUuid)=0;

	virtual void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess)=0;
	virtual void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid)=0;

	virtual bool AddChannel(std::string)=0;
	virtual bool DelChannel(std::string)=0;

	virtual bool AddNick(std::string, std::string)=0;
	virtual bool DelNick(std::string, std::string)=0;

	virtual bool AddAuth(std::string, std::string)=0;
	virtual bool DelAuth(std::string, std::string)=0;

	virtual bool SetAccess(std::string, std::string, int)=0;
	virtual int GetAccess(std::string, std::string)=0;

	virtual bool SetGiveops(std::string, int)=0;
	virtual int GetGiveops(std::string)=0;

	virtual bool SetGivevoice(std::string, int)=0;
	virtual int GetGivevoice(std::string)=0;

	virtual std::string GetCid(std::string)=0;
	virtual bool SetCid(std::string, std::string)=0;

	virtual bool GetOp(std::string, std::string)=0;
	virtual bool SetOp(std::string, std::string, bool)=0;

	virtual bool GetVoice(std::string, std::string)=0;
	virtual bool SetVoice(std::string, std::string, bool)=0;

	virtual std::vector< std::string > GetChannels()=0;
	virtual std::vector< std::string > GetNicks(std::string)=0;
	virtual std::vector< std::string > GetAuths(std::string)=0;
	virtual void Debug()=0;
};

#endif // ChannelsInterface_h

