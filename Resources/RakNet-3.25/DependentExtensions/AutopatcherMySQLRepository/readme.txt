Project: Autopatcher Server, implemented using MySQL for the database
Description: Provides patch information and asynchronous database queries to AutopatcherClient

Dependencies: MySQL version 5.0 (http://dev.mysql.com/downloads/mysql/5.0.html), installed to C:\Program Files\MySQL\. If your installation directory is different, update it at
C/C++ / General / Additional Include Directories
and under
Build Events / Post Build Events
copy "C:\Program Files\MySQL\MySQL Server 5.0\lib\opt\*.dll" .\Release
to whatever your path is.

Related projects: AutopatcherClientRestarter, AutopatcherMySQLRepository, AutopatcherServer

For help and support, please visit http://www.jenkinssoftware.com
