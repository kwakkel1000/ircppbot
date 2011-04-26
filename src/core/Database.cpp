//
//
//  @ Project : ircppbot
//  @ File Name : Database.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include <iostream>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
/**************
* File Name: database.cpp
* Author: Jade@design1online.com
* Purpose: mysql database class
**************/
#include "../include/core/Database.h"
/**************
* Purpose: default constructor, initialize class values
* Precondition: none
* Postcondition: none
**************/
database::database()
{
    // initilize the mysql fields
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
    if (sock)  // already connected to another database
        disconnect();  // disconnect from that one

    // initilize the socket
    sock = mysql_init(0);

    // something went wrong with the socket
    if (!sock)
        return ERR201;

    // try connecting to the database
    if (!mysql_real_connect(sock, host, user, pass, db, 0, NULL, 0))
        return ERR202;

    // successfully connected to the database
    return SUCCESS;
}

/**************
* Purpose: disconnect from the database
* Precondition: none
* Postcondition: socket closed
**************/
bool database::disconnect()
{
    if (sock)  // they have a socket open
        mysql_close(sock);

    sock = NULL;

    // release result data
    free();

    // database disconnected
    return true;
}

/************
*return true if connected
************/
bool database::connected()
{
    if (sock)
    {
        return true;
    }
    return false;
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
static std::string suc_str = "SUCCESS";
static std::string e201_str = "201 SOCKET ERROR: SOCKET FAILURE";
static std::string e202_str = "202 CONNECTION ERROR: CANNOT ACCESS THE SERVER";
static std::string e203_str = "203 DATABASE ERROR: QUERY FAILED";
const char *dberror(int errorcode)
{
    // display the appropriate error message for this error
    switch (errorcode)
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

    return NULL;  // no error, return null char
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: result set returned (or null)
**************/
MYSQL_RES *database::query(const char *query)
{
    // query the database
    mysql_query(sock, query);

    // store the results
    result = mysql_store_result(sock);

    return result;
}

std::vector< std::vector< std::string > > database::sql_query(const char* sql_string)
{
    std::vector< std::vector< std::string > > sql_result;
    int query_state = mysql_query(sock, sql_string);
    if (!query_state)
    {
        result = mysql_store_result(sock);
        int num_fields;
        int i;
        num_fields = mysql_num_fields(result);
        while ( ( row = mysql_fetch_row(result)) != NULL )
        {
            std::vector< std::string > tmp;
            for (i = 0; i < num_fields; i++)
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
        std::cout << query_state << std::endl;
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
        return 0;  // failed query
    else
        return 1;  // successful query
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: the FIRST result is returned (or null)
*                will not return multiple rows, only the first
**************/
char *database::stringQuery(const char *query)
{
    // if old results exist, free them
    // free();

    // query the database
    mysql_query(sock, query);

    // store the results
    result = mysql_store_result(sock);

    if (!result)
        return NULL;  // no results

    // fetch the row
    row = mysql_fetch_row(result);

    // store the result & convert it to a number
    char *stringResult = row[0];

    // free the results
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
    // query the database
    mysql_query(sock, query);

    // store the results
    result = mysql_store_result(sock);

    if (!result)
        return -1;  // no results

    // fetch the row
    row = mysql_fetch_row(result);

    // store the result & convert it to a number
    // int id = atoi(row[0]);
    int id = convertString(row[0]);

    // free the results
    free();

    return id;  // return the id number
}

/**************
* Purpose: return the result set of the query
* Precondition: the query string
* Postcondition: the FIRST result is returned (or null)
*                will not return multiple rows, only the first
**************/
bool database::boolQuery(const char *query)
{
    // query the database
    mysql_query(sock, query);

    // store the results
    result = mysql_store_result(sock);

    return (bool)row[0];
}

int database::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);
    ss >> i;
    return i;
}
