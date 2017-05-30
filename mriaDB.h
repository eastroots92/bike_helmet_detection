#pragma once

#include <iostream>
#include <string>
#include "mysql\mysql.h"

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS "maria"
#define DB_NAME "MHLDB"
#define DB_PORT 3306
using namespace std;


class MHLdata
{
private:
	MYSQL *connection;
	MYSQL conn;
	MYSQL_RES *sql_result;
	MYSQL_ROW sql_row;
	int query_stat;
	int query_read;
	string no;
public:
	MHLdata();
public:
	void isInsertQuery(string no, string imgname, string inputname, string datapath, string licenseno);
	string isSelectQuery();
public:
	MYSQL *getConnection() { return connection; }
};
