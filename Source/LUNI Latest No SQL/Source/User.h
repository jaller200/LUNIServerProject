#pragma once

#include "Common.h"
#include "Character.h"
#include "RakNet\RakNetTypes.h"

class User {
	private:
		uint id;
		std::string username;
		uint characters[4]; //array of User characters id
		Ref<Character> current; //current char
		SystemAddress ip;

		User(uint id, const std::string& username, const SystemAddress& systemAddress);
	public:
		uint numredir = 1;
		ulong nextcid = 0; //only for development

		User(const User& usr);
		//return a vector of char for the selection screen
		std::vector< Ref<Character> > LoadCharacters();

		// login, return users. NULL if fails to login
		static Ref<User> Login(const std::string& nikname, const std::string& password, const SystemAddress& systemAddress);

		//find user id for user with a specifi username. 0 if not founded
		static uint FindUserID(const std::string& nikname);

		uint GetID();
		Ref<Character> GetCurrentCharacter();
		SystemAddress GetIp();
		std::string GetUsername();

		destructor User();
};