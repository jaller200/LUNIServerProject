<?
define("SQL_SERVER", "localhost");
define("SQL_USER", "root");
define("SQL_PASS", "universe");
define("SQL_DB", "universe");

function Conect() {
		mysql_connect(SQL_SERVER, SQL_USER, SQL_PASS);
		mysql_select_db(SQL_DB);
}
?>