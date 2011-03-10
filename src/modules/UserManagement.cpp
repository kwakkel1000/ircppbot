#include "../../include/UserManagement.h"

#include "../../include/IrcSocket.h"
#include "../../include/Users.h"
#include "../../include/Channels.h"
#include "../../include/Database.h"
#include "../../include/Global.h"

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>


const char addchar    = '+';
const char remchar    = '-';

const char gonechar   = 'G';
const char herechar   = 'H';
const char operchar   = '*';
const char botchar    = 'd';

const char ownerchar  = 'q';
const char adminchar  = 'a';
const char opchar     = 'o';
const char halfopchar = 'h';
const char voicechar  = 'v';

const char ownerwhochar  = '~';
const char adminwhochar  = '&';
const char opwhochar     = '@';
const char halfopwhochar = '%';
const char voicewhochar  = '+';


UserManagement::UserManagement()
{

}

void UserManagement::Init()
{
    D = new Data();
    D->Init(true, false, false, false);
    Global::Instance().get_IrcData().AddConsumer(D);
}

void UserManagement::WHO(vector<string> data)
{
    string chan = data[3];
    string nick = data[7];
    string modes = data[8];

    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    C.AddNick(chan, nick);
    bool added = U.AddUser(nick);
    U.AddChannel(nick, chan);

	size_t Gonepos = modes.find(gonechar);
    if (Gonepos != string::npos)
    {
        U.SetGone(nick, true);
    }

    size_t Herepos = modes.find(herechar);
	if (Herepos != string::npos)
    {
        U.SetGone(nick, false);
    }

    size_t Ownerpos = modes.find(ownerwhochar);
	if (Ownerpos != string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Adminpos = modes.find(adminwhochar);
	if (Adminpos != string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Oppos = modes.find(opwhochar);
	if (Oppos != string::npos)
    {
        C.SetOp(chan, nick, true);
    }

    size_t Halfoppos = modes.find(halfopwhochar);
	if (Halfoppos != string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Voicepos = modes.find(voicewhochar);
	if (Voicepos != string::npos)
    {
        C.SetVoice(chan, nick, true);
    }

    size_t Xpos = modes.find("x");
	if (Xpos != string::npos)
    {
        U.SetX(nick, true);
    }

    size_t Dpos = modes.find(botchar);
	if (Dpos != string::npos)
    {
        U.SetD(nick, true);
    }

    size_t Ircoppos = modes.find(operchar);
	if (Ircoppos != string::npos)
    {
        U.SetIrcop(nick, true);
    }

	if (added)
    {
        U.AddWhois(nick);
        //if (WhoisSend == false)
        //{
            string whoisstring = "WHOIS " + nick + " " + nick + "\r\n";
            Send(whoisstring);
            //WhoisSend = true;
        //}
        //cout << "UserManagement::WHO(vector<string> data)  user added " << nick << "\r\n";
    }
}

void UserManagement::WHOIS(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    U.DelWhois(data[3]);
    //WhoisSend = false;
    string whois = U.GetWhois();
    if (whois != "NULL")
    {
        whois = "WHOIS " + whois + " " + whois + "\r\n";
        Send(whois);
    }
}

void UserManagement::JOIN(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        C.AddChannel(chan[0]);
        string whostring = "WHO " + chan[0] + "\r\n";
        Send(whostring);
        DBChannelInfo(chan[0]);
    }
    else
    {
        C.AddNick(chan[0], nick);
        bool added = false;
        added = U.AddUser(nick);
        U.AddChannel(nick, chan[0]);
        if (added)
        {
            string whoisstring = "WHOIS " + nick + " " + nick + "\r\n";
            Send(whoisstring);
            //cout << "Parse::JOIN(vector<string> data)  user added " << nick << "\r\n";
        }
    }
}

void UserManagement::PART(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        vector<string> chanusers = C.GetNicks(chan[0]);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            U.DelChannel(chanusers[i], chan[0]);
            C.DelNick(chan[0], chanusers[i]);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                cout << "no channels left" << endl;
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan[0]);
    }
    else
    {
        C.DelNick(chan[0], nick);
        U.DelChannel(nick, chan[0]);
        if (U.GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U.DelUser(nick);
        }
        //cout << "PART" << endl;
    }
}

void UserManagement::KICK(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    string chan = data[2];
    string nick = data[3];
    if (nick == Global::Instance().get_BotNick())
    {
        vector<string> chanusers = C.GetNicks(chan);
        for ( unsigned int i = 0 ; i < chanusers.size(); i++ )
        {
            C.DelNick(chan, chanusers[i]);
            U.DelChannel(chanusers[i], chan);
            if (U.GetChannels(chanusers[i])[0] == "NULL")
            {
                cout << "no channels left" << endl;
                U.DelUser(chanusers[i]);
            }
        }
        C.DelChannel(chan);
    }
    else
    {
        C.DelNick(chan, nick);
        U.DelChannel(nick, chan);
        if (U.GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U.DelUser(nick);
        }
        //cout << "KICK" << endl;
    }
}

void UserManagement::MODE(vector<string> data)
{
    Channels& C = Global::Instance().get_Channels();
    string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {

    }
    else
    {

    }
    bool add = false;
    int parsepos = 4;
	string modeparse = data[3];
    for (unsigned int i = 0; i < modeparse.length(); i++)
    {
        //cout << "modeparse[i]" << modeparse[i] << endl;
        if (modeparse[i] == addchar)
        {
            //cout << "+" << endl;
            add = true;
        }
        if (modeparse[i] == remchar)
        {
            //cout << "-" << endl;
            add = false;
        }
        if (modeparse[i] == ownerchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << ownerchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == adminchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << adminchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == opchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << opchar << endl;
            C.SetOp(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == halfopchar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << halfopchar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
        if (modeparse[i] == voicechar)
        {
            //cout << "chan: " << data[2] << " user: " << data[parsepos] << " " << voicechar << endl;
            C.SetVoice(data[2], data[parsepos], add);
            parsepos++;
        }
    }
}

void UserManagement::QUIT(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    string nick = HostmaskToNick(data);
    if (nick == Global::Instance().get_BotNick())
    {
        U.~Users();
        C.~Channels();
    }
    else
    {
        vector<string> channels = U.GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], nick);
        }
        U.DelUser(nick);
    }
}

void UserManagement::NICK(vector<string> data)
{
    Users& U = Global::Instance().get_Users();
    Channels& C = Global::Instance().get_Channels();
    string oldnick = HostmaskToNick(data);
    vector<string> nick = Split(data[2], ":",true,true);
    if (oldnick == Global::Instance().get_BotNick())
    {
        Global::Instance().set_BotNick(nick[0]);
        //botnick = nick[0];
        U.ChangeNick(oldnick, nick[0]);
        vector<string> channels = U.GetChannels(nick[0]);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick[0]);
        }
        //cout << "NICK" << endl;
    }
    else
    {
        U.ChangeNick(oldnick, nick[0]);
        vector<string> channels = U.GetChannels(nick[0]);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C.DelNick(channels[i], oldnick);
            C.AddNick(channels[i], nick[0]);
        }
        //cout << "NICK" << endl;
    }
}

string UserManagement::HostmaskToNick(vector<string> data)
{
    vector<string> who;
    who = Split(data[0],"!",true,true);
    string nick = who[0];
    who = Split(nick, ":",true,true);
    nick = who[0];
    return nick;
}

bool UserManagement::Send(string data)
{
    Global::Instance().get_IrcData().AddLowPrioritySendQueue(data);
    return true;
}

// Hmm... where have i seen this function before...
vector< vector<string> > UserManagement::RawSqlSelect(string data)
{
    ConfigReader& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    cout << data << endl;
    vector< vector<string> > sql_result;
    database *db;
    db = new database();
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
	delete db;
    return sql_result;
}

// Looksie, another function i know ;)
bool UserManagement::RawSql(string data)
{
    ConfigReader& CR = Global::Instance().get_ConfigReader();
    std::string hostname_str = CR.GetString("hostname");
    std::string databasename_str = CR.GetString("databasename");
    std::string username_str = CR.GetString("username");
    std::string pass_str = CR.GetString("password");
    cout << data << endl;
    database *db;
    db = new database();
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    db->updateQuery( data.c_str() );
    db->disconnect();
	delete db;
    return true;
}

string UserManagement::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int UserManagement::convertString(string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

void UserManagement::DBUserInfo(string data)
{
    Users& U = Global::Instance().get_Users();
    string auth = U.GetAuth(data);
    if (boost::iequals(auth,"NULL") != true)
    {
        vector< vector<string> > sql_result;
		// Note: you should escape the "auth" var before inserting into query
        string sql_string = "select auth.id, auth.oaccess, auth.god from auth where auth = '" + auth + "';";
        sql_result = RawSqlSelect(sql_string);
        unsigned int i;
        for (i = 0 ; i < sql_result.size() ; i++)
        {
            cout << sql_result[i][1] << endl;
            U.SetUid(data, convertString(sql_result[i][0]));
            U.SetOaccess(data, convertString(sql_result[i][1]));
            U.SetGod(data, convertString(sql_result[i][2]));
        }
    }
}

void UserManagement::DBChannelInfo(string data)
{   ////// vet onhandige sql query's binnenkort maar ff 2 van maken.
    Channels& C = Global::Instance().get_Channels();
    vector< vector<string> > sql_result;
    string sql_string = "select channels.id, users.access, auth.auth, channels.giveops, channels.givevoice from users JOIN auth ON users.uid = auth.id JOIN channels ON users.cid = channels.id where channels.channel = '" + data + "';";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << " " << sql_result[i][3] << " " << sql_result[i][4] << endl;
        C.SetCid(data, convertString(sql_result[i][0]));
        C.AddAuth(data, sql_result[i][2]);
        C.SetAccess(data, sql_result[i][2], convertString(sql_result[i][1]));
        C.SetGiveops(data, convertString(sql_result[i][3]));
        C.SetGivevoice(data, convertString(sql_result[i][4]));
        C.SetGiveops(data, convertString(sql_result[i][3]));
        C.SetGivevoice(data, convertString(sql_result[i][4]));
    }
}

void UserManagement::DBinit()
{
    Users& U = Global::Instance().get_Users();
	vector< vector<string> > sql_result;
	string sql_string = "select auth from auth";
	sql_result = RawSqlSelect(sql_string);
	unsigned int i;
	for (i = 0; i < sql_result.size(); i++)
	{
		U.AddAuth(sql_result[i][0]);
		cout << sql_result[i][0] << endl;
	}
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
vector<string> UserManagement::Split(const string& s, const string& match, bool removeEmpty, bool fullMatch){
    typedef string::size_type (string::*find_t)(const string& delim, string::size_type offset) const;
    vector<string> result;                 // return container for tokens
    string::size_type start = 0,           // starting position for searches
                      skip = 1;            // positions to skip after a match
    find_t pfind = &string::find_first_of; // search algorithm for matches

    if (fullMatch)
    {
        // use the whole match string as a key
        // instead of individual characters
        // skip might be 0. see search loop comments
        skip = match.length();
        pfind = &string::find;
    }
    //cout << "vector:";
    while (start != string::npos){
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
        //cout << " [" << token << "]";
    }
    //cout << "\n";

    return result;
}
