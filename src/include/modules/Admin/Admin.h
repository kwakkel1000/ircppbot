#ifndef Tran_Admin_H
#define Tran_Admin_H

#include "AdminInterface.h"
#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include "Parse.h"

using namespace std;

class Admin : public AdminInterface
{
public:
    void Init(Parse *p, ServerSocket *s);
    void Run();

private:
    ServerSocket server;
    Parse *P;
    ServerSocket new_sock;
    void parse(vector<string> data);
    void Send(string data);
    vector<string> Split(const std::string&, const std::string&, bool, bool);
    bool run;
    void sha512(char* input, char output[129]);
};

#endif // Tran_Admin_H

