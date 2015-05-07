#pragma once

#include "RakNet\MessageIdentifiers.h"

// put all packet specific information here like structs, enums, etc..
// dont know the real names but that doesn't matter as long as they serve their purpose

// "main" ID, first byte of the packet data

enum{
	ID_LEGO_PACKET = ID_USER_PACKET_ENUM,	// all lego packets that don't do RakNet stuff have this main ID
	LUNI_INITAW = 0,
	LUNI_LOGIN = 1,
	LUNI_CHARACTER_CHARSDATA = 1,

	LUNI_CHARACTER = 4,
	LUNI_CHARACTER_CREATEDNEW = 3,
	LUNI_CHARACTER_DELETED = 6,

	LUNI_WORLD_IN = 19,
	LUNI_WORLD_CHAT = 25
	// other unknown ID's didn't appear (yet?)
};

// second and fourth byte seem to serve as ID's as well, see inside server loop for more comments