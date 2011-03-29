#ifndef AdminInterface_H
#define AdminInterface_H
#include "../core/Bot.h"

class Bot;
class AdminInterface {

public:
	AdminInterface() { }
	virtual void Init(Bot *bot, int port)=0;
	virtual void Run()=0;

};

typedef AdminInterface* create_tai();
typedef void destroy_tai(AdminInterface*);


#endif // AdminInterface_H



