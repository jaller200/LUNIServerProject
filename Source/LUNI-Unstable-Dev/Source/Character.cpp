#include "Character.h"
#include "serverLoop.h"

#include "RakNet\MessageIdentifiers.h"
#include "RakNet\ReplicaManager.h"
#include "RakNet\RakPeerInterface.h"

#include <cstdlib>

using namespace RakNet;

extern ReplicaManager replicaManager;
extern Character *character;
extern RakPeerInterface* rakServer;

Character::Character() {
	replicaManager.Construct(this, false, UNASSIGNED_SYSTEM_ADDRESS, false);
	replicaManager.DisableReplicaInterfaces(this, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE);
}

Character::Character(ulonglong objectID)
{
	stringstream query;
	query << "SELECT * FROM `characters` WHERE `objectID` = '" << objectID << "';";

	auto qr = Database::Query(query.str());
	if (mysql_num_rows(qr) == 0) {
		cout << "ERROR!!!! NO CHARACTER EXISTS WITH THAT OBJECT ID!!! CHECK DATABASE OR SERVER!!!" << endl;
	}

	auto ftc = mysql_fetch_row(qr);

	// Set the charName/ objectID
	charObjectID = objectID;
	charName = StringToWString((string)ftc[3], sizeof(ftc[3]));

	// Set the position
	posX = ::atof(((string)ftc[28]).c_str());
	posY = ::atof(((string)ftc[29]).c_str());
	posZ = ::atof(((string)ftc[30]).c_str());

	// Set the rotation from the database
	rotX = ::atof(((string)ftc[36]).c_str());
	rotY = ::atof(((string)ftc[37]).c_str());
	rotZ = ::atof(((string)ftc[38]).c_str());
	rotW = ::atof(((string)ftc[39]).c_str());

	// Set health and imagination stats
	currentHealth = ::atof(((string)ftc[40]).c_str());
	currentShield = ::atof(((string)ftc[41]).c_str());
	currentImagination = ::atof(((string)ftc[42]).c_str());
	maxHealth = ::atof(((string)ftc[43]).c_str());
	maxShield = ::atof(((string)ftc[44]).c_str());
	maxImagination = ::atof(((string)ftc[45]).c_str());

	// Set the player's level
	playerLevel = (ulonglong)atoi(ftc[33]);

	// Set the styles
	hairColor = (ulong)atoi(ftc[12]);
	hairStyle = (ulong)atoi(ftc[11]);
	unknown1 = 0;
	unknown2 = 0;
	shirtColor = (ulong)atoi(ftc[7]);
	pantsColor = (ulong)atoi(ftc[9]);
	eyebrows = (ulong)atoi(ftc[14]);
	eyes = (ulong)atoi(ftc[15]);
	mouth = (ulong)atoi(ftc[16]);

	// Set the faction ID?
	factionID = 0;

	charAccountID = (ulonglong)atoi(ftc[1]);
	llog = 0;

	currentUScore = 0;
	freeToPlay = false;
	invisible = false;
	teamMember = false; isMythran = false; mythranItems = false;

	itemCount = 4;
	shirtObjectID = 2000; neckObjectID = 2001; headObjectID = 2002;
	pantsObjectID = 2003; lhObjectID = 0; rhObjectID = 0;

	shirtID = (ulong)atoi(ftc[19]);
	neckID = (ulong)atoi(ftc[18]);
	headID = (ulong)atoi(ftc[17]);
	pantsID = (ulong)atoi(ftc[20]);
	lhID = 0;//(ulong)atoi(ftc[21]);
	rhID = 0;//(ulong)atoi(ftc[22]);

	/*string friendString = ftc[31];
	stringstream ss(friendString);
	string token;

	while (std::getline(ss, token, ',')) {
		cout << "Friend ID is: " << token << endl;
		userFriendIDs.push_back(token);
	}*/


	//replicaManager.Construct(this, false, UNASSIGNED_SYSTEM_ADDRESS, true);
	replicaManager.DisableReplicaInterfaces(this, REPLICA_RECEIVE_SCOPE_CHANGE);
}

Character::~Character()
{
	//replicaManager.Destruct(this, UNASSIGNED_SYSTEM_ADDRESS, true);
	//replicaManager.DereferencePointer(this);
	//character = 0;
}

Character::Character(const Character& character)
{
	memcpy(this, &character, sizeof(Character));
}

// Creates the character from database info
Ref< Character > Character::InitializeCharacter(ulonglong objectID)
{
	return Ref< Character >(new Character(objectID));
}

ReplicaReturnResult Character::SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp)
{
	//replicaManager.SetScope(this, true, systemAddress, false);

	//outBitStream->Write((ulong)1);
	
	cout << "Sending Construction..." << endl;
	//Character::WriteInitPacket(outBitStream);

	Character::TestReplicaInitPacket(outBitStream);

	auto v = OpenPacket(".\\world\\world_2a.bin");

	if (v.size() <= 0) {
		cout << "World 2A contains no data!!!!" << endl;
	}

	ServerSendPacket(rakServer, v, systemAddress);
	// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
	v = OpenPacket(".\\world\\world_2b.bin");	// I assume these two are sent together?
	ServerSendPacket(rakServer, v, systemAddress);
	
	SavePacket("testReplicaPacket.bin", (char*)outBitStream->GetData(), outBitStream->GetNumberOfBytesUsed());

	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult Character::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
{
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult Character::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
{
	if (inScope) {
		cout << "Sending scrope change as true" << endl;
	}
	else {
		cout << "Sending scope change as false" << endl;
	}

	outBitStream->Write(inScope);
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult Character::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
{
	outBitStream->Write((ulong)1);

	cout << "Serializing..." << endl;

	SavePacket("testSerializePacket.bin", (char*)outBitStream->GetData(), outBitStream->GetNumberOfBytesUsed());

	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult Character::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress)
{
	return REPLICA_PROCESSING_DONE;
}

// These aren't used...
ReplicaReturnResult Character::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	delete character;

	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult Character::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	bool inScope;
	inBitStream->Read(inScope);
	if (inScope)
		printf("Received message that scope is now true in Player\n");
	else
		printf("Received message that scope is now false in Player\n");
	return REPLICA_PROCESSING_DONE;
}

void Character::TestReplicaInitPacket(RakNet::BitStream *bitStream)
{
	auto v = OpenPacket(".\\world\\test\\world_2_1.bin");
	//auto v = OpenPacket("testReplicaPacket.bin");

	cout << endl << endl << endl << endl << "Extracted Replica Data Begins Here:" << endl << endl;

	RakNet::BitStream loadedStream(v.data(), v.size(), false);

	bool flag; // Used for flags (confusing if used for other bools... Use below)

	bool boolData;
	uchar ucharData;
	ushort shortData;
	ulong longData;
	ulonglong longLongData;
	wchar_t wcharData;
	float floatData;
	double doubleData;

	// This data will simply be read... It is RakNet
	// added though and will not be rewritten to the
	// new bitStream
	loadedStream.Read(ucharData); // This is the first byte - 0x24
	//bitStream->Write(ucharData);
	cout << "Packet Begins with: " << (int)ucharData << endl;

	loadedStream.Read(flag); // Flag for NetworkID
	//bitStream->Write(flag);
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;

	loadedStream.Read(shortData); // NetworkID
	//bitStream->Write(shortData);
	cout << "Network ID is: " << shortData << endl;

	// BEGIN DATA EXTRACTION AND WRITING!
	loadedStream.Read(longLongData); // Object ID
	cout << "Object ID is: " << longLongData << endl;
	//longLongData = charObjectID;
	bitStream->Write(longLongData);

	loadedStream.Read(longData); // LOT
	cout << "Char LOT is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(ucharData); // Character Length of name
	bitStream->Write(ucharData);
	cout << "Length of name is: " << (int)ucharData << endl;

	int byteLength = ucharData * 2;

	wchar_t nameData[24];
	

	for (int i = 0; i < ucharData; i++) {
		wchar_t wchar;
		loadedStream.Read(wchar);
		bitStream->Write(wchar);
		wcout << "- Name Letter is: " << wchar << endl;
	}

	uchar uByteLength = charName.size() - 1;
	//bitStream->Write(uByteLength);

	std::vector<wchar_t> charNameVector;
	for (int i = 0; i < charName.size() - 1; i++) {
		charNameVector.push_back(charName[i]);
		//bitStream->Write(charName[i]);
	}

	//bitStream->Write(charNameWchar);

	loadedStream.Read(longData); // Unknown long data
	cout << "Unknown Long Data is " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(flag); // Flag for compressed info
	cout << "Flag for data is ";

	if (flag == true) {
		cout << "true" << endl;
		bitStream->Write(flag);

		loadedStream.Read(longData); // Compressed Data Length
		cout << "- Length of compressed data is: " << longData << endl;
		bitStream->Write(longData);

		std::vector<uchar> compressedData;

		for (int i = 0; i < longData; i++) {
			loadedStream.Read(ucharData); // Each piece of the compressed data
			compressedData.push_back(ucharData);
		}

		cout << "- Compressed data is: " << RawDataToString(compressedData.data(), compressedData.size()) << endl;

		bitStream->Write((char*)compressedData.data(), compressedData.size());
	}
	else {
		cout << "false" << endl;
		bitStream->Write(flag);
	}

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // flag for Spawner ID
	cout << "Flag for spawner data is ";
	bitStream->Write(flag);
	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longLongData); // Spawner ID
		cout << "- Spawner ID is: " << longLongData << endl;
		bitStream->Write(longLongData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(flag); // Unknown flag
		bitStream->Write(flag);
		cout << "- Flag for data is ";

		if (flag == true) {
			cout << "true" << endl;
		}
		else {
			cout << "false" << endl;
		}

		loadedStream.Read(flag); // Unknown flag
		bitStream->Write(flag);
		cout << "- Flag for data is ";

		if (flag == true) {
			cout << "true" << endl;
		}
		else {
			cout << "false" << endl;
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag - Gray data if true
	bitStream->Write(flag);
	cout << "Flag for data is "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	
	loadedStream.Read(flag);
	bitStream->Write(flag);
	cout << "Flag for data is ";

	if (flag == true) {
		cout << "true" << endl;

		for (int i = 0; i < 7; i++) {
			loadedStream.Read(longData); // Unknown ulong/signed long/???
			cout << "- Unknown Long Data is: " << longData << endl;
			bitStream->Write(longData);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		for (int i = 0; i < 2; i++) {
			loadedStream.Read(floatData); // Unknown piece of float(?) data
			cout << "- Unknown Float Data is: " << floatData << endl;
			bitStream->Write(floatData);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		loadedStream.Read(longData); // Unknown ulong
		cout << "- Unknown Long Data is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(boolData);
		cout << "- Unknown Bool Data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(flag); // Unknown flag
		bitStream->Write(flag);
		cout << "- Flag for data is: ";

		if (flag == true) {
			cout << "true" << endl;
		}
		else {
			cout << "false" << endl;
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for character position data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		for (int i = 0; i < 7; i++) {
			// Checks PosX - PosY - PosZ - RotX/Z - RotY - RotZ/X - RotW
			loadedStream.Read(floatData); // Data for above
			cout << "- Position/Rotation Data is for i=" << i + 1 << ": " << floatData << endl;
			bitStream->Write(floatData);
		}

		loadedStream.Read(boolData); // Is player on ground?
		cout << "- Is player on ground: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		loadedStream.Read(boolData); // Unknown bool
		cout << "- Unknown bool is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);
		
		loadedStream.Read(flag); // Flag for velocity data
		bitStream->Write(flag);
		cout << "- Flag for data is: ";

		if (flag == true) {
			cout << "true" << endl;

			for (int i = 0; i < 3; i++) {
				// Checks VelX - VelY - VelZ
				loadedStream.Read(floatData); // See above
				cout << "-- Velocity (for " << i << ") is: " << floatData << endl;
				bitStream->Write(floatData);
			}
		}
		else {
			cout << "false" << endl;
		}

		loadedStream.Read(flag); // Flag for angular velocity data
		bitStream->Write(flag);
		cout << "- Flag for data is: ";

		if (flag == true) {
			cout << "true" << endl;

			for (int i = 0; i < 3; i++) {
				// Checks AngularVelX - AngularVelY - AngularVelZ
				loadedStream.Read(floatData); // See above
				cout << "-- Angular Velocity (for " << i << ") is: " << floatData << endl;
				bitStream->Write(floatData);
			}
		}
		else {
			cout << "false" << endl;
		}

		loadedStream.Read(flag); // Flag for data
		bitStream->Write(flag);
		cout << "- Flag for data is: ";

		if (flag == true) {
			cout << "true" << endl;

			loadedStream.Read(longLongData); // Unknown ulonglong data
			cout << "-- Unknown uLongLong Data is: " << longLongData << endl;
			bitStream->Write(longLongData);

			for (int i = 0; i < 3; i++) {
				// Three unknown floats(?)
				loadedStream.Read(floatData); // Unknown float data
				cout << "-- Unknown float data is: " << floatData << endl;
				bitStream->Write(floatData);
			}

			loadedStream.Read(flag); // Unknown flag
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;

				for (int i = 0; i < 3; i++) {
					// Three unknown floats(?)
					loadedStream.Read(floatData); // Unknown float data
					cout << "--- Unknown float data is: " << floatData << endl;
					bitStream->Write(floatData);
				}
			}
			else {
				cout << "false" << endl;
			}
		}
		else {
			cout << "false" << endl;
		}

		loadedStream.Read(flag); // Unknown flag
		bitStream->Write(flag);
		cout << "Flag for data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	// This struct is repeated
	// Struct 1
	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Count of following structs
		cout << "- Count of following structs is: " << longData << endl;
		bitStream->Write(longData);

		int structCount = longData;

		for (int i = 0; i < structCount; i++) {
			loadedStream.Read(longData); // Unknown ulong
			cout << "- Unknown uLong Data is: " << longData << endl;
			bitStream->Write(longData);

			loadedStream.Read(flag); // Unknown flag
			bitStream->Write(flag);
			cout << "- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;
				loadedStream.Read(longData); // Unknown long data
				cout << "-- Unknown uLong Data is: " << longData << endl;
			}
			else {
				cout << "false" << endl;
			}

			for (int j = 0; j < 8; j++) {
				loadedStream.Read(boolData); // Unknown Bool data
				cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
				bitStream->Write(boolData);
			}

			bool structToggle;
			loadedStream.Read(boolData); // Unknown Bool data - Toggles below struct
			cout << "- Struct Toggle bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			structToggle = boolData;
			bitStream->Write(boolData);

			loadedStream.Read(boolData); // Unknown Bool data
			cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			bitStream->Write(boolData);

			if (structToggle == true) {
				cout << "-- Toggled struct (from above) is true" << endl;
				loadedStream.Read(longLongData); // Unknown uLongLong Data
				cout << "Unknown uLongLong Data is: " << longLongData << endl;
				bitStream->Write(longLongData);
			}
			else {
				cout << "-- Toggled struct (from above) is false" << endl;
			}

			loadedStream.Read(longData); // Unknown long data
			cout << "Unknown Long Data is: " << longData << endl;
			bitStream->Write(longData);

		}
	}
	else {
		cout << "false" << endl;
	}

	// Struct 2
	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Count of following structs
		cout << "- Count of following structs is: " << longData << endl;
		bitStream->Write(longData);

		int structCount = longData;

		for (int i = 0; i < structCount; i++) {
			loadedStream.Read(longData); // Unknown ulong
			cout << "- Unknown uLong Data is: " << longData << endl;
			bitStream->Write(longData);

			loadedStream.Read(flag); // Unknown flag
			bitStream->Write(flag);
			cout << "- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;
				loadedStream.Read(longData); // Unknown long data
				cout << "-- Unknown uLong Data is: " << longData << endl;
			}
			else {
				cout << "false" << endl;
			}

			for (int j = 0; j < 8; j++) {
				loadedStream.Read(boolData); // Unknown Bool data
				cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
				bitStream->Write(boolData);
			}

			bool structToggle;
			loadedStream.Read(boolData); // Unknown Bool data - Toggles below struct
			cout << "- Struct Toggle bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			structToggle = boolData;
			bitStream->Write(boolData);

			loadedStream.Read(boolData); // Unknown Bool data
			cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			bitStream->Write(boolData);

			if (structToggle == true) {
				cout << "-- Toggled struct (from above) is true" << endl;
				loadedStream.Read(longLongData); // Unknown uLongLong Data
				cout << "Unknown uLongLong Data is: " << longLongData << endl;
				bitStream->Write(longLongData);
			}
			else {
				cout << "-- Toggled struct (from above) is false" << endl;
			}

			loadedStream.Read(longData); // Unknown long data
			cout << "Unknown Long Data is: " << longData << endl;
			bitStream->Write(longData);

		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		for (int i = 0; i < 9; i++) {
			loadedStream.Read(longData); // Unknown long(?) data
			cout << "- Unknown Long Data is: " << longData << endl;
			bitStream->Write(longData);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for character stats data
	bitStream->Write(flag);
	cout << "Flag for charater stats data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // u32 data for current health
		cout << "- Current Health (u32) is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(floatData); // float data for current health
		cout << "- Current Health (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(longData); // u32 data for current armor
		cout << "- Current Armor (u32) is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(floatData); // float data for current armor
		cout << "- Current Armor (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(longData); // u32 data for current imagination
		cout << "- Current Imagination (u32) is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(floatData); // float data for current imagination
		cout << "- Current Imagination (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(longData); // Unknown long data
		cout << "- Unknown Long Data is: " << longData << endl;
		bitStream->Write(longData);

		for (int i = 0; i < 3; i++) {
			loadedStream.Read(boolData); // Unknown Bool data
			cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			bitStream->Write(boolData);
		}

		loadedStream.Read(floatData); // float data for max health
		cout << "- Max Health (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(floatData); // float data for max armor
		cout << "- Max Armor (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(floatData); // float data for max imagination
		cout << "- Max Imagination (float) is: " << floatData << endl;
		bitStream->Write(floatData);

		loadedStream.Read(longData); // Unknown long - Count for faction data?
		cout << "- Unknown Long Data (Count for faction struct?) is: " << longData << endl;
		bitStream->Write(longData);

		int factionStructCount = longData;

		for (int i = 0; i < factionStructCount; i++) {
			loadedStream.Read(longData); // Faction ID of player?
			cout << "-- Faction ID of player(?) is: " << longData << endl;
			bitStream->Write(longData);
		}

		loadedStream.Read(boolData); // Unknown Bool data
		cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		// Skipped in Serialization
		for (int i = 0; i < 2; i++) {
			loadedStream.Read(flag); // Flag for Data
			cout << "- Flag for data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
			bitStream->Write(flag);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(boolData); // Unknown Bool data
		cout << "- Unknown Bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(boolData); // Unknown Bool data
		cout << "- Unknown Bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		loadedStream.Read(ucharData); // Unknown uChar Data
		bitStream->Write(ucharData);
		cout << "- Unknown uChar Data is: " << (int)ucharData << endl;
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data (containing level data)
	bitStream->Write(flag);
	cout << "Level Bool data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Level
		cout << "- Level of char is: " << longData << endl;
		bitStream->Write(longData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		for (int i = 0; i < 2; i++) {
			loadedStream.Read(boolData); // Unknown Bool data
			cout << "- Unknown bool data is: "; boolData == true ? cout << "true" : cout << "false"; cout << endl;
			bitStream->Write(boolData);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data (possibly for "co" data from chardata packet?)
	bitStream->Write(flag);
	cout << "Flag for (\"co\" data)? is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longLongData); // Unknown ulonglong data - possibly "co" from chardata packet?
		cout << "Unknown uLongLong Data (co?) is: " << longLongData << endl;
		bitStream->Write(longLongData);
	}
	else {
		cout << "false" << endl;
	}

	for (int i = 0; i < 3; i++) {
		loadedStream.Read(flag); // Unknown flag
		cout << "Flag for data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(flag);
	}

	loadedStream.Read(longData); // Hair color
	cout << "Hair color is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Hair style
	cout << "Hair style is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Hdc/hd?
	cout << "HDC/HD(?) is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Shirt color
	cout << "Shirt color is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Pants color
	cout << "Pants color is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // cd(?)
	cout << "CD(?) is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Hdc/hd? color
	cout << "HDC/HD(?) is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Eyebrows style
	cout << "Eyebrows style is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Eyes style
	cout << "Eyes style is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longData); // Mouth style
	cout << "Mouth style is: " << longData << endl;
	bitStream->Write(longData);

	loadedStream.Read(longLongData); // Account ID
	cout << "Account ID is: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // llog
	cout << "llog is: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Unknown uLongLong Data from chardata packet(?)
	cout << "Unknown uLongLong Data is: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // LEGO Score (uScore)
	cout << "LEGO Score (uScore) is: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(boolData); // Bool that sets F2P - 1 = F2P, 0 = Premium
	cout << "Is Character F2P?: "; flag == true ? cout << "yes" : cout << "no"; cout << endl;
	bitStream->Write(boolData);

	loadedStream.Read(longLongData); // Total Currency Collected
	cout << "Total Currency Collected: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Bricks Smashed
	cout << "Total Bricks Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Quickbuilds Completed
	cout << "Total Quickbuilds Completed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Smashables Smashed
	cout << "Total Smashables Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Quick Builds Completed
	cout << "Total Quick Builds Completed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total enemies smashed
	cout << "Total Enemies Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Rockets Used
	cout << "Total Rockets Used: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total missions completed
	cout << "Total Missions Completed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Pets tamed
	cout << "Total Pets Tamed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Imagination power-ups collected
	cout << "Total Imagination Power-Ups Collected: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Life Power-Ups Collected
	cout << "Total Life Power-Ups Collected: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Armor power-ups collected
	cout << "Total Armor Power-Ups Collected: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Distance Traveled (in meters)
	cout << "Total Distance Traveled (in meters): " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Number of times smashed
	cout << "Number of Times Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total damage taken
	cout << "Total Damage Taken: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total damage Healed
	cout << "Total Damage Healed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Armor Repaired
	cout << "Total Armor Repaired: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Imagination Restored
	cout << "Total Imagination Restored: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Imagination used
	cout << "Total Imagination Used: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Time Airborne in a Race Car (in seconds)
	cout << "Total Time Airborne in a Race Car (in seconds): " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Racing Imagination power-ups collected
	cout << "Total Racing Imagination Power-Ups Collected: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Racing Imagination Crates Smashed
	cout << "Total Racing Imagination Crates Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Times Race Car Boost Activated
	cout << "Total Times Race Car Boost Activated: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Wrecks in a Race Car
	cout << "Total Wrecks in a Race Car: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Racing Smashables smashed
	cout << "Total Racing Smashables Smashed: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total Races finished
	cout << "Total Races Finished: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(longLongData); // Total 1st Place Race Finishes
	cout << "Total 1st Place Race Finishes: " << longLongData << endl;
	bitStream->Write(longLongData);

	loadedStream.Read(flag); // Unknown flag
	cout << "Flag for data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
	bitStream->Write(flag);

	loadedStream.Read(flag); // Unknown flag
	bitStream->Write(flag);
	cout << "Flag for data is ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(shortData); // Count of characters
		cout << "- Count of characters is: " << shortData << endl;
		bitStream->Write(shortData);

		for (int i = 0; i < shortData; i++) {
			wchar_t wchar;
			loadedStream.Read(wchar); // Characters (LDF format?)
			cout << "-- Character " << i << " is: " << wchar << endl;
			bitStream->Write(wchar);
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(flag); // Flag for data
		bitStream->Write(flag);
		cout << "- Flag for data is: ";

		if (flag == true) {
			cout << "true" << endl;
		}
		else {
			cout << "false" << endl;
		}

		loadedStream.Read(boolData); // Unknown Bool Data
		cout << "- Unknown Bool data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		loadedStream.Read(ucharData); // Unknown uChar Data
		bitStream->Write(ucharData);
		cout << "- Unknown uChar Data is: " << (int)ucharData << endl;

		loadedStream.Read(boolData); // Unknown Bool Data
		cout << "- Unknown Bool data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		loadedStream.Read(ucharData); // Unknown uChar Data
		bitStream->Write(ucharData);
		cout << "- Unknown uChar Data is: " << (int)ucharData << endl;
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Unknown Long Data
		cout << "- Unknown Long Data is: " << longData << endl;
		bitStream->Write(longData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longLongData); // Unknown longLongData
		cout << "- Unknown uLongLong Data is: " << longLongData << endl;
		bitStream->Write(longLongData);

		loadedStream.Read(ucharData); // Count?
		bitStream->Write(ucharData);
		cout << "- Unknown uChar Data is: " << (int)ucharData << endl;

		loadedStream.Read(boolData); // Unknown Bool Data
		cout << "- Unknown Bool data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
		bitStream->Write(boolData);

		loadedStream.Read(longData); // Unknown Long Data
		cout << "- Unknown Long Data is: " << longData << endl;
		bitStream->Write(longData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for char items data
	bitStream->Write(flag);
	cout << "Flag for char items data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Count of player items struct
		cout << "- Number of player items is: " << longData << endl;
		bitStream->Write(longData);
		
		int itemsStructCount = longData;

		for (int i = 0; i < itemsStructCount; i++) {
			signed long long sLongLongData;
			loadedStream.Read(sLongLongData); // ObjectID of the Item
			cout << "-- Item ObjectID is: " << longLongData << endl;
			bitStream->Write(sLongLongData);

			int itemLOT;

			signed long sLongData;

			loadedStream.Read(sLongData); // LOT of the item
			cout << "-- Item LOT is: " << longData << endl;
			itemLOT = longData;
			bitStream->Write(sLongData);

			loadedStream.Read(flag); // Flag for data
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;
			}
			else {
				cout << "false" << endl;
			}

			loadedStream.Read(flag); // Flag for data
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;

				loadedStream.Read(longData); // Unknown Long Data - Always 0x01?
				cout << "--- Unknown Long Data is: " << longData << endl;
				bitStream->Write(longData);
			}
			else {
				cout << "false" << endl;
			}

			loadedStream.Read(flag); // Flag for item slot data
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;

				loadedStream.Read(shortData); // Slot in inventory
				cout << "--- Item with LOT " << itemLOT << " is in slot " << shortData << endl;
				bitStream->Write(shortData);
			}
			else {
				cout << "false" << endl;
			}

			loadedStream.Read(flag); // Flag for data
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;

				loadedStream.Read(longData); // Unknown Long Data - Always 0x04?
				cout << "--- Unknown Long Data is: " << longData << endl;
				bitStream->Write(longData);
			}
			else {
				cout << "false" << endl;
			}

			std::vector<uchar> compressedDataVector;

			loadedStream.Read(flag); // Unknown flag
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;

				loadedStream.Read(longData); // Count of following compressed data
				cout << "--- Count of compressed data (in bytes) is: " << longData << endl;
				bitStream->Write(longData);

				int compressedCount = longData;

				for (int j = 0; j < compressedCount; i++) {
					loadedStream.Read(ucharData);
					compressedDataVector.push_back(ucharData);
					bitStream->Write(ucharData);
				}

				cout << "---- Compressed data is: " << RawDataToString(compressedDataVector.data(), compressedDataVector.size()) << endl;
			}
			else {
				cout << "false" << endl;
			}

			loadedStream.Read(flag); // Unknown flag
			bitStream->Write(flag);
			cout << "-- Flag for data is: ";

			if (flag == true) {
				cout << "true" << endl;
			}
			else {
				cout << "false" << endl;
			}
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Unknown long data
		cout << "- Unknown Long Data is: " << longData << endl;
		bitStream->Write(longData);
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longData); // Count of structs
		cout << "- Struct count is: " << longData << endl;
		bitStream->Write(longData);

		int structCount = longData;

		for (int i = 0; i < structCount; i++) {
			for (int j = 0; j < 4; j++) {
				loadedStream.Read(longData); // Unknown long data
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);
			}

			loadedStream.Read(longData); // Struct count
			cout << "-- Struct count is: " << longData << endl;
			bitStream->Write(longData);

			int structCount2 = longData;

			for (int j = 0; j < structCount2; j++) {
				loadedStream.Read(longData); // Unknown long data
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longData); // Unknown long data - Something in BehaviorTemplate?
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longData); // Unknown long data
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longData); // Unknown long data - Always 0x18?
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longLongData); // Unknown longlong data - Minifig ObjectID?
				cout << "-- Unknown uLongLong Data is: " << longLongData << endl;
				bitStream->Write(longLongData);

				loadedStream.Read(longLongData); // Unknown longlong data - Minifig ObjectID?
				cout << "-- Unknown uLongLong Data is: " << longLongData << endl;
				bitStream->Write(longLongData);

				loadedStream.Read(longLongData); // Unknown longlong data - Always 0x00?
				cout << "-- Unknown uLongLong Data is: " << longLongData << endl;
				bitStream->Write(longLongData);

				loadedStream.Read(boolData); // Unknown Bool Data
				cout << "- Unknown Bool data is: "; flag == true ? cout << "true" : cout << "false"; cout << endl;
				bitStream->Write(boolData);

				loadedStream.Read(longData); // Unknown long data - Always 0x00?
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longData); // Unknown long data - Always 0x00?
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);

				loadedStream.Read(longData); // Unknown long data - Always 0x00?
				cout << "-- Unknown uLong Data is: " << longData << endl;
				bitStream->Write(longData);
			}
		}
	}
	else {
		cout << "false" << endl;
	}

	loadedStream.Read(longData); // Number of BehaviorEffects(?)
	cout << "Number of BehaviorEffects(?) is: " << longData << endl;
	bitStream->Write(longData);

	int behaviorEffectsCount = longData;

	for (int i = 0; i < behaviorEffectsCount; i++) {
		loadedStream.Read(ucharData); // Behavior strength length
		cout << "- BehaviorEffects string length is: " << (int)ucharData << endl;
		bitStream->Write(ucharData);

		int behaviorStringLength = ucharData;

		std::string behaviorString;

		for (int j = 0; j < behaviorStringLength; i++) {
			loadedStream.Read(ucharData); // String char
			bitStream->Write(ucharData);
			behaviorString.push_back(ucharData);
		}

		cout << "- BehaviorString is: " << behaviorString << endl;

		loadedStream.Read(longData); // EffectID
		cout << "- EffectID is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(ucharData); // wstring Length
		bitStream->Write(ucharData);
		cout << "- WString Length is: " << (int)ucharData << endl;

		int wstringLength = ucharData;

		string wstringString;

		for (int j = 0; j < wstringLength; j++) {
			wchar_t wchar;

			loadedStream.Read(wchar);
			wstringString.push_back(wchar);
			bitStream->Write(wchar);
		}

		cout << "- EffectType is: " << wstringString << endl;

		loadedStream.Read(longData); // Unknown long data
		cout << "- Unknown uLong Data is: " << longData << endl;
		bitStream->Write(longData);

		loadedStream.Read(longLongData); // Unknown longlong data
		cout << "- Unknown uLongLong Data is: " << longLongData << endl;
		bitStream->Write(longLongData);
	}

	loadedStream.Read(flag); // Flag for data
	bitStream->Write(flag);
	cout << "Flag for data is: ";

	if (flag == true) {
		cout << "true" << endl;

		loadedStream.Read(longLongData); // Unknown longlong data
		cout << "- Unknown uLongLong Data is: " << longLongData << endl;
		bitStream->Write(longLongData);
	}
	else {
		cout << "false" << endl;
	}

	cout << endl << "End of Extracted Replica Data." << endl << endl << endl << endl << endl;
}

void Character::CreateFriendsList(RakPeerInterface *rakServer, const SystemAddress& systemAddr, ulonglong currentObjectID) 
{
	RakNet::BitStream bitStream;
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 30, &bitStream);
	bitStream.Write((uchar)0);

	ushort packetSize;

	RakNet::BitStream friendsBitStream;

	ushort friendAmt = userFriendIDs.size();

	for (int i = 0; i < userFriendIDs.size(); i++) {
		ulonglong characterID = (ulonglong)atoi(userFriendIDs[i].c_str());

		/*auto character = OnlineUsersPool->FindChar(characterID);

		if (character != NULL) {
			uchar isOnline = 1;
			uchar isBestFriend = 0;
			uchar isFTP = 0;
			ushort worldID = character->currentWorldID;
			ushort worldInstance = 0;
			ulong worldInstanceLong = 0;
			ulonglong charID = character->charObjectID;
			std::wstring charName = character->charName;
			charName.resize(66);

			friendsBitStream.Write(isOnline);
			friendsBitStream.Write(isBestFriend);
			friendsBitStream.Write(isFTP);
			for (int j = 0; j < 5; j++) { friendsBitStream.Write((uchar)0); }
			friendsBitStream.Write(worldID);
			friendsBitStream.Write(worldInstance);
			friendsBitStream.Write(worldInstanceLong);
			friendsBitStream.Write((char*)charName.data(), sizeof(wchar_t)* charName.size() / 2);
			for (int k = 0; k < 6; k++) { friendsBitStream.Write((uchar)0); }
		}
		else {
			friendsBitStream.Write((uchar)0);
			friendsBitStream.Write((uchar)0);
			friendsBitStream.Write((uchar)0);
			friendsBitStream.Write((ulong)0);
			friendsBitStream.Write((uchar)0);
			friendsBitStream.Write((ushort)1000);
			friendsBitStream.Write((ushort)0);
			friendsBitStream.Write((ulong)0);
			std::wstring noCharName(L"0");
			noCharName.resize(66);
			friendsBitStream.Write((char*)noCharName.data(), sizeof(wchar_t)* noCharName.size() / 2);
			friendsBitStream.Write((ulong)0);
			friendsBitStream.Write((ushort)0);
		}*/
	}

	packetSize = friendsBitStream.GetNumberOfBytesUsed() + bitStream.GetNumberOfBytesUsed() - 1;

	bitStream.Write(packetSize);
	bitStream.Write(friendAmt);
	bitStream.Write(friendsBitStream);
}

void Character::WriteCharDataPacket(RakPeerInterface *rakServer, SystemAddress &systemAddr)
{
	RakNet::BitStream bitStream;

	WriteLDFData<signed long long>(L"accountID", 8, charAccountID, &bitStream);
	WriteLDFData<signed long>(L"chatmode", 1, 0, &bitStream);
	WriteLDFData<bool>(L"editor_enabled", 7, false, &bitStream);
	WriteLDFData<signed long>(L"editor_level", 1, 0, &bitStream);
	WriteLDFData<bool>(L"freetrial", 7, false, &bitStream);
	WriteLDFData<signed long>(L"gmlevel", 1, 0, &bitStream);
	WriteLDFData<bool>(L"legoclub", 7, true, &bitStream);
	WriteLDFData<signed long long>(L"levelid", 8, 10001, &bitStream);
	WriteLDFData<std::wstring>(L"name", 0, charName, &bitStream);
	WriteLDFData<signed long long>(L"objid", 9, charObjectID, &bitStream);
	WriteLDFData<float>(L"position.x", 3, posX, &bitStream);
	WriteLDFData<float>(L"position.y", 3, posY, &bitStream);
	WriteLDFData<float>(L"position.z", 3, posZ, &bitStream);
	WriteLDFData<signed long>(L"propertycloneid", 1, 10001, &bitStream);
	WriteLDFData<ulong>(L"propertycloneid", 5, 10001, &bitStream);
	WriteLDFData<signed long long>(L"reputation", 8, 0, &bitStream);
	WriteLDFData<bool>(L"requiresrename", 7, false, &bitStream);
	WriteLDFData<float>(L"rotation.w", 3, rotW, &bitStream);
	WriteLDFData<float>(L"rotation.x", 3, rotX, &bitStream);
	WriteLDFData<float>(L"rotation.y", 3, rotY, &bitStream);
	WriteLDFData<float>(L"rotation.z", 3, rotZ, &bitStream);
	WriteLDFData<signed long>(L"template", 1, 1, &bitStream);

	Character::CreateXMLData(&bitStream);
}

void Character::CreateXMLData(RakNet::BitStream *dataStream)
{
	RakNet::BitStream xmlStream;

	stringstream qr;
	qr << "SELECT `items` FROM `characters` WHERE `objectID` = '" << charObjectID << "';";
	auto query = Database::Query(qr.str());

	if (mysql_num_rows(query) == 0) {
		cout << "ERROR! No minfig found!!";
	}

	auto ftc = mysql_fetch_row(query);

	std::vector<string> itemsVector;

	std::string allItems = ftc[0];
	std::istringstream ssItems(allItems);
	std::string token;
	
	// The string from the database is formated like so:
	// objectinfo1:objectinfo2:objectinfo3,objectinfo1:objectinfo2:objectinfo3,..."
	// This breaks it apart, first by the "," and then by the ":" and adds each piece
	// to the vector 'allItems'
	while (std::getline(ssItems, token, ',')) {
		cout << token << endl;

		std::istringstream itemInfo(token);
		std::string token2;

		while (std::getline(itemInfo, token2, ':')) {
			itemsVector.push_back(token2);
		}
	}

	stringstream xmlString;
	xmlString << "<obj v=\"1\"><mf hc=\"" << hairColor << "\" hs=\"" << hairStyle << "\" hd=\"0\" t=\"" << shirtColor << "\" l=\"" << pantsID << "\" hcd=\"0\" cd=\"0\" lh=\"" << lhID << "\" rh=\"" << rhID << "\" es=\"" << eyes << "\" ess=\"" << eyebrows << "\" ms=\"" << mouth << "\"/>";
	xmlString << "<char acct=\"" << charAccountID << "\" cc=\"0\" gm=\"0\" ft=\"0\"/>";
	xmlString << "<dest hm=\"4\" hc=\"4\" im=\"0\" ic=\"0\" am=\"0\" ac=\"0\" d=\"0\">";
	xmlString << "<inv><items><in t=\"0\">";

	for (int i = 0; i < itemsVector.size(); i) {
		xmlString << "<i l=\"" << itemsVector[i] << "\" id=\"" << itemsVector[++i] << "\" s=\"" << itemsVector[++i] << "\" eq=\"" << itemsVector[++i] << "\"/>";
	}

	xmlString << "</in></items></inv>";
	xmlString << "<lvl l=\"" << playerLevel << "\" cv=\"1\" sb=\"" << currentUScore << "\"/></obj>";

	cout << endl << endl << xmlString.str() << endl << endl;

	xmlStream.Write((uchar)14);
	xmlStream.Write(L"xmlData");
	xmlStream.Write((uchar)13);
	xmlStream.Write((ulong)sizeof(xmlString));
	xmlStream.Write(xmlString.str());

	dataStream->Write(xmlStream);
}

//void Character::WriteInitPacket(RakPeerInterface *rakServer, SystemAddress &systemAddr, RakNet::BitStream *bitStream)
void Character::WriteInitPacket(RakNet::BitStream *bitStream)
{
	bitStream->Write(charObjectID);
	bitStream->Write(charLOT);

	uchar nameLength = charName.length() + 2;
	bitStream->Write(nameLength);
	bitStream->Write((char*)charName.data(), sizeof(wchar_t) * charName.size() / 2);

	bitStream->Write((ulong)0);
	bitStream->Write((bool)true);
	//bitStream.Write((ulong)6);

	RakNet::BitStream ldfData;

	WriteLDFData(L"blueprintid", 9, 30001, &ldfData);
	WriteLDFData(L"componentWhitelist", 1, 1, &ldfData);
	WriteLDFData(L"modelBehaviors", 5, 1000, &ldfData);
	WriteLDFData(L"modelType", 1, 1, &ldfData);
	WriteLDFData(L"propertyObjectID", 7, true, &ldfData);
	WriteLDFData(L"userModelID", 9, 1, &ldfData);

	ulong structLength = ldfData.GetNumberOfBytesUsed();

	bitStream->Write(structLength);
	bitStream->Write(ldfData.GetData());

	bitStream->Write((bool)true);
	bitStream->Write((bool)true); // Flag for data
	bitStream->Write((ulonglong)22527622183783184);

	for (int i = 0; i < 6; i++) {
		bitStream->Write((bool)false);
	}

	for (int j = 0; j < 7; j++) {
		bitStream->Write((ulong)0);
	}

	bitStream->Write((bool)true);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);

	bitStream->Write((bool)true);
	bitStream->Write((ulong)0);
	bitStream->Write((bool)true);

	// User data - pt 1
	bitStream->Write((bool)true); // BOOL to include user data
	bitStream->Write(posX); // Position X
	bitStream->Write(posY); // Position Y
	bitStream->Write(posZ); // Position Z
	bitStream->Write(rotX); // Rotation X
	bitStream->Write(rotY); // Rotation Y
	bitStream->Write(rotZ); // Rotation Z
	bitStream->Write(rotW); // Rotation W

	bitStream->Write((bool)true); // ???
	bitStream->Write((bool)true); // ???

	bitStream->Write((bool)true); // Flag for following data

	bitStream->Write((float)0.0); // Velocity X?
	bitStream->Write((float)0.0); // Velocity Y?
	bitStream->Write((float)0.0); // Velocity Z?

	bitStream->Write((bool)true); // Flag for following data

	bitStream->Write((float)0.0); // Acceleration X?
	bitStream->Write((float)0.0); // Acceleration Y?
	bitStream->Write((float)0.0); // Acceleration Z?

	// Unknown data
	bitStream->Write((bool)true);
	bitStream->Write((ulonglong)10001);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);

	bitStream->Write((bool)true); // Flag for data?
	
	for (int k = 0; k < 3; k++) {
		bitStream->Write((ulong)0);
	}

	bitStream->Write((bool)false); // Flag for serialization data?

	for (int l = 0; l < 2; l++) {
		bitStream->Write((bool)true);
		bitStream->Write((ulong)1);
		bitStream->Write((ulong)0);

		bitStream->Write((bool)true);
		bitStream->Write((ulong)0);

		for (int m = 0; m < 10; m++) {
			bitStream->Write((bool)true);
		}

		bitStream->Write((ulonglong)10002);
		bitStream->Write((ulong)0);
	}

	bitStream->Write((bool)true);
	for (int n = 0; n < 9; n++) {
		bitStream->Write((ulong)0);
	}

	// User data - pt 2
	bitStream->Write((bool)true); // Flag for user data
	bitStream->Write((ulong)currentHealth);
	bitStream->Write(currentHealth);
	bitStream->Write((ulong)currentShield);
	bitStream->Write(currentShield);
	bitStream->Write((ulong)currentImagination);
	bitStream->Write(currentImagination);

	bitStream->Write((ulong)0);

	bitStream->Write((bool)true);
	bitStream->Write((bool)true);
	bitStream->Write((bool)true);
	
	bitStream->Write(maxHealth);
	bitStream->Write(maxShield);
	bitStream->Write(maxImagination);

	// Something with factions?
	bitStream->Write((ulong)1);
	bitStream->Write((ulong)0);

	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);

	bitStream->Write((uchar)0);

	bitStream->Write((bool)true);
	bitStream->Write(playerLevel);

	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);

	// User data - pt 3
	bitStream->Write((bool)true);

	bitStream->Write((ulonglong)1003);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);

	bitStream->Write(hairColor);
	bitStream->Write(hairStyle);
	bitStream->Write((ulong)0);
	bitStream->Write(shirtColor);
	bitStream->Write(pantsColor);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);
	bitStream->Write(eyebrows);
	bitStream->Write(eyes);
	bitStream->Write(mouth);
	bitStream->Write(charAccountID);
	bitStream->Write(llog);
	bitStream->Write((ulonglong)0);
	bitStream->Write(currentUScore);

	bitStream->Write(freeToPlay);

	for (int ix = 0; ix < 27; ix++) {
		bitStream->Write((ulonglong)0);
	}

	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)false);
	bitStream->Write((bool)true);
	bitStream->Write((uchar)0);
	bitStream->Write((bool)true);
	bitStream->Write((uchar)0);

	bitStream->Write((bool)true);
	bitStream->Write((ulong)0);

	bitStream->Write((bool)true);
	bitStream->Write((ulonglong)0);
	bitStream->Write((uchar)0);
	bitStream->Write((bool)false);
	bitStream->Write((ulong)0);

	// Character Items
	bitStream->Write((bool)true); // Flag for data
	bitStream->Write((ulong)3); // How many items does the character have on?
	bitStream->Write(shirtObjectID);
	bitStream->Write(shirtID);
	bitStream->Write((bool)false);
	bitStream->Write((bool)true);
	bitStream->Write((ulong)1);
	bitStream->Write((bool)true);
	bitStream->Write((ushort)0); // Slot in inventory
	bitStream->Write((bool)true);
	bitStream->Write((ulong)4);
	bitStream->Write((bool)true);
	
	ldfData.Reset();
	WriteLDFData(L"_Metric_Currency_Delta_Int", 1, 1, &ldfData);
	WriteLDFData(L"_Metric_Mail_ID_Int64", 8, 100, &ldfData);
	WriteLDFData(L"_Metric_Mission_ID_Int", 1, 2, &ldfData);
	WriteLDFData(L"_Metric_Source_LOT_Int", 1, 3, &ldfData);
	WriteLDFData(L"_Metric_Transaction_ID_Int64", 9, 101, &ldfData);
	WriteLDFData(L"assemblyPartLOTs", 0, L"9001", &ldfData);
	
	structLength = ldfData.GetNumberOfBytesUsed();

	bitStream->Write(structLength);
	bitStream->Write(ldfData.GetData());

	bitStream->Write(pantsObjectID);
	bitStream->Write(pantsID);
	bitStream->Write((bool)false);
	bitStream->Write((bool)true);
	bitStream->Write((ulong)1);
	bitStream->Write((bool)true);
	bitStream->Write((ushort)1); // Slot in inventory
	bitStream->Write((bool)true);
	bitStream->Write((ulong)4);
	bitStream->Write((bool)true);

	ldfData.Reset();
	WriteLDFData(L"_Metric_Currency_Delta_Int", 1, 1, &ldfData);
	WriteLDFData(L"_Metric_Mail_ID_Int64", 8, 100, &ldfData);
	WriteLDFData(L"_Metric_Mission_ID_Int", 1, 2, &ldfData);
	WriteLDFData(L"_Metric_Source_LOT_Int", 1, 3, &ldfData);
	WriteLDFData(L"_Metric_Transaction_ID_Int64", 9, 101, &ldfData);
	WriteLDFData(L"assemblyPartLOTs", 0, L"9001", &ldfData);

	structLength = ldfData.GetNumberOfBytesUsed();

	bitStream->Write(structLength);
	bitStream->Write(ldfData.GetData());

	bitStream->Write(headObjectID);
	bitStream->Write(headID);
	bitStream->Write((bool)false);
	bitStream->Write((bool)true);
	bitStream->Write((ulong)1);
	bitStream->Write((bool)true);
	bitStream->Write((ushort)2); // Slot in inventory
	bitStream->Write((bool)true);
	bitStream->Write((ulong)4);
	bitStream->Write((bool)true);

	ldfData.Reset();
	WriteLDFData(L"_Metric_Currency_Delta_Int", 1, 1, &ldfData);
	WriteLDFData(L"_Metric_Mail_ID_Int64", 8, 100, &ldfData);
	WriteLDFData(L"_Metric_Mission_ID_Int", 1, 2, &ldfData);
	WriteLDFData(L"_Metric_Source_LOT_Int", 1, 3, &ldfData);
	WriteLDFData(L"_Metric_Transaction_ID_Int64", 9, 101, &ldfData);
	WriteLDFData(L"assemblyPartLOTs", 0, L"9001", &ldfData);

	structLength = ldfData.GetNumberOfBytesUsed();

	bitStream->Write(structLength);
	bitStream->Write(ldfData.GetData());

	bitStream->Write((bool)true);
	bitStream->Write((ulong)0);

	bitStream->Write((bool)true);
	bitStream->Write((ulong)1);
	
	for (int jx = 0; jx < 4; jx++) {
		bitStream->Write((ulong)0);
	}

	bitStream->Write((ulong)1);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)1);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)18);
	bitStream->Write(charObjectID);
	bitStream->Write(charObjectID);
	bitStream->Write((ulonglong)0);
	bitStream->Write((bool)false);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);
	bitStream->Write((ulong)0);

	bitStream->Write((ulong)0);

	bitStream->Write((bool)true);
	bitStream->Write((ulonglong)1002);

	//SavePacket("ReplicaCharTest1.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
	//rakServer->Send(bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddr, false);
}