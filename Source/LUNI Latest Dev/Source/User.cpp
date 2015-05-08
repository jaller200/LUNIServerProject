#include "User.h"
#include "Database.h"

#include "UsersPool.h"

#include <sstream>
using namespace std;

// This is the user initalization
User::User(uint id, const string& username, const SystemAddress& systemAddress, UserSuccess loginStatus, uchar numChars, uchar frontChar) {
	this->id = id; // Store User ID
	this->username = username; // Store Username
	ip = systemAddress; // Store systemAddress
	for (int i = 0; i < 4; i++) characters[i] = NULL; // Initialize characters
	current = NULL; // Initialize current character
	successState = loginStatus; // Set the success state
	userChars = numChars; // Set the number of user characters
	userFrontChar = frontChar; // Set the user front char

	// Print the success state to the console
	cout << successState << endl;

	ostringstream osid;
	osid << id;

	auto qr = Database::Query("SELECT * FROM `characters` WHERE `accountID` = " + osid.str() + " LIMIT 4"); // Load chars from MySQL DB
	MYSQL_ROW row;
	int i = 0;
	while (row = mysql_fetch_row(qr)) {
		stringstream s(row[0]);
		uint cid;
		s >> cid;
		characters[i] = cid;
		i++;
	}
}

// Initialize user with another user's data
User::User(const User& usr) {
	memcpy(this, &usr, sizeof(User));
}

// Load the characters
vector< Ref<Character> > User::LoadCharacters() {
	vector< Ref<Character> > r(4);
	for (int i = 0; i < 4; i++) r[i] = Ref<Character>( new Character(characters[i]) );
		//(characters[i] != 0) ? unique_ptr<Character>(new Character(characters[i])) : NULL;
	return r;
}

// Login a user
Ref<User> User::Login(const string& nikname, const string& password, const SystemAddress& systemAddress) {
	UserSuccess currentLoginStatus = UserSuccess::SUCCESS;

	auto qr = Database::Query("SELECT `id`, `password`, `rank`, `locked`, `banned`, `loginTries`, `numChars`, `frontChar` FROM `accounts` WHERE `name` = '" + nikname + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) {
		cout << "NOT FOUND!!"; 
		currentLoginStatus = UserSuccess::INVALID_USER;
		return NULL; 
	}

	auto ftc = mysql_fetch_row(qr);

	// This converts the loginTries in the database to an int value
	string loginTriesStr = ftc[5];
	int loginTries = atoi(loginTriesStr.c_str());

	if (password != ftc[1] && loginTries != 3) {
		currentLoginStatus = UserSuccess::INVALID_PASS;	// Set the loginStatus to INVALID_PASS
		loginTries++; // Increase the loginTries

		// If login tries equals 3, update database accordingly
		if (loginTries == 3) {
			Database::Query("UPDATE `accounts` SET `locked` = '1' WHERE `name` = '" + nikname + "';"); // Change account status to 'locked'
			Database::Query("UPDATE `accounts` SET `loginTries` = '3' WHERE `name` = '" + nikname + "';"); // Make sure 'loginTries = 3' so that we know they are locked
			currentLoginStatus = UserSuccess::LOCKED; // Finally, change currentStatus to LOCKED
		}
		else {
			// If login was wrong, make sure loginTries (which was increased above) is set in database
			Database::Query("UPDATE `accounts` SET `loginTries` = '" + to_string(loginTries) + "' WHERE `name` = '" + nikname + "';");
		}
	} else {
		Database::Query("UPDATE `accounts` SET `loginTries` = '0' WHERE `name` = '" + nikname + "';");

		// For some reason, ftc[4] = "1" doesn't work, but converting "1" into a string does... Hmmm....
		// Need to fix this
		string banned = "1";

		// Simple check to see if account is banned. If it is, return BANNED (for client to show)
		if (ftc[4] == banned) {
			cout << "Banned!";
			currentLoginStatus = UserSuccess::BANNED;
		}
	}

	// If account is locked, whether user entered in correct password or not,
	// Show this (because each username is unique)
	if (atoi(ftc[3]) == 1) {
		cout << "Account Locked";

		// Change currentStatus to LOCKED
		currentLoginStatus = UserSuccess::LOCKED;
	}

	int currentNumChars = atoi(ftc[6]);

	if (currentNumChars > 4) {
		currentNumChars = 0;
	}

	int currentFrontChar = atoi(ftc[7]);

	if (currentFrontChar > 3) {
		currentFrontChar == 0;
	}

	cout << "Current Front Char is: " << currentFrontChar << endl;

	cout << "Current Number of Chars is: " << currentNumChars << endl;

	// Print current status to console (just to check, to delete later)
	cout << "Current login status is: " << currentLoginStatus << endl;

	stringstream s(ftc[0]);
	uint uid;
	s >> uid;
	
	return Ref<User>( new User(uid, nikname, systemAddress, currentLoginStatus, currentNumChars, currentFrontChar) );
}

// Check to see if a User ID exists by querying the database
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