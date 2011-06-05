#ifndef ReplyInterface_H
#define ReplyInterface_H
#include <string>
#include <vector>
class ReplyInterface
{
public:
	virtual void Init()=0;
	virtual void DBreplyinit()=0;

	//reply
	virtual std::string irc_reply(std::string reply_name, std::string reply_language)=0;
	virtual std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)=0;

	virtual std::string irc_privmsg(std::string target, std::string text)=0;
	virtual std::string irc_notice(std::string target, std::string text)=0;
	virtual std::string irc_mode(std::string target, std::string mode)=0;
	virtual std::string irc_join(std::string channel)=0;
	virtual std::string irc_part(std::string channel, std::string reason)=0;
	virtual std::string irc_kick(std::string channel, std::string target, std::string reason)=0;
	virtual std::string irc_invite(std::string channel, std::string target)=0;

	//reload
	virtual void Reload()=0;
};


#endif  // ReplyInterface_H

