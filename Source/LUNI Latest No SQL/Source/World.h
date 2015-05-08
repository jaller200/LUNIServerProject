#pragma once

#include "Common.h"
#include <iostream>

enum ZoneId : ushort { 
	VENTURE_EXPLORER = 1000, VENTURE_EXPLORER_RETURN = 1001,
	AVANT_GARDENS = 1100,
	NIMBUS_STATION = 1200, NIMBUS_ROCK = 1250, NIMBUS_ISLE = 1251,
	PET_COVE = 1201,
	GNARLED_FOREST = 1300,
	FORBIDDEN_VALLEY = 1400,
	STARBASE_3001 = 1600,
	DEEP_FREEZE = 1601,
	ROBOT_CITY = 1602,
	MOON_BASE = 1603,
	PORTABELLO = 1604,
	CRUX_PRIME = 1800,
	NEXUS_TOWER = 1900,
	NINJAGO_MONASTERY = 2000
};

//prints zone name
std::ostream& operator<<(std::ostream& out, const ZoneId value);

struct World {
	ZoneId zone;
};