#ifndef AdminInterface_H
#define AdminInterface_H
#include "../core/Parse.h"

class AdminInterface {

public:
	AdminInterface() { }
	virtual void Init(Parse *p, int port)=0;
	virtual void Run()=0;

};

typedef AdminInterface* create_tai();
typedef void destroy_tai(AdminInterface*);


#endif // AdminInterface_H



