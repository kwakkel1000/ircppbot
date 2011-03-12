#ifndef AdminInterface_H
#define AdminInterface_H
#include "ServerSocket.h"
#include "SocketException.h"
#include "Parse.h"

using namespace std;

class AdminInterface {

    public:

                AdminInterface() { }

                virtual void Init(Parse *p, ServerSocket *s)=0;

                virtual void Run()=0;

//private:
protected:
                ServerSocket *S;

};

typedef AdminInterface* create_tai();

typedef void destroy_tai(AdminInterface*);


#endif // AdminInterface_H



