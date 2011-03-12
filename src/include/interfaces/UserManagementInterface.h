#ifndef UserManagementInterface_H
#define UserManagementInterface_H
class UserManagementInterface
{
public:
    UserManagementInterface() { }
    virtual void Init()=0;
    virtual void read()=0;
    virtual void stop()=0;

};

typedef UserManagementInterface* create_tUMI();

typedef void destroy_tUMI(UserManagementInterface*);

#endif // UserManagementInterface_H


