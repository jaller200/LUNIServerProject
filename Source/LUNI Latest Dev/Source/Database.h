#pragma once

#include <string>

#include <my_global.h> // If you have included MySQL in Project Settings
#include <mysql.h> // If you have included MySQL in Project Settings

// This is a MySQL Exception subclasses from std::exception
class MySqlException : public std::exception {
	public:
	MySqlException() : std::exception("mysql error") {}
	MySqlException(const std::string& msg) : std::exception(msg.c_str()) {}
};

// This is the declaration for methods that will be used to connect to
// MySQL database
class Database {
	private:
		static MYSQL * _con; // This is the MySQL Connection
		Database() {} // Private Database initializer
	public:
		// Initialize connection. TODO add server, database, user and password as parameters and read form config file
		// This takes the host, the database, the DB username, and the DB password as strings and connects the server to
		// the MySQL database
		static void Connect(const std::string& host, const std::string& database, const std::string& username, const std::string& password);

		// Query the MySQL database and return results
		static MYSQL_RES * Query(const std::string & query); 
};