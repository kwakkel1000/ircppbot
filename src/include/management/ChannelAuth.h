#ifndef ChannelAuth_h
#define ChannelAuth_h

class ChannelAuth {

public:

    ChannelAuth();
    ~ChannelAuth();

    void SetAccess(int);
    int GetAccess();

private:
        int access;
};


#endif // ChannelAuth_h
