#ifndef Reply_H
#define Reply_H
#include "../interfaces/ReplyInterface.h"

class Reply : public ReplyInterface
{

	public:
		Reply();
		~Reply();
		//init
		void Init();
		void DBreplyinit();

		//reply
		std::string irc_reply(std::string reply_name, std::string reply_language);
		std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string);

		//reload
		void Reload();

	private:
		//vars

		//reply vars
		std::vector< std::string > reply_name_vector;
		std::vector< std::string > reply_vector;
		std::vector< std::string > reply_language_vector;

		//database
		std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
};


#endif  // Reply_H
