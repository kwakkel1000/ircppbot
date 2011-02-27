#ifndef Tran_Reply_H
#define Tran_Reply_H

#include <vector>
#include <string>
#include "ConfigReader.h"

class Reply
{

    public:

                Reply();
                ~Reply();
                //init
                void Init(ConfigReader& reader);
                void DBreplyinit();

                //reply
                std::string irc_reply(std::string reply_name, std::string reply_language);
                std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string);

                //reload
                void Reload();

    private:
                //vars
                //database vars
                std::string hostname_str;
                std::string databasename_str;
                std::string username_str;
                std::string pass_str;

                //reply vars
                std::vector<std::string> reply_name_vector;
                std::vector<std::string> reply_vector;
                std::vector<std::string> reply_language_vector;

                //functions

                //database
                std::vector< std::vector<std::string> > RawSqlSelect(std::string data);
};


#endif  // Tran_Reply_H
