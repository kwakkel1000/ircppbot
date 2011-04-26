//
//
//  @ Project : ircppbot
//  @ File Name : Database.h
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


#ifndef SRC_INCLUDE_CORE_DATABASE_H_
#define SRC_INCLUDE_CORE_DATABASE_H_

#include <mysql/mysql.h>
#include <string>
#include <vector>

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
        // class constructor
        database();

        // connection & disconnect
        int       openConnection(const char *host, const char *db, const char *user, const char *pass);  // connect to the database
        bool      disconnect();  // disconnect from the database
        bool      connected();

        // querying
        MYSQL_RES *query(const char *query);  // return result set
        char      *stringQuery(const char *query);  // return string
        int       intQuery(const char *query);  // return an integer
        bool      boolQuery(const char *query);  // return a boolean
        bool      updateQuery(const char *query);  // updates, returns true if update went through
        std::vector< std::vector< std::string> > sql_query(const char* sql_string);
        // free the query results
        bool free();

    private:
        MYSQL *sock;
        MYSQL_RES *result;
        MYSQL_ROW row;
        MYSQL_FIELD *field;
        char *qbuf[MAX_QUERY_LEN];
        int convertString(std::string data);
};

const char *dberror(int errorcode);  // return a string for this error message

#endif  // SRC_INCLUDE_CORE_DATABASE_H_
