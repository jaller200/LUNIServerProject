#include "User.h"
#include "Database.h"
#include <sstream>
using namespace std;

User::User(uint id, const string& username, const SystemAddress& systemAddress) {
	this->id = id;
	this->username = username;
	ip = systemAddress;
	for (int i = 0; i < 4; i++) characters[i] = NULL;
	current = NULL;

	ostringstream osid;
	osid << id;

	#ifndef LUNI_NO_AUTH
	auto qr = Database::Query("SELECT * FROM `characters` WHERE `id_acc` = " + osid.str() + " LIMIT 4"); //load characters
	MYSQL_ROW row;
	int i = 0;
	while (row = mysql_fetch_row(qr)) {
		stringstream s(row[0]);
		uint cid;
		s >> cid;
		characters[i] = cid;
		i++;
	}
	#endif
}

User::User(const User& usr) {
	memcpy(this, &usr, sizeof(User));
}

vector< Ref<Character> > User::LoadCharacters() {
	vector< Ref<Character> > r(4);
	for (int i = 0; i < 4; i++) r[i] = Ref<Character>( new Character(characters[i]) );
		//(characters[i] != 0) ? unique_ptr<Character>(new Character(characters[i])) : NULL;
	return r;
}

Ref<User> User::Login(const string& nikname, const string& password, const SystemAddress& systemAddress) {
	#ifndef LUNI_NO_AUTH
		auto qr = Database::Query("SELECT `id`, `password`, `rank` FROM `accounts` WHERE `name` = '" + nikname + "' LIMIT 1;");
		if (mysql_num_rows(qr) == 0) return NULL; //no user with username found

		auto ftc = mysql_fetch_row(qr);
		if (password != ftc[1]) return NULL; //wrong password
		stringstream s(ftc[0]);
		uint uid;
		s >> uid;
	#else
		uint uid = 0;
	#endif
	return Ref<User>( new User(uid, nikname, systemAddress) );
}

uint User::FindUserID(const string& nikname) {
	auto qr = Database::Query("SELECT `id` FROM `accounts` WHERE `name` = '" + nikname + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) return 0;
	auto ftc = mysql_fetch_row(qr);
	istringstream o(ftc[0]);
	uint r;
	o >> r;
	return r;
}

uint User::GetID() { return id; }
Ref<Character> User::GetCurrentCharacter() { return current; }
SystemAddress User::GetIp() { return ip; }
std::string User::GetUsername() { return username; }

User::destructor User() { current = NULL; }