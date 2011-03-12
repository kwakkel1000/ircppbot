#include "../../include/Admin.h"
#include "openssl/sha.h"
#include <stdio.h>


extern "C" AdminInterface* create()
{
    return new Admin;
}

extern "C" void destroy(AdminInterface* x)
{
    delete x;
}

void Admin::Init(Parse *p, ServerSocket *s)
{
    cout << "Admin::Init" << endl;
    P=p;
    server = *s;
}

void Admin::Run()
{
    string input = "test";
    char inputbuffer[input.size()];
    strcpy(inputbuffer, input.c_str());
    static char buffer[129];
    sha512(inputbuffer, buffer);
    cout << buffer << endl;
    //send
    try
    {
        run = false;
        while ( 0 )
        {
            /*server.accept ( new_sock );
            new_sock << "press enter to continiue\r\n";
            run = true;
            try
            {
                while ( run )
                {
                    vector<string> result;
                    std::string data;
                    new_sock >> data;
                    cout << data << endl;
                    result = Split(data," ",true,true);
                    parse(result);
                }
            }
            catch ( SocketException& )
            {
                cout << "HALP" << endl;
            }*/
        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }
}

void Admin::parse(vector<string> data)
{
    if (data.size() == 1)
    {
        if (data[0] == "reloadall")      //reloadall
        {
            P->ReloadAll();
        }
        if (data[0] == "close")      //close
        {
            run = false;
        }
    }
}

void Admin::Send(string data)
{
    new_sock << data;
}

void Admin::sha512(char* input, char output[129])
{
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    SHA512_Update(&sha512, input, strlen(input));
    SHA512_Final(hash, &sha512);
    int i = 0;
    for(i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[128] = 0;
}

/// <summary>
/// Splits the string s on the given delimiter(s) and
/// returns a list of tokens without the delimiter(s)
/// </summary>
/// <param name=s>The string being split</param>
/// <param name=match>The delimiter(s) for splitting</param>
/// <param name=removeEmpty>Removes empty tokens from the list</param>
/// <param name=fullMatch>
/// True if the whole match string is a match, false
/// if any character in the match string is a match
/// </param>
/// <returns>A list of tokens</returns>
vector<string> Admin::Split(const string& s, const string& match, bool removeEmpty, bool fullMatch)
{
    typedef string::size_type (string::*find_t)(const string& delim, string::size_type offset) const;
    vector<string> result;                    // return container for tokens
    string::size_type start = 0,            // starting position for searches
    skip = 1;                                // positions to skip after a match
    find_t pfind = &string::find_first_of;    // search algorithm for matches
    if (fullMatch)
    {
        // use the whole match string as a key
        // instead of individual characters
        // skip might be 0. see search loop comments
        skip = match.length();
        pfind = &string::find;
    }
    cout << "vector:";
    while (start != string::npos)
    {
        // get a complete range [start..end)
        string::size_type end = (s.*pfind)(match, start);
        // null strings always match in string::find, but
        // a skip of 0 causes infinite loops. pretend that
        // no tokens were found and extract the whole string
        if (skip == 0) end = string::npos;
        string token = s.substr(start, end - start);

        if (!(removeEmpty && token.empty()))
        {
            // extract the token and add it to the result list
            result.push_back(token);
        }
        // start the next range
        if ((start = end) != string::npos) start += skip;
        cout << " [" << token << "]";
    }
    cout << endl;
    return result;
}
