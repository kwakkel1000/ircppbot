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

    virtual bool AddChannel(std::string);
    virtual bool DelChannel(std::string);

    virtual bool AddNick(std::string, std::string);
    virtual bool DelNick(std::string, std::string);

    virtual bool AddAuth(std::string, std::string);
    virtual bool DelAuth(std::string, std::string);

    virtual bool SetAccess(std::string, std::string, int);
    virtual int GetAccess(std::string, std::string);

    virtual bool SetGiveops(std::string, int);
    virtual int GetGiveops(std::string);

    virtual bool SetGivevoice(std::string, int);
    virtual int GetGivevoice(std::string);

    virtual std::string GetCid(std::string);
    virtual bool SetCid(std::string, std::string);

    virtual bool GetOp(std::string, std::string);
    virtual bool SetOp(std::string, std::string, bool);

    virtual bool GetVoice(std::string, std::string);
    virtual bool SetVoice(std::string, std::string, bool);

    virtual std::vector< std::string > GetChannels();
    virtual std::vector< std::string > GetNicks(std::string sChannel);
    virtual std::vector< std::string > GetAuths(std::string sChannel);
    virtual void Debug();


private:

    std::vector< std::string > channellist;
    std::vector< Channel* > c;
    unsigned int GetChannelIndex(std::string sChannel);
};

#endif // Channels_h
