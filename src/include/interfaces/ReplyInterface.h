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

	//reload
	virtual void Reload()=0;
};


#endif  // ReplyInterface_H

