#include "Database.h"
using namespace std;

MYSQL * Database::_con = NULL; // Set MYSQL connection variable to NULL

// Connect to the MySQL database. Requires the host, database, DB user, and DB password to connect
void Database::Connect(const string& host, const string& database, const string& username, const string& password) {
	_con = mysql_init(NULL); //init sql connection
	if (!_con) throw MySqlException("Can't start mysql service!\n");

	_con = mysql_real_connect(_con, host.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0); //connection to luni database
	if (!_con) throw MySqlException("Unable to connect to database!\n");
}

// Query the MySQL database for a specific string
MYSQL_RES * Database::Query(const string & query) {
	if (_con == NULL) throw MySqlException("Database connection error!\n");
	mysql_query(_con, (char*)query.c_str());
	return mysql_store_result(_con);
}