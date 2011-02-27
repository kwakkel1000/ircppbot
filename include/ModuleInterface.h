#ifndef Tran_ModuleInterface_H
#define Tran_ModuleInterface_H
#include <vector>
#include <string>
#include "Users.h"
#include "Channels.h"
#include "ConfigReader.h"
#include "IrcData.h"
#include "Reply.h"

class Users;
class Channels;
class IrcData;
class Reply;
class ModuleInterface {

    public:

                ModuleInterface() { }
                virtual void BaseInit(std::string nick, Users *u, Channels *c, ConfigReader& reader, IrcData *id, Reply *r)=0;
                virtual void threadloop()=0;
                virtual void stopthreadloop()=0;
                virtual void Init()=0;
                virtual void timerrun()=0;
};

typedef ModuleInterface* create_tmi();

typedef void destroy_tmi(ModuleInterface*);


#endif // Tran_ModuleInterface_H



