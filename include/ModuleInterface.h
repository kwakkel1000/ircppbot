#ifndef ModuleInterface_H
#define ModuleInterface_H
class ModuleInterface {

    public:
        ModuleInterface() { }
        virtual void threadloop()=0;
        virtual void stopthreadloop()=0;
        virtual void Init()=0;
        virtual void timerrun()=0;
};

typedef ModuleInterface* create_tmi();

typedef void destroy_tmi(ModuleInterface*);


#endif // ModuleInterface_H



