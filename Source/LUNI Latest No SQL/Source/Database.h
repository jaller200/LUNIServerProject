#pragma once

#include <string>
#include <my_global.h> // If you have in settings
#include <mysql.h>

class MySqlException : public std::exception {
	public:
	MySqlException() : std::exception("mysql error") {}
	MySqlException(const std::string& msg) : std::exception(msg.c_str()) {}
};

class Database {
	private:
		static MYSQL * _con;
		Database() {}
	public:
		// Initialize connection. TODO add server, database, user and password as parameters and read form config file
		static void Connect(const std::string& host, const std::string& database, const std::string& username, const std::string& password);
		// Make sql query on db and return result
		static MYSQL_RES * Query(const std::string & query); 
};