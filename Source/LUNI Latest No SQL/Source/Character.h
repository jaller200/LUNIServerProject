#pragma once

#include "Common.h"
#include "World.h"

//#define MAXCHARNAMELEN 32

struct MinifigureData {
	uint shirtcolor, shirtstyle, pants, hairstyle, haircolor, unknow2, unknow3, eyebrows, eyes, mouth;

	MinifigureData() {
		memset(this, 0, sizeof(MinifigureData));
	}
};

struct CharacterData {
	MinifigureData minifig;
	uchar unknow4[6]; 
	ZoneId travelling;
	uchar unknow5[8]; // x, y ??? z may not be needed... because the client can calculate that
	uchar unknowseq[4]; // 23 / 24 / 25 4f 00 00
	uint eq_specialitem, eq_handleft, eq_handright, eq_backpack, eq_pants, eq_shirt, eq_headgear; //equipment

	CharacterData() {
		memset(this, 0, sizeof(CharacterData)); //to be deleted next
		unknowseq[0] = 0x23; // 23 / 24 / 25
		unknowseq[1] = 0x4f;
		//unknowseq[2] = unknowseq[3] = 0x00; //useless thanks to the zero memset ^
	}
};

struct Character {
	std::vector< uchar > Name;
	CharacterData Data;

	Character() {}

	//load character from database
	Character(uint id);

	//loads char form packet
	//packet will start with charname, no message ids please!
	Character(const uchar* packet);

	uint GetGeneratedPacketSize();

	//use to generate Packet
	uchar* Serialize();

	std::string GetName();
};

typedef Character CharacterCreation;