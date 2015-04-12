#pragma once

#include "Common.h"
#include "World.h"

// This was used to define the length of the Character Name (as it is 32 sets of two bits (basically 64 bytes)
// in the char_aw2.bin
//#define MAXCHARNAMELEN 32

// This is the MinifigureData struct. It will hold all Minifigure Information from the server database
// and eventually send it to the client
struct MinifigureData {
	// Minifigure data struct
	uint shirtcolor, shirtstyle, pants, hairstyle, haircolor, unknow2, unknow3, eyebrows, eyes, mouth;

	// Initializer... Set memory size equal to the sizeof the data
	MinifigureData() {
		memset(this, 0, sizeof(MinifigureData));
	}
};

// This is the CharacterData struct. It holds all the information of the Character (in char_w2.bin)
struct CharacterData {
	MinifigureData minifig; // This is the Minifigure
	uchar unknow4[4]; // This an unknown sequence of 6 bytes (What I call a "byte" here is actually two bits... Just to be clear :) )
	ZoneId lastZone; // This is the last zone that the character was in
	ZoneId travelling; // This is the world that the user is travelling to, wrapped in a ZoneId
	
	// This data has sorta been discovered by devs... changed below
	//uchar unknow5[8]; // x, y ??? z may not be needed... because the client can calculate that - This is unknown data
	//uchar unknowseq[4]; // 23 / 24 / 25 4f 00 00 - This is unknown data

	uchar mapClone[4]; // This is the map clone
	uchar timestamp[8]; // This is possibly the timestamp

	uint eq_specialitem, eq_handleft, eq_handright, eq_backpack, eq_pants, eq_shirt, eq_headgear; // This is the equipment of the character

	// Character initialization - Allocate and store the memory of the Character
	CharacterData() {
		memset(this, 0, sizeof(CharacterData)); //to be deleted next
		//unknowseq[0] = 0x23; // 23 / 24 / 25
		//unknowseq[1] = 0x4f;
		//unknowseq[2] = unknowseq[3] = 0x00; // Useless thanks to the zero memset ^
	}
};

// This is the Character struct, which brings together the CharacterData into a Character
struct Character {
	std::vector< uchar > Name; // This is the name of the Character
	CharacterData Data; // This is the CharacterData (see above struct)

	// Character Initialization
	Character() {}

	// Load character from database with Character ID
	Character(uint id);

	// Loads Character packet (char_aw2.bin)
	// Packet will start with charname, no message ids please!
	Character(const uchar* packet);

	// Get Generated Packet Size
	uint GetGeneratedPacketSize();

	// Use to generate packets
	uchar* Serialize();

	// Get Character name
	std::string GetName();
};

// This is a typedef of Character. Basically, instead of using "Character",
// you can use "CharacterCreation".
typedef Character CharacterCreation;