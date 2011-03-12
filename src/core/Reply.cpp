#include "../include/core/Reply.h"
#include "../include/core/Database.h"
#include "../include/core/Global.h"
#include <boost/algorithm/string.hpp>

//public
Reply::Reply()
{

}

Reply::~Reply()
{

}

//init
void Reply::Init()
{
    DBreplyinit();
}

//reply
std::string Reply::irc_reply(std::string reply_name, std::string reply_language)
{
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (boost::iequals(reply_name, reply_name_vector[i]) && boost::iequals(reply_language, reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    for (unsigned int i = 0; i < reply_name_vector.size(); i++)
    {
        if (boost::iequals(reply_name, reply_name_vector[i]) && boost::iequals(Global::Instance().get_ConfigReader().GetString("defaultlanguage"), reply_language_vector[i]))
        {
            return reply_vector[i];
        }
    }
    return "NO SUCH REPLY " + reply_name;
}

//reload
void Reply::Reload()
{
    DBreplyinit();
}

//private
//init
void Reply::DBreplyinit()
{
    reply_name_vector.clear();
    reply_vector.clear();
    reply_language_vector.clear();
    std::vector< std::vector< std::string > > sql_result;
    std::string sql_string = "select reply_name, reply, language FROM reply;";
    sql_result = RawSqlSelect(sql_string);
    unsigned int i;
    for (i = 0 ; i < sql_result.size() ; i++)
    {
        //cout << sql_result[i][0] << " " << sql_result[i][1] << " " << sql_result[i][2] << endl;
        reply_name_vector.push_back(sql_result[i][0]);
        reply_vector.push_back(sql_result[i][1]);
        reply_language_vector.push_back(sql_result[i][2]);
    }
}

//mysql
std::vector< std::vector< std::string > > Reply::RawSqlSelect(std::string data)
{
    //cout << data << endl;
	std::string hostname_str = Global::Instance().get_ConfigReader().GetString("hostname");
    std::string databasename_str = Global::Instance().get_ConfigReader().GetString("databasename");
    std::string username_str = Global::Instance().get_ConfigReader().GetString("username");
    std::string pass_str = Global::Instance().get_ConfigReader().GetString("password");
    database *db;
    std::vector< std::vector<std::string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    db->openConnection(hostname_str.c_str(), databasename_str.c_str(), username_str.c_str(), pass_str.c_str());
    sql_result = db->sql_query( data.c_str() );
    db->disconnect();
    delete db;
    return sql_result;
}

//reply
std::string Reply::irc_reply_replace(std::string source_string, std::string search_string, std::string replace_string)
{
    size_t search_pos;
    search_pos = source_string.find(search_string);
    if (search_pos != string::npos)
    {
        source_string.replace(search_pos, search_string.length(), replace_string);
    }
    return source_string;
}
