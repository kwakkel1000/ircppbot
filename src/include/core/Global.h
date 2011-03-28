#ifndef Global_H
#define Global_H

#include <string>
#include "../interfaces/UsersInterface.h"
#include "../interfaces/ChannelsInterface.h"
#include "../interfaces/IrcDataInterface.h"
#include "../interfaces/ReplyInterface.h"
#include "../interfaces/ConfigReaderInterface.h"

class UsersInterface;
class ChannelsInterface;
class IrcDataInterface;
class ReplyInterface;
class ConfigReaderInterface;
class Global
{
    public:
        static Global& Instance()
        {
            static Global obj;
            return obj;
        }

        void delete_all();

        virtual void set_Run(bool _Run);
        virtual bool get_Run();

        virtual void set_BotNick(std::string _BotNick);
        virtual std::string get_BotNick();

        virtual void set_ConfigFile(std::string _ConfigFile);
        virtual std::string get_ConfigFile();

        virtual void set_Users(UsersInterface* _Users);
        virtual UsersInterface& get_Users();

        virtual void set_Channels(ChannelsInterface* _Channels);
        virtual ChannelsInterface& get_Channels();

        virtual void set_IrcData(IrcDataInterface* _IrcData);
        virtual IrcDataInterface& get_IrcData();

        virtual void set_Reply(ReplyInterface* _Reply);
        virtual ReplyInterface& get_Reply();

        virtual void set_ConfigReader(ConfigReaderInterface* _ConfigReader);
        virtual ConfigReaderInterface& get_ConfigReader();

    private:
        Global() {}
        ~Global() {}

        bool m_Run;

        std::string m_BotNick;
		std::string m_ConfigFile;
        UsersInterface* m_Users;
        ChannelsInterface* m_Channels;
        IrcDataInterface* m_IrcData;
        ReplyInterface* m_Reply;
        ConfigReaderInterface* m_ConfigReader;

};

#endif // Global_H





