#include "../../include/Support.h"

extern "C" ModuleInterface* create()
{
    return new Support;
}

extern "C" void destroy(ModuleInterface* x)
{
    delete x;
}

void Support::Init(string nick, IrcSocket *s, Users *u, Channels *c, ConfigReader& reader, IrcData *id)
{
    string chandebugstr;
    hostname_str = reader.GetString("hostname");
    databasename_str = reader.GetString("databasename");
    username_str = reader.GetString("username");
    pass_str = reader.GetString("password");
    trigger = reader.GetString("trigger");
    chandebugstr = reader.GetString("chandebug");
    debugchannel = reader.GetString("debugchannel");
    supportchannel = reader.GetString("supportchannel");

    chandebug = (chandebugstr == "true");

    botnick = nick;
    S=s;
    U=u;
    C=c;
    ID=id;
    BindInit();
    DBreplyinit();
    runthreadloop = true;
}

void Support::BindInit()
{

    commands.clear();
    binds.clear();
    oas.clear();

    vector< vector<string> > sql_result;
    string sql_string;
    unsigned int i;

    sql_string = "select command, bind, oaccess from ocommands";
    sql_result = RawSqlSelect(sql_string);
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        commands.push_back(sql_result[i][0]);
        binds.push_back(sql_result[i][1]);
        oas.push_back(convertString(sql_result[i][2]));
    }
}

void Support::ParseData(vector<string> data)
{
    Data = data;
    boost::lock_guard<boost::mutex> lock(parse_mutex);
    data_ready=true;
    parse_wait_condition.notify_one();
}

void Support::stopthreadloop()
{
    runthreadloop = false;
    data_ready=true;
    cout << "notify" << endl;
    parse_wait_condition.notify_one();
}

void Support::threadloop()
{
    while (runthreadloop)
    {
        boost::unique_lock<boost::mutex> lock(parse_mutex);
        while(!data_ready)
        {
            parse_wait_condition.wait(lock);
        }
        data_ready = false;
        if (runthreadloop)
        {
            vector<string> data = Data;
            if (data.size() >= 4)
            {
                if (data[1] == "PRIVMSG")   //PRIVMSG
                {
                    PRIVMSG(data);
                }
            }
        }
    }
}

void Support::PRIVMSG(vector<string> data)
{
    vector<string> args;
    vector<string> data3;
    size_t chanpos1;
    size_t chanpos2;
    size_t triggerpos;
    chanpos1 = data[2].find("#");
    chanpos2 = data[3].find("#");
    triggerpos = data[3].find(trigger);
    string chan = "NULL";
    string command = "NULL";
    int triggered = 0;
    int triggertype = -1;
    int chantrigger = -1;
    if (data.size() >= 4)
    {
        data3 = Split(data[3], ":",true,true);
    }
    if (triggerpos != string::npos)
    {
        triggertype = 1; //PRIVMSG ... :!;
        if (data3.size() > 0)
        {
            if (chanpos1 != string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;    //PRIVMSG nick #channel :!#chan command
                //cout << "channel: triggerchannel command" << endl;
                if (data.size() >= 5)
                {
                    vector<string> tmpchan = Split(data3[0], trigger,true,true);
                    if (tmpchan.size() > 0)
                    {
                        triggered = 1;
                        chan = tmpchan[0];
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 != string::npos && chanpos2 == string::npos)
            {
                chantrigger = 0;    //PRIVMSG nick #chan :!command
                //cout << "channel: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    vector<string> tmpcommand = Split(data3[0], trigger,true,true);
                    chan = data[2];
                    if (tmpcommand.size() > 0)
                    {
                        triggered = 1;
                        command = tmpcommand[0];
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 == string::npos && chanpos2 == string::npos)
            {
                chantrigger = -1;   //PRIVMSG nick user :!command
                //cout << "user: triggercommand" << endl;
                if (data.size() >= 4)
                {
                    vector<string> tmpcommand = Split(data3[0], trigger,true,true);
                    chan = "NULL";
                    if (tmpcommand.size() > 0)
                    {
                        triggered = 1;
                        command = tmpcommand[0];
                        for (unsigned int i = 4 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
            else if (chanpos1 == string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :!#chan command
                //cout << "user: triggerchannel command" << endl;
                if (data.size() >= 5)
                {
                    vector<string> tmpchan = Split(data3[0], trigger,true,true);
                    if (tmpchan.size() > 0)
                    {
                        triggered = 1;
                        chan = tmpchan[0];
                        command = data[4];
                        for (unsigned int i = 5 ; i < data.size() ; i++)
                        {
                            args.push_back(data[i]);
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (data3.size() > 0)
        {
            if (chanpos1 == string::npos && chanpos2 != string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :#chan command
                //cout << "user: channel command" << endl;
                if (data.size() >= 5)
                {
                    triggered = 1;
                    chan = data3[0];
                    command = data[4];
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                }
            }
            if (chanpos1 == string::npos && chanpos2 == string::npos)
            {
                chantrigger = 1;   //PRIVMSG nick user :command
                //cout << "user: command" << endl;
                if (data.size() >= 5)
                {
                    chan == "NULL";
                    triggered = 1;
                    command = data3[0];
                    for (unsigned int i = 5 ; i < data.size() ; i++)
                    {
                        args.push_back(data[i]);
                    }
                }
            }
        }
    }
    if (triggered == 1)
    {
        cout << "Support" << endl;
        //cout << "chan " << chan << endl;
        //cout << "command " << command << endl;
        string nick = HostmaskToNick(data);
        string auth = U->GetAuth(nick);
        if (args.size() == 0)
        {
            for (unsigned int i = 0; i < binds.size(); i++)
            {
                if (caseInsensitiveStringCompare(command, binds[i]))
                {
                }
            }
        }
        if (args.size() >= 1)
        {
            for (unsigned int i = 0; i < binds.size(); i++)
            {
                if (caseInsensitiveStringCompare(command, binds[i]))
                {
                    if (caseInsensitiveStringCompare(commands[i], "support"))
                    {
                        string supportstring;
                        for (unsigned int j = 0; j < args.size()-1; j++)
                        {
                            supportstring = supportstring + args[j] + " ";
                        }
                        if (args.size() > 0)
                        {
                            supportstring = supportstring + args[args.size()-1];
                        }
                        support(nick, auth, supportstring);
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                }
            }
        }
        if (args.size() == 3)
        {
            for (unsigned int i = 0; i < binds.size(); i++)
            {
                if (caseInsensitiveStringCompare(command, binds[i]))
                {
                    if (caseInsensitiveStringCompare(commands[i], "addznc"))
                    {
                        //addznc(nick, auth, port, );
                        overwatch(commands[i], command, chan, nick, auth, args);
                    }
                }
            }
        }
    }
}

void Support::overwatch(string bind, string command, string chan, string nick, string auth, vector<string> args)
{
    string debugstring = "PRIVMSG " + debugchannel + " :[" + nick + ":" + auth + "] [" + chan + ":" + convertInt(C->GetAccess(chan, auth)) + "] ";
    if (U->GetGod(nick) == 1)
    {
        debugstring = debugstring + "[G] ";
    }
    debugstring = debugstring + command + ":" + bind;
    for (unsigned int i = 0; i < args.size(); i++)
    {
        debugstring = debugstring + " " + args[i];
    }
    debugstring = debugstring + "\r\n";
    Send(debugstring);
}

void Support::support(string nick, string auth, string supportstring)
{
    cout << "Support::support" << endl;
    support_ret = support_nick_string_map.insert (pair<string, string>(nick, supportstring));
    if (support_ret.second == true)
    {
        cout << "new ticket" << endl;
        support_nick.push_back(nick);
    }
    else
    {
        cout << "nickname has already a ticket" << endl;
    }
}
/*
void Support::JOIN(vector<string> data)
{
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
    }
    else
    {
        C->AddNick(chan[0], nick);
        bool added = false;
        added = U->AddUser(nick);
        U->AddChannel(nick, chan[0]);
        if (added)
        {
            string whoisstring = "WHOIS " + nick + " " + nick + "\r\n";
            Send(whoisstring);
            cout << "Parse::JOIN(vector<string> data)  user added " << nick << "\r\n";
        }
    }
}

void Support::PART(vector<string> data)
{
    vector<string> chan = Split(data[2], ":",true,true);
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
    }
    else
    {
        C->DelNick(chan[0], nick);
        U->DelChannel(nick, chan[0]);
        if (U->GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U->DelUser(nick);
        }
        cout << "PART" << endl;
    }
}

void Support::KICK(vector<string> data)
{
    string chan = data[2];
    string nick = data[3];
    if (nick == botnick)
    {
    }
    else
    {
        C->DelNick(chan, nick);
        U->DelChannel(nick, chan);
        if (U->GetChannels(nick)[0] == "NULL")
        {
            cout << "no channels left" << endl;
            U->DelUser(nick);
        }
        cout << "KICK" << endl;
    }
}

void Support::QUIT(vector<string> data)
{
    string nick = HostmaskToNick(data);
    if (nick == botnick)
    {
    }
    else
    {
        vector<string> channels = U->GetChannels(nick);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C->DelNick(channels[i], nick);
        }
        U->DelUser(nick);
    }
}

void Support::NICK(vector<string> data)
{
    string oldnick = HostmaskToNick(data);
    vector<string> nick = Split(data[2], ":",true,true);
    if (oldnick == botnick)
    {
    }
    else
    {
        U->ChangeNick(oldnick, nick[0]);
        vector<string> channels = U->GetChannels(nick[0]);
        for ( unsigned int i = 0 ; i < channels.size(); i++ )
        {
            C->DelNick(channels[i], oldnick);
            C->AddNick(channels[i], nick[0]);
        }
        cout << "NICK" << endl;
    }
}
*/
bool Support::Send(string data)
{
    ID->AddSendQueue(data);
    /*
    cout << "bool Support::Send(string data)" << endl;
    cout << ">> " << data;
    try
    {
        S->Send(data);
    }
    catch (IrcSocket::Exception& e)
    {
        cout << "Exception caught: " << e.Description() << endl;
        return false;
    }*/
    return true;
}

void Support::DBreplyinit()
{
    vector< vector<string> > sql_result;
    string sql_string = "select reply_name, reply, language FROM reply;";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << endl;
        reply_name_vector.push_back(sql_result[i][0]);
        reply_vector.push_back(sql_result[i][1]);
        reply_language_vector.push_back(sql_result[i][2]);
    }
}

string Support::irc_reply(string reply_name, string reply_language)
{
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (caseInsensitiveStringCompare(reply_name, reply_name_vector[i]) && caseInsensitiveStringCompare(reply_language, reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    return "NO SUCH REPLY " + reply_name;
}

string Support::irc_reply_replace(string source_string, string search_string, string replace_string)
{
    size_t search_pos;
    search_pos = source_string.find(search_string);
    if (search_pos != string::npos)
    {
        source_string.replace(search_pos, search_string.length(), replace_string);
    }
    return source_string;
}

vector< vector<string> > Support::RawSqlSelect(string data)
{
    cout << data << endl;
    database *db;
    vector< vector<string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
    delete db;
    return sql_result;
}

bool Support::RawSql(string data)
{
    cout << data << endl;
    database *db;
    db = new database();    // lol whut... connecting for each query? :'D
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    db->updateQuery( data.c_str() );
    db->disconnect();
    delete db;
    return true;
}


string Support::convertInt(int number)
{
    stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int Support::convertString(string data)
{
    int i;
    stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}

vector<string> Support::lineout(vector<string> data, unsigned int rowamount, unsigned int length)
{
    vector<string> return_vector;
    string tmpvector;
    unsigned int k = 0;
    for (unsigned int j = 0; j < data.size(); j++)
    {
        if (k < rowamount)
        {
            tmpvector = tmpvector + data[j];
            for (unsigned int l = data[j].size(); l < length; l++)
            {
                tmpvector = tmpvector + " ";
            }
            k++;
        }
        if (k >= rowamount)
        {
            return_vector.push_back(tmpvector);
            tmpvector = "";
            k = 0;
        }
    }
    if (k != 0)
    {
        return_vector.push_back(tmpvector);
    }
    return return_vector;
}

string Support::HostmaskToNick(vector<string> data)
{
    vector<string> who;
    who = Split(data[0],"!",true,true);
    string nick = who[0];
    who = Split(nick, ":",true,true);
    nick = who[0];
    return nick;
}

bool Support::caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 ) {
    std::string str1Cpy( str1 );
    std::string str2Cpy( str2 );
    std::transform( str1Cpy.begin(), str1Cpy.end(), str1Cpy.begin(), ::tolower );
    std::transform( str2Cpy.begin(), str2Cpy.end(), str2Cpy.begin(), ::tolower );
    return ( str1Cpy == str2Cpy );
}

void Support::timerrun()
{
    //cout << "Support::timerrun()" << endl;
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

vector<string> Support::Split(const string& s, const string& match, bool removeEmpty, bool fullMatch)
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

    //cout << "vector:";

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

        //cout << " [" << token << "]";

    }

    //cout << "\n";


    return result;

}
