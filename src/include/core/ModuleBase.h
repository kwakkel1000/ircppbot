#ifndef ModuleBase_H
#define ModuleBase_H

#include <vector>
#include <string>
#include "../interfaces/ModuleInterface.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class ModuleBase : public ModuleInterface
{

    public:

                ModuleBase();
                ~ModuleBase();

    //private:
    protected:
                //global functions

                //irc
				void overwatch(std::string bind, std::string command, std::string chan, std::string nick, std::string auth, std::vector< std::string > args);
                void PRIVMSG(std::vector<std::string> data, std::string trigger);
                virtual void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger){};
                bool Send(std::string data);
                bool SendHighPriority(std::string data);
                bool SendLowPriority(std::string data);
				void simulate(std::string nick, std::string auth, std::string chan, std::string simulate_nick, std::string simulate_command, std::vector< std::string > args, int oa);

                //database
                std::vector< std::vector<std::string> > RawSqlSelect(std::string data);
                bool RawSql(std::string data);

                //reply
                std::string irc_reply(std::string reply_name, std::string reply_language);
                std::string irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string);
                void replyReload();

                //other
                std::string HostmaskToNick(std::vector<std::string> data);
                std::vector<std::string> lineout(std::vector<std::string> data, unsigned int rowamount, unsigned int length);
                std::string convertInt(int);
                int convertString(std::string);

};

#endif // ModuleBase_H




