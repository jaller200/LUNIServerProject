#pragma once

#include "Common.h"

#include "RakNet\RakNetTypes.h"

class Character;

// This is a struct that provides names for User login success values
enum UserSuccess : char {
	SUCCESS = 0x01,
	BANNED,
	PERMISSION_LOW,
	UNKNOWN1,
	UNKNOWN2,
	INVALID_PASS,
	LOCKED,
	INVALID_USER,
	ACTIVATION_PENDING,
	ACCOUNT_DISABLED,
	GAME_TIME_EXPIRED,
	FREE_TRIAL_ENDED,
	PLAY_SCHEDULE_NOT_ALLOWED,
	ACCOUNT_NOT_ACTIVATED
};

class User {
	private:
		uint id; // This is the ID of the Users
		std::string username; // This is the username of User
		uint characters[4]; // Array of User Characters IDs
		SystemAddress ip; // This is the SystemAddress from RakNet 

		Ref<Character> current; // This is the current character of the player

		// This is the User initialization. It takes a User ID, a Username, and an SystemAddress
		User(uint id, const std::string& username, const SystemAddress& systemAddress, UserSuccess loginStatus, uchar numChars, uchar frontChar);
	public:
		uint numredir = 1; // Number of redirects
		ulong nextcid = 0; // Only for development - this is the next character ID (CID = Character ID)

		UserSuccess successState; // This is the success state of the user login (stores locally)
		uchar userChars; // This is the number of characters the user has (stores locally)
		uchar userFrontChar; // This is the front character of the user (stores locally)
		uchar nameInUse = 0; // Whether or not the selected username / predefined name is in use (stores locally)

		// Initializer
		User(const User& usr);

		// Return a vector of characters for the selection screen
		std::vector< Ref< Character > > LoadCharacters();

		// Login, return users. NULL if fails to login
		static Ref<User> Login(const std::string& nikname, const std::string& password, const SystemAddress& systemAddress);

		// Find User ID for user with a specific username. 0 if not founded
		static uint FindUserID(const std::string& nikname);

		uint GetID(); // Get the ID of the user
		Ref< Character > GetCurrentCharacter(); // Get the current character
		void SetCurrentCharacter(Ref< Character > character);
		SystemAddress GetIp(); // Get the current SystemAddress
		std::string GetUsername(); // Get the current Username

		destructor User(); // User destructor
};