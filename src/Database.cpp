#include <iostream>
/**************
* File Name: database.cpp
* Author: Jade@design1online.com
* Purpose: mysql database class
**************/
#include "../include/Database.h"
using namespace std;
/**************
* Purpose: default constructor, initialize class values
* Precondition: none
* Postcondition: none
**************/
database::database()
{
    //initilize the mysql fields
    sock = NULL;
    row = NULL;
    result = NULL;
    field = NULL;
}

/**************
* Purpose: connect to the appropriate database
* Precondition: host, database, user name and password
* Postcondition: connected to that database
**************/
int database::openConnection(const char *host, const char *db, const char *user, const char *pass)
{
    if (sock) //already connected to another database
        disconnect(); //disconnect from that one

    //initilize the socket
    sock = mysql_init(0);

    //something went wrong with the socket
    if (!sock)
        return ERR201;

    //try connecting to the database
    if (!mysql_real_connect(sock, host, user, pass, db, 0, NULL, 0))
        return ERR202;

    //successfully connected to the database
    return SUCCESS;
}

/**************
* Purpose: disconnect from the database
* Precondition: none
* Postcondition: socket closed
**************/
bool database::disconnect()
{
    if (sock) //they have a socket open
        mysql_close(sock);

    //release result data
    free();

    //database disconnected
    return true;
}

/**************
* Purpose: free the results from the database query
* Precondition: none
* Postcondition: results no longer there
**************/
bool database::free()
{
    if (result)
    {
        mysql_free_result(result);
        return true;
    }

    return false;
}

/**************
* Purpose: return the approprite error message
* Precondition: error code
* Postcondition: string with the error returned
**************/
static string suc_str = "SUCCESS";
static string e201_str = "201 SOCKET ERROR: SOCKET FAILURE";
static string e202_str = "202 CONNECTION ERROR: CANNOT ACCESS THE SERVER";
static string e203_str = "203 DATABASE ERROR: QUERY FAILED";
const char *dberror(int errorcode)
{
    //display the appropriate error message for this error
    switch(errorcode)
    {
        case SUCCESS: return suc_str.c_str();
        break;

        case ERR201: return e201_str.c_str();
        break;

        case ERR202: return e202_str.c_str();
        break;

        case ERR203: return e203_str.c_str();
        break;
    }

    return NULL; //no error, return null char
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: result set returned (or null)
**************/
MYSQL_RES *database::query(const char *query)
{
    //query the database
    mysql_query(sock, query);

    //store the results
    result = mysql_store_result(sock);

    return result;
}

vector< vector<string> > database::sql_query(const char* sql_string)
{
    vector< vector<string> > sql_result;
    int query_state = mysql_query(sock, sql_string );
    if (!query_state)
    {
        result = mysql_store_result(sock);
        int num_fields;
        int i;
        num_fields = mysql_num_fields(result);
        while ( ( row = mysql_fetch_row(result)) != NULL )
        {
            vector<string> tmp;
            for (i=0; i < num_fields; i++)
            {
                if (row[i] != NULL)
                {
                    tmp.push_back(row[i]);
                }else{
                    tmp.push_back("-1");
                }
            }
            sql_result.push_back(tmp);
        }
        return sql_result;
    }
    else
    {
        std::vector< std::vector< std::string > > dummy;
        std::vector< std::string > tmp;
        tmp.push_back("NULL");
        dummy.push_back(tmp);
        cout << query_state << endl;
        return dummy;
    }
}


/**************
* Purpose: update the database no result returned
* Precondition: the query string
* Postcondition: false if failed, true if suceess
**************/
bool database::updateQuery(const char *query)
{
    if (!mysql_query(sock, query))
        return 0; //failed query
    else
        return 1; //successful query
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: the FIRST result is returned (or null)
*                will not return multiple rows, only the first
**************/
char *database::stringQuery(const char *query)
{
    //if old results exist, free them
    //free();

    //query the database
    mysql_query(sock,query);

    //store the results
    result = mysql_store_result(sock);

    if (!result)
        return NULL; //no results

    //fetch the row
    row = mysql_fetch_row(result);

    //store the result & convert it to a number
    char *stringResult = row[0];

    //free the results
    free();

    return stringResult;
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: the FIRST result is returned (or null)
*                will not return multiple rows, only the first
**************/
int database::intQuery(const char *query)
{
    //query the database
    mysql_query(sock,query);

    //store the results
    result = mysql_store_result(sock);

    if (!result)
        return -1; //no results

    //fetch the row
    row = mysql_fetch_row(result);

    //store the result & convert it to a number
    //int id = atoi(row[0]);
    int id = convertString(row[0]);

    //free the results
    free();

    return id; //return the id number
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: the FIRST result is returned (or null)
*                will not return multiple rows, only the first
**************/
bool database::boolQuery(const char *query)
{
    //query the database
    mysql_query(sock, query);

    //store the results
    result = mysql_store_result(sock);

    return (bool)row[0];
}

int database::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}
