#ifndef GlobalInterface_H
#define GlobalInterface_H

#include <string>
#include "UsersInterface.h"
#include "ChannelsInterface.h"
#include "IrcDataInterface.h"
#include "ReplyInterface.h"
#include "ConfigReaderInterface.h"
class GlobalInterface {

    public:

        virtual GlobalInterface& Instance()=0;

        virtual void delete_all();

        virtual void set_Run(bool _Run)=0;
        virtual bool get_Run()=0;

        virtual void set_BotNick(std::string _BotNick)=0;
        virtual std::string get_BotNick()=0;

        virtual void set_ConfigFile(std::string _ConfigFile)=0;
        virtual std::string get_ConfigFile()=0;

        virtual void set_Users(UsersInterface* _Users)=0;
        virtual UsersInterface& get_Users()=0;

        virtual void set_Channels(ChannelsInterface* _Channels)=0;
        virtual ChannelsInterface& get_Channels()=0;

        virtual void set_IrcData(IrcDataInterface* _IrcData)=0;
        virtual IrcDataInterface& get_IrcData()=0;

        virtual void set_Reply(ReplyInterface* _Reply)=0;
        virtual ReplyInterface& get_Reply()=0;

        virtual void set_ConfigReader(ConfigReaderInterface* _ConfigReader)=0;
        virtual ConfigReaderInterface& get_ConfigReader()=0;
};
#endif // GlobalInterface_H




