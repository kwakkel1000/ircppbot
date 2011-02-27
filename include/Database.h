#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**************
* Database Error Definitions
**************/
// I would make an enum out of these like:
/*
enum DatabaseErrors
{
    DB_SUCCESS = 200,
    DB_ERR201 = 201, // use meaningfull names instead of these :-)
    DB_ERR202 = 202,
    DB_ERR203 = 203
}
*/
#define SUCCESS 200
#define ERR201 201
#define ERR202 202
#define ERR203 203

#define MAX_QUERY_LEN 500

// Meh! I changed all the char* to const char* but it would be easier if they where std::string's
class database
{
public:
    //class constructor
    database();

    //connection & disconnect
    int       openConnection(const char *host, const char *db, const char *user, const char *pass); //connect to the database
    bool      disconnect(); //disconnect from the database

    //querying
    MYSQL_RES *query(const char *query); //return result set
    char      *stringQuery(const char *query); //return string
    int       intQuery(const char *query); //return an integer
    bool      boolQuery(const char *query); //return a boolean
    bool      updateQuery(const char *query); //updates, returns true if update went through
    vector< vector<std::string> > sql_query(const char* sql_string );
    //free the query results
    bool free();

    private:
    MYSQL *sock;
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    char *qbuf[MAX_QUERY_LEN];
    int convertString(std::string data);
};

const char *dberror(int errorcode); //return a string for this error message

#endif // DATABASE_H
