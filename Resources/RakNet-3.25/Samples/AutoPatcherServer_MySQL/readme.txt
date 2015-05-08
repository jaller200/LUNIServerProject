Project: Test of the Autopatcher Server, implemented using MySQL for the database

Description: This is a sample implementation of the autopatcher server. It's actually complete enough to use if you wish.

Dependencies: MySQL version 5.0 (http://dev.mysql.com/downloads/mysql/5.0.html). Dlls should exist with the .exe. See the post-build process which copies copy "C:\Program Files\MySQL\MySQL Server 5.0\lib\opt\*.dll" .\Release

Other requirements:
1. Run "CREATE DATABASE myDatabaseName" first in the adminstrator tools. Enter myDatabaseName when prompted "Enter DB schema:" from the C++ sample.
2. In MySQL Administrator, in the advanced networking tab, check max packet size, and change it to 1000M. Then restart the service through service control.

Related projects: AutopatcherClientRestarter, AutopatcherMySQLRepository, AutopatcherServer

For help and support, please visit http://www.jenkinssoftware.com
