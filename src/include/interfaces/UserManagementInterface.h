#ifndef UserManagementInterface_H
#define UserManagementInterface_H
#include "DataInterface.h"
class UserManagementInterface
{
public:
    UserManagementInterface() { }
    virtual void Init(DataInterface* pData)=0;
    virtual void read()=0;
    virtual void stop()=0;

};

typedef UserManagementInterface* create_tUMI();

typedef void destroy_tUMI(UserManagementInterface*);

#endif // UserManagementInterface_H


