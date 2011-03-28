#include "include/UserManagement.h"

#include <interfaces/UsersInterface.h>
#include <interfaces/ChannelsInterface.h>
#include <interfaces/ConfigReaderInterface.h>
#include <core/DatabaseData.h>
#include <core/Global.h>
#include <core/Data.h>

#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>


const char addchar    = '+';
const char remchar    = '-';

const char gonechar   = 'G';
const char herechar   = 'H';
const char operchar   = '*';
const char botchar    = 'd';

const char ownerchar  = 'q';
const char adminchar  = 'a';
const char opchar     = 'o';
const char halfopchar = 'h';
const char voicechar  = 'v';

const char ownerwhochar  = '~';
const char adminwhochar  = '&';
const char opwhochar     = '@';
const char halfopwhochar = '%';
const char voicewhochar  = '+';

extern "C" UserManagement* create() {
    return new UserManagement;
}

extern "C" void destroy(UserManagement* x) {
    delete x;
}

UserManagement::UserManagement()
{
}

UserManagement::~UserManagement()
{
    stop();
	Global::Instance().get_IrcData().DelConsumer(mpDataInterface);
    delete mpDataInterface;
}

void UserManagement::stop()
{
	Run = false;
    mpDataInterface->stop();
    std::cout << "UserManagement::stop" << std::endl;
    raw_parse_thread->join();
    std::cout << "raw_parse_thread stopped" << std::endl;
}

void UserManagement::Init(DataInterface* pData)
{
	GetAuths();
	mpDataInterface = pData;
	mpDataInterface->Init(true, false, false, false);
    Global::Instance().get_IrcData().AddConsumer(mpDataInterface);
    NickServ = (Global::Instance().get_ConfigReader().GetString("nickserv") == "true");
    WhoExtra = (Global::Instance().get_ConfigReader().GetString("whoextra") == "true");
}

void UserManagement::read()
{
    Run = true;
    assert(!raw_parse_thread);
    raw_parse_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&UserManagement::parse_raw, this)));
}

void UserManagement::parse_raw()
{
    std::vector< std::string > data;
    while(Run)
    {
        data = mpDataInterface->GetRawQueue();
        ParseData(data);
    }
}

void UserManagement::ParseData(std::vector< std::string > data)
{
    if (data.size() == 3)
    {
        if (data[1] == "JOIN")      //JOIN
        {
            JOIN(data);
        }
        if (data[1] == "NICK")      //NICK nickchange
        {
            NICK(data);
        }
    }
    if (data.size() >= 3)
    {
        if (data[1] == "QUIT")      //QUIT
        {
            QUIT(data);
        }
        if (data[1] == "PART")      //PART
        {
            PART(data);
        }
    }
    if (data.size() >= 4)
    {
        if (data[1] == "001")   //welcome
        {
            Global::Instance().set_BotNick(data[2]);
        }
        if (data[1] == "307")       //WHOIS regged nick
        {
        	if (NickServ)
        	{
				UserAuth(data[3], data[3]);
        	}
        }
        if (data[1] == "318")       //WHOIS end
        {
            //WHOIS(data);
        }
        if (data[1] == "330")       //WHOIS auth
        {
        	if (!NickServ)
        	{
				UserAuth(data[3], data[4]);
        	}
        }
        if (data[1] == "402")       //WHOIS no server
        {
            //WHOIS(data);
        }
        if (data[1] == "354")       //WHO (extra)
        {
        	if (WhoExtra)
        	{
				WHOEXTRA(data);
        	}
        }
    }
    if (data.size() >= 5)
    {
        if (data[1] == "KICK")      //KICK
        {
            KICK(data);
        }
        if (data[1] == "MODE")      //KICK
        {
            MODE(data);
        }
    }
    if (data.size() >= 11)
    {
        if (data[1] == "352")       //WHO
        {
            WHO(data);
        }
    }
}

void UserManagement::WHO(std::vector< std::string > data)
{
    std::string chan = data[3];
    std::string nick = data[7];
    std::string modes = data[8];

    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    C.AddNick(chan, nick);
    bool added = U.AddUser(nick);
    U.AddChannel(nick, chan);

	size_t Gonepos = modes.find(gonechar);
    if (Gonepos != std::string::npos)
    {
        U.SetGone(nick, true);
    }

    size_t Herepos = modes.find(herechar);
	if (Herepos != std::string::npos)
    {
        U.SetGone(nick, false);
    }

    size_t Ownerpos = modes.find(ownerwhochar);
	if (Ownerpos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Adminpos = modes.find(adminwhochar);
	if (Adminpos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Oppos = modes.find(opwhochar);
	if (Oppos != std::string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Halfoppos = modes.find(halfopwhochar);
	if (Halfoppos != std::string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Voicepos = modes.find(voicewhochar);
	if (Voicepos != std::string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Xpos = modes.find("x");
	if (Xpos != std::string::npos)
    {
        U.SetX(nick, true);
    }

    size_t Dpos = modes.find(botchar);
	if (Dpos != std::string::npos)
    {
        U.SetD(nick, true);
    }

    size_t Ircoppos = modes.find(operchar);
	if (Ircoppos != std::string::npos)
    {
        U.SetIrcop(nick, true);
    }

	if (added)
    {
        std::string whoisstring = "WHOIS " + nick + " " + nick + "\r\n";
        Send(whoisstring);
    }
}

void UserManagement::WHOEXTRA(std::vector< std::string > data)
{
	if (data.size() == 7)
	{
		std::string chan = data[3];
		std::string nick = data[4];
		std::string modes = data[5];
		std::string auth = data[6];
		size_t chanpos;
		chanpos = chan.find("#");
		if (chanpos != std::string::npos)
		{
			UsersInterface& U = Global::Instance().get_Users();
			ChannelsInterface& C = Global::Instance().get_Channels();
			C.AddNick(chan, nick);
			U.AddUser(nick);
			U.AddChannel(nick, chan);
			UserAuth(nick, auth);

			size_t Gonepos = modes.find(gonechar);
			if (Gonepos != std::string::npos)
			{
				U.SetGone(nick, true);
			}

			size_t Herepos = modes.find(herechar);
			if (Herepos != std::string::npos)
			{
				U.SetGone(nick, false);
			}

			size_t Ownerpos = modes.find(ownerwhochar);
			if (Ownerpos != std::string::npos)
			{
				C.SetOp(chan, nick, true);
			}

			size_t Adminpos = modes.find(adminwhochar);
			if (Adminpos != std::string::npos)
			{
				C.SetOp(chan, nick, true);
			}

			size_t Oppos = modes.find(opwhochar);
			if (Oppos != std::string::npos)
			{
				C.SetOp(chan, nick, true);
			}

			size_t Halfoppos = modes.find(halfopwhochar);
			if (Halfoppos != std::string::npos)
			{
				C.SetVoice(chan, nick, true);
			}

			size_t Voicepos = modes.find(voicewhochar);
			if (Voicepos != std::string::npos)
			{
				C.SetVoice(chan, nick, true);
			}

			size_t Xpos = modes.find("x");
			if (Xpos != std::string::npos)
			{
				U.SetX(nick, true);
			}

			size_t Dpos = modes.find(botchar);
			if (Dpos != std::string::npos)
			{
				U.SetD(nick, true);
			}

			size_t Ircoppos = modes.find(operchar);
			if (Ircoppos != std::string::npos)
			{
				U.SetIrcop(nick, true);
			}
		}
	}
}

void UserManagement::JOIN(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string chan = data[2];
    boost::erase_all(chan, ":");
    //vector<string> chan = Split(data[2], ":",true,true);
    std::string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        C.AddChannel(chan);
        std::string whostring;
		if (WhoExtra)
		{
			 whostring = "WHO " + chan + " %ncaf\r\n";
		}
		else
		{
			whostring = "WHO " + chan + "\r\n";
		}
        Send(whostring);
        GetChannelInfo(chan);
    }
    else
    {
        C.AddNick(chan, nick);
        bool added = false;
        added = U.AddUser(nick);
        U.AddChannel(nick, chan);
        if (added)
        {
            std::string whoisstring = "WHOIS " + nick + " " + nick + "\r\n";
            Send(whoisstring);
        }
    }
}

void UserManagement::PART(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string chan = data[2];
    boost::erase_all(chan, ":");
    //vector<string> chan = Split(data[2], ":",true,true);
    std::string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        std::vector< std::string > chanusers = C.GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            U.DelChannel(chanusers[i], chan);
            C.DelNick(chan, chanusers[i]);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                std::cout << "no channels left " << chanusers[i] << std::endl;
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan);
    }
    else
    {
        C.DelNick(chan, nick);
        U.DelChannel(nick, chan);
        if (U.GetChannels(nick)[0] == "NULL")
        {
            std::cout << "no channels left" << std::endl;
            U.DelUser(nick);
        }
        //cout << "PART" << endl;
    }
}

void UserManagement::KICK(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string chan = data[2];
    std::string nick = data[3];
    if (nick == Global::Instance().get_BotNick())
    {
        std::vector< std::string > chanusers = C.GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            C.DelNick(chan, chanusers[i]);
            U.DelChannel(chanusers[i], chan);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                std::cout << "no channels left" << std::endl;
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan);
    }
    else
    {
        C.DelNick(chan, nick);
        U.DelChannel(nick, chan);
        if (U.GetChannels(nick)[0] == "NULL")
        {
            std::cout << "no channels left" << std::endl;
            U.DelUser(nick);
        }
        //cout << "KICK" << endl;
    }
}

void UserManagement::MODE(std::vector< std::string > data)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {

    }
    else
    {

    }
    bool add = false;
    int parsepos = 4;
	std::string modeparse = data[3];
    for (unsigned int i = 0; i < modeparse.length(); i++)
    {
        //cout << "modeparse[i]" << modeparse[i] << endl;
        if (modeparse[i] == addchar)
        {
            //cout << "+" << endl;
            add = true;
        }
        if (modeparse[i] == remchar)
        {
            //cout << "-" << endl;
            add = false;
        }
        if (modeparse[i] == ownerchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << ownerchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == adminchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << adminchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == opchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << opchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == halfopchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << halfopchar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == voicechar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << voicechar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
    }
}

void UserManagement::QUIT(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        /*U.~Users();
        C.~Channels();*/
    }
    else
    {
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], nick);
        }
        U.DelUser(nick);
    }
}

void UserManagement::NICK(std::vector< std::string > data)
{
    UsersInterface& U = Global::Instance().get_Users();
    ChannelsInterface& C = Global::Instance().get_Channels();
    std::string oldnick = HostmaskToNick(data);
    std::string nick = data[2];
    boost::erase_all(nick, ":");
    //vector<string> nick = Split(data[2], ":",true,true);
    if (oldnick == Global::Instance().get_BotNick())
    {
        Global::Instance().set_BotNick(nick);
        //botnick = nick[0];
        U.ChangeNick(oldnick, nick);
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick);
        }
        //cout << "NICK" << endl;
    }
    else
    {
        U.ChangeNick(oldnick, nick);
        std::vector< std::string > channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick);
        }
        //cout << "NICK" << endl;
    }
}

void UserManagement::UserAuth(std::string mNick, std::string mAuth)
{
    UsersInterface& U = Global::Instance().get_Users();
    U.SetAuth(mNick, mAuth);
    U.SetOaccess(mNick, -1);
    std::string sqlstring;
    if (U.AddAuth(mAuth) == true)
    {
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		std::stringstream ss;
		ss << uuid;
		std::string UserUuid = ss.str();
		std::cout << "UserUuid: " << UserUuid<< std::endl;
        DatabaseData::Instance().AddAuth(UserUuid, mAuth);
    }
    std::vector< std::string > userchannels = U.GetChannels(mNick);
    if (boost::iequals(userchannels[0], "NULL") == false)
    {
        GetUserInfo(mNick);
    }
    else
    {
        U.DelUser(mNick);
    }
}

std::string UserManagement::HostmaskToNick(std::vector< std::string > data)
{
    std::vector< std::string > who;
    boost::split( who, data[0], boost::is_any_of("!"), boost::token_compress_on );
    std::string nick = who[0];
    boost::erase_all(nick, ":");
    return nick;
}

bool UserManagement::Send(std::string data)
{
    Global::Instance().get_IrcData().AddLowPrioritySendQueue(data);
    return true;
}

std::string UserManagement::convertInt(int number)
{
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int UserManagement::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

void UserManagement::GetUserInfo(std::string data)
{
    UsersInterface& U = Global::Instance().get_Users();
    std::string auth = U.GetAuth(data);
    if (boost::iequals(auth, "NULL") != true)
    {
    	//U.SetUid(data, DatabaseData::Instance().GetUserUuidByAuth(auth));
    	U.SetOaccess(data, DatabaseData::Instance().GetOaccessByAuth(auth));
    	U.SetGod(data, DatabaseData::Instance().GetGodByAuth(auth));
    	U.SetLanguage(data, DatabaseData::Instance().GetLanguageByAuth(auth));
    }
}

void UserManagement::GetChannelInfo(std::string data)
{
    ChannelsInterface& C = Global::Instance().get_Channels();
    C.SetCid(data, DatabaseData::Instance().GetChannelUuidByChannel(data));
    C.SetGiveops(data, DatabaseData::Instance().GetGiveOpsByChannel(data));
    C.SetGivevoice(data, DatabaseData::Instance().GetGiveVoiceByChannel(data));

    std::vector< std::vector< std::string > > channels_vector;
    std::string ChannelUuid = DatabaseData::Instance().GetChannelUuidByChannel(data);
    channels_vector = DatabaseData::Instance().GetUserUuidAndAccessByChannelUuid(ChannelUuid);
	unsigned int i;
    for (i = 0 ; i < channels_vector.size() ; i++)
    {
    	std::string auth = DatabaseData::Instance().GetAuthByUserUuid(channels_vector[i][0]);
    	std::cout << data << " " << auth << " " << channels_vector[i][1] << std::endl;
    	C.AddAuth(data, auth);
        C.SetAccess(data, auth, convertString(channels_vector[i][1]));
    }
}

void UserManagement::GetAuths()
{
    UsersInterface& U = Global::Instance().get_Users();
	std::vector< std::string > auth_vector;
	auth_vector = DatabaseData::Instance().GetAuths();
	unsigned int i;
	for (i = 0; i < auth_vector.size(); i++)
	{
		U.AddAuth(auth_vector[i]);
		std::cout << auth_vector[i] << std::endl;
	}
}
