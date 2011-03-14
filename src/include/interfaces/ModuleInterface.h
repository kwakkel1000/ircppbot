#ifndef ModuleInterface_H
#define ModuleInterface_H
#include "DataInterface.h"
class ModuleInterface
{
public:
	ModuleInterface() { }
	virtual void read()=0;
	virtual void stop()=0;
	virtual void Init(DataInterface* pData)=0;
	virtual void timerrun()=0;
};

typedef ModuleInterface* create_tmi();

typedef void destroy_tmi(ModuleInterface*);


#endif // ModuleInterface_H



