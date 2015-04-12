#include "Character.h"
#include "Database.h"
#include <sstream>
using namespace std;

// This is the character initialization by the Character ID
Character::Character(uint charid) {
	uint id = charid; // This is the Character ID

	stringstream o; // This is a stringstream that stores information
	o << id; // Add the Character ID to the stringstream 'o'
	string qr = "SELECT `name`, `lastZoneId`, `x`, `y`, `z` FROM `characters` WHERE `objectID` = " + o.str(); // Create the MySQL query string
	auto row = mysql_fetch_row(Database::Query(qr)); // Query the MySQL database for user

	// name = row[2] + '0';
	for (uint i = 0; i < strlen(row[0]); i++) Name.push_back(row[0][i]); // Keep adding the next character to the Name
	Name.push_back(0); // Add a null terminator to the end of the name

	// Clear the stringstream
	o.clear();

	// Copy the world into the stringstream
	o << row[1];
	//o >> world;

	// Clear the stringstream again
	o.clear();

	// Copy the coordinates into the stringstream
	o << row[2] << row[3] << row[4];

	// Initialize the coordinates
	double _x, _y, _z;

	// Print out the coordinates
	o >> _x >> _y >> _z;
	//position = unique_ptr<Point3D>(new Point3D(_x, _y, _z));
}

// This is the character initialization by a packet
Character::Character(const uchar* packet) {
	// Initialize an int for counting
	int i = 0;

	// Reading name
	while (true) {
		// Add the next byte into the std::vector<uchar> Name
		Name.push_back(packet[i++]);

		// If the last two bytes are 0x00 0x00 (which means there is no
		// more characters in the name), then break from the loop
		if (packet[++i] == 0) break;
	}
	
	// Add the NULL terminator (0x00) to the end of the name
	Name.push_back(0);

	// Add one more to i
	i++;

	//cout << "\n Size: " << size << " CharacterData size: " << sizeof(CharacterData) << endl;

	// Copy the data into memory
	memcpy(&Data, packet + i, sizeof(CharacterData));
}

// Get the packet size (char name * 2 + the size of the character data)
uint Character::GetGeneratedPacketSize() { return Name.size() * 2 + sizeof(CharacterData); }

// Serialize the character
uchar* Character::Serialize() {
	// Allocate the size generated above
	uchar * r = (uchar*) malloc( GetGeneratedPacketSize() );

	// If r is equal to NULL, return NULL
	if (r == NULL) return NULL;

	// Initialize uint
	uint i = 0;

	// Copy the name into r
	for (uint j = 0; j < Name.size(); i++, j++) {
		r[i] = Name[j]; // Copy letter
		r[++i] = 0; // Next byte is 0x00 (the name is actually a wstring, not a std::string)
	}
	memcpy(r + i, &Data, sizeof(CharacterData)); // Copy the data into memory

	// Return the data as a uchar*
	return r;
}

// Get the character name
string Character::GetName() { return (char*) Name.data(); }