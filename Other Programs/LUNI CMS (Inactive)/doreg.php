<?
require("mysql.php");
Conect();
echo "<br>Username: ".$_POST['ctl00$ContentPlaceHolder1$TextBoxUserName']."<br>";
echo "Password: ".$_POST['ctl00$ContentPlaceHolder1$TextBoxPassword']."<br>";
if( mysql_num_rows(mysql_query("SELECT * FROM `accounts` WHERE `name` = '".$_POST['ctl00$ContentPlaceHolder1$TextBoxUserName']."'"))==0 ){
	mysql_query("INSERT INTO `accounts` VALUES (NULL , '".$_POST['ctl00$ContentPlaceHolder1$TextBoxUserName']."', '".$_POST['ctl00$ContentPlaceHolder1$TextBoxPassword']."');");
	echo "<br>Registration Sucess!";
}
else { echo "Account already exist! Go Back"; }
?>
