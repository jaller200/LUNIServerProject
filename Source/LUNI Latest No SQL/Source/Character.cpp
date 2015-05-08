#include "Character.h"
#include "Database.h"
#include <sstream>
using namespace std;

Character::Character(uint charid) {
	uint id = charid;

	stringstream o;
	o << id;
	string qr = "SELECT * FROM `characters` WHERE `id` = " + o.str();
	auto row = mysql_fetch_row(Database::Query(qr));

	//name = row[2] + '0';
	for (uint i = 0; i < strlen(row[2]); i++) Name.push_back(row[2][i]);
	Name.push_back(0);

	o.clear();
	o << row[4];
	//o >> world;

	o.clear();
	o << row[5] << row[6] << row[7];
	double _x, _y, _z;
	o >> _x >> _y >> _z;
	//position = unique_ptr<Point3D>(new Point3D(_x, _y, _z));
}

Character::Character(const uchar* packet) {
	int i = 0;
	while (true) { //reading name
		Name.push_back(packet[i++]);
		if (packet[++i] == 0) break;
	}
	Name.push_back(0);
	i++;
	//cout << "\n Size: " << size << " CharacterData size: " << sizeof(CharacterData) << endl;
	memcpy(&Data, packet + i, sizeof(CharacterData));
}

uint Character::GetGeneratedPacketSize() { return Name.size() * 2 + sizeof(CharacterData); }

uchar* Character::Serialize() {
	uchar * r = (uchar*) malloc( GetGeneratedPacketSize() );
	if (r == NULL) return NULL;

	uint i = 0;
	for (uint j = 0; j < Name.size(); i++, j++) {
		r[i] = Name[j];
		r[++i] = 0;
	}
	memcpy(r + i, &Data, sizeof(CharacterData));

	return r;
}

string Character::GetName() { return (char*) Name.data(); }