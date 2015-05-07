#pragma once

#include "Common.h"

#include <iostream>

// This is the FactionID enum that contains faction
// names and values
enum FactionId : ushort {
	SENTINAL = 0,
	ASSEMBLY,
	PARADOX,
	VENTURE_LEAGUE
};

// Prints Faction name
std::ostream& operator<<(std::ostream& out, const FactionId value);

// This is the Faction struct that stores a FactionId
struct Faction {
	FactionId faction;
};