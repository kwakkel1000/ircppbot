#ifndef Tran_ModuleBase_H
#define Tran_ModuleBase_H

#include <vector>
#include <string>
#include "ModuleInterface.h"
#include "Database.h"
#include "Users.h"
#include "Channels.h"
#include "ConfigReader.h"
#include "IrcData.h"
#include "Data.h"
#include "Reply.h"

using namespace std;

class Users;
class Channels;
class IrcData;
class Data;
class Reply;
class ConfigReader;
class ModuleBase : public ModuleInterface
{

    public:

                ModuleBase();
                ~ModuleBase();
                //init
                void BaseInit(std::string nick, Users *u, Channels *c, ConfigReader& reader, IrcData *id, Reply *r);

    //private:
    protected:

                //class objecten
                IrcData *ID;
                Data *D;
                Users *U;
                Channels *C;
                Reply *R;

                //vars
                std::string botnick;
                bool chandebug;
                std::string debugchannel;
                std::string hostname_str;
                std::string databasename_str;
                std::string username_str;
                std::string pass_str;
                std::string trigger;



                //global functions

                //irc
                void PRIVMSG(std::vector<std::string> data);
                virtual void ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger){};
                bool Send(std::string data);

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

#endif // Tran_ModuleBase_H




