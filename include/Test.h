#ifndef Test_H
#define Test_H

#include "ModuleInterface.h"
#include "ModuleBase.h"
#include <string>
#include <vector>

class Test : public ModuleBase
{
public:
    Test();
    void threadloop();
    void stopthreadloop();
    void Init();
    void timerrun();

private:
    void PRIVMSG(std::vector< std::string > data);
    bool runthreadloop;
};

#endif // Test_H

