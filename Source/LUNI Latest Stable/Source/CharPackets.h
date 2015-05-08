#pragma once

#include <limits>
#include <iostream>
#include <fstream>

#include "Common.h"
#include "Packet.h"
#include "User.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\MessageIdentifiers.h"
#include "RakNet\BitStream.h"

using namespace std;

// Struct that puts names to each pants ID (for easy use)
enum CharCreatePantsColor : ulong {
	PANTS_BRIGHT_RED = 2508,
	PANTS_BRIGHT_ORANGE = 2509,
	PANTS_BRICK_YELLOW = 2511,
	PANTS_MEDIUM_BLUE = 2513,
	PANTS_SAND_GREEN = 2514,
	PANTS_DARK_GREEN = 2515,
	PANTS_EARTH_GREEN = 2516,
	PANTS_EARTH_BLUE = 2517,
	PANTS_BRIGHT_BLUE = 2519,
	PANTS_SAND_BLUE = 2520,
	PANTS_DARK_STONE_GRAY = 2521,
	PANTS_MEDIUM_STONE_GRAY = 2522,
	PANTS_WHITE = 2523,
	PANTS_BLACK = 2524,
	PANTS_REDDISH_BROWN = 2526,
	PANTS_DARK_RED = 2527
};

// Struct that puts names to each base shirt ID (for easy use)
enum CharCreateShirtColor : ulong {
	SHIRT_BRIGHT_RED = 4049,
	SHIRT_BRIGHT_BLUE = 4083,
	SHIRT_BRIGHT_YELLOW = 4117,
	SHIRT_DARK_GREEN = 4151,
	SHIRT_BRIGHT_ORANGE = 4185,
	SHIRT_BLACK = 4219,
	SHIRT_DARK_STONE_GRAY = 4253,
	SHIRT_MEDIUM_STONE_GRAY = 4287,
	SHIRT_REDDISH_BROWN = 4321,
	SHIRT_WHITE = 4355,
	SHIRT_MEDIUM_BLUE = 4389,
	SHIRT_DARK_RED = 4423,
	SHIRT_EARTH_BLUE = 4457,
	SHIRT_EARTH_GREEN = 4491,
	SHIRT_BRICK_YELLOW = 4525,
	SHIRT_SAND_BLUE = 4559,
	SHIRT_SAND_GREEN = 4593
};

// Struct to hold all char data
struct CharactersPacket {
	uchar charactersLength; // This is the number of characters the user has from 1 - 4
	uchar frontChar; // This is the character in front

	// -- BEGIN CHAR DATA -- //
	unsigned long long int objectID; // This is the Character ID - For now, we assign it 0 - 3 (linked to acct ID)
	ushort unknown1;
	wstring charName; // This is the name of the char (null terminated)... Should be wstring (so you can only use 65 bytes)
	wstring unapprovedName; // Similar to above... null-terminated and should be wstring
	uchar nameRejected; // 0 if approved, 1 if rejected
	uchar freeToPlay; // 0 if premium, 1 if free to play
	string unknown2; // This should be 10 bytes long
	ulong shirtColor; // The shirt color
	ulong shirtStyle; // Could this be something to do with the shirt? - Possibly shirt style...
	ulong pantsColor; // Pants color
	ulong hairStyle; // Hair style
	ulong hairColor; // Hair color
	ulong lh; // Dunno what this is...
	ulong rh; // Dunno what this is...
	ulong eyebrows; // Eyebrows
	ulong eyes; // Eyes
	ulong mouth; // Mouth
	ulong unknown4; // Dunno what this is either...
	ushort lastZoneId; // Last Zone ID
	ushort mapInstance; // Most likely the map instance, so I am naming it that here
	ulong mapClone; // The map clone (from the "53 05 00 02" packet structure)
	ulonglong lastLogout; // Last logout in seconds

	ushort equippedItemsLength; // The number of items to follow

	CharactersPacket() { } // Initalizer does nothing right now

	// Function to write the data to bitstream.
	void WriteDataToBitstream(RakNet::BitStream *bitStream, uint charId) {
		cout << sizeof(unsigned long long int) << endl;

		cout << "Fetching character..." << endl;
		cout << "Character name is: " << charName.c_str() << endl;
		cout << "Unapproved name is: " << unapprovedName.c_str() << endl;
		cout << "Name Rejected?: " << nameRejected << endl;
		cout << "Free to play?: " << freeToPlay << endl;
		cout << "Shirt color: " << shirtColor << endl;
		cout << "Shirt style: " << shirtStyle << endl;
		cout << "Pants color is: " << pantsColor << endl;
		cout << "Hair style is: " << hairStyle << endl;
		cout << "Hair color is: " << hairColor << endl;
		cout << "Eyebrows is: " << eyebrows << endl;
		cout << "Eyes are: " << eyes << endl;
		cout << "Mouth is: " << mouth << endl;

		
		ushort z = 0;

		bitStream->Write(objectID);
		bitStream->Write(unknown1);
		bitStream->Write(z);

		bitStream->Write((char*)charName.data(), sizeof(wchar_t) * charName.size() / 2);
		bitStream->Write((char*)unapprovedName.data(), sizeof(wchar_t) * unapprovedName.size() / 2);
		
		bitStream->Write(nameRejected);
		bitStream->Write(freeToPlay);
		
		uchar zeroChar = 0;
		uchar oneChar = 1;
		for (int i = 0; i < 10; i++) {
			if (i == 1 && charId == 0) {
				bitStream->Write(oneChar);
			}
			else {
				bitStream->Write(zeroChar);
			}
		}

		bitStream->Write(shirtColor);
		bitStream->Write(shirtStyle);
		bitStream->Write(pantsColor);
		bitStream->Write(hairStyle);
		bitStream->Write(hairColor);
		bitStream->Write(lh);
		bitStream->Write(rh);
		bitStream->Write(eyebrows);
		bitStream->Write(eyes);
		bitStream->Write(mouth);
		bitStream->Write(unknown4);
		bitStream->Write(lastZoneId);
		bitStream->Write(mapInstance);
		bitStream->Write(mapClone);
		bitStream->Write(lastLogout);
	}
};

// This sends the packet known as char_aw2.bin to the client
void SendCharPacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> user);

// Sends the deletion packet to the client
void SendDeletePacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> user, uchar *packetData, uint length);

// This will get the unapproved name and return it as a std::string
string GetUnapprovedUsername(ulong firstLine, ulong middleLine, ulong lastLine);

// Go to and read the line in a txt file
fstream& GoToLine(fstream& file, ulong num);

// This will add the created character to the SQL database
void AddCharToDatabase(RakPeerInterface *rakServer, SystemAddress& systemAddress, uchar *packetData, uint length, Ref<User> usr);

// Store the character in database based on charId and userId
void StoreCharInDatabase(uint usrId, uchar charId);

// Figure out the items the character is wearing / was created with
ulong FindCharShirtID(ulong shirtColor, ulong shirtStyle);
ulong FindCharPantsID(ulong pantsColor);

// Figure out how many special items the user has and equip any
// that need to be equipped
void GetCharSpecialItems(ulonglong objectID, RakNet::BitStream *bitStream);