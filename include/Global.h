#ifndef Global_H
#define Global_H

#include <string>

#include "Users.h"
#include "Channels.h"
#include "IrcData.h"
#include "Reply.h"
#include "ConfigReader.h"

//#include "Database.h"

class Users;
class Channels;
class IrcData;
class Reply;
class ConfigReader;
class Global
{
    public:
        static Global& Instance()
        {
            static Global obj;
            return obj;
        }

        void set_BotNick(std::string _BotNick);
        std::string get_BotNick();

        void set_Users(Users* _Users);
        Users& get_Users();

        void set_Channels(Channels* _Channels);
        Channels& get_Channels();

        void set_IrcData(IrcData* _IrcData);
        IrcData& get_IrcData();

        void set_Reply(Reply& _Reply);
        Reply& get_Reply();

        void set_ConfigReader(ConfigReader& _ConfigReader);
        ConfigReader& get_ConfigReader();

    private:
        Global() {}
        ~Global() {}

        std::string m_BotNick;
        Users* m_Users;
        Channels* m_Channels;
        IrcData* m_IrcData;
        Reply m_Reply;
        ConfigReader m_ConfigReader;

};

#endif // Global_H





