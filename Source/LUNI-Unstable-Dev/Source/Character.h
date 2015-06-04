#pragma once

//#include "serverLoop.h"
#include "World.h"
#include "WorldPackets.h"
#include "Packet.h"

#include "RakNet\RakNetTypes.h"
#include "RakNet\PacketPriority.h"
#include "RakNet\ReplicaEnums.h"
#include "RakNet\Replica.h"

namespace RakNet
{
	class BitStream;
}

class Character : public Replica {
private: 
	// Functions
	//Ref<Character> InitializeCharacter(ulonglong objectID, RakPeerInterface *rakServer, SystemAddress &systemAddr);
	ulong charLOT = 1;

	// Stats table (I know this is long - it's for myself really. Want it to look like a table)
	ulonglong totalCurrencyCollected;
	ulonglong bricksSmashed;
	ulonglong completedQuickBuilds;
	ulonglong enemiesSmashed;
	ulonglong rocketsUsed;
	ulonglong missionsCompleted;
	ulonglong petsTamed;
	ulonglong imaginationCollected;
	ulonglong lifeCollected;
	ulonglong shieldCollected;
	ulonglong totalDistanceTravelled; // In meters
	ulonglong timesSmashed;
	ulonglong totalDamageTaken;
	ulonglong totalDamageHealed;
	ulonglong totalArmorRepaired;
	ulonglong totalImaginationRestored;
	ulonglong totalImaginationUsed;
	ulonglong totalDistanceDriven; // In meters
	ulonglong totalTimesAirborn; // In seconds (for racing)
	ulonglong racingPowerupsCollected;
	ulonglong racingCratesSmashed;
	ulonglong numberBoostsActivated;
	ulonglong numberWrecks;
	ulonglong racingSmashablesSmashed;
	ulonglong racesFinished;
	ulonglong racesWon;

public:
	Character();
	~Character();

	// ReplicaManager Functions
	ReplicaReturnResult SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp);
	ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
	ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress);

	// These aren't used...
	ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);

	int GetSortPriority(void) const { return 0; }

	Character(ulonglong objectID);
	Character(const Character& character);

	//Ref< Character > InitializeCharacter(ulonglong objectID);

	//void WriteInitPacket(RakPeerInterface *rakServer, SystemAddress &systemAddr, RakNet::BitStream *bitStream);
	void WriteInitPacket(RakNet::BitStream *bitStream);
	void WriteCharDataPacket(RakPeerInterface *rakServer, SystemAddress &systemAddr);
	void CreateXMLData(RakNet::BitStream *dataStream);

	void TestReplicaInitPacket(RakNet::BitStream *bitStream);

	ulonglong charObjectID; // The char's NetworkID (called ObjectID by humanoid and lcdr)
	std::wstring charName; // The char's name

	ushort currentWorldID;
	ushort currentWorldInstance;

	// I do not believe the free version of RakNet 3.25 has
	// Vec3f and Quaternion classes... Correct me if I am wrong please!
	float posX, posY, posZ; // The char's position (should be Vec3f)
	float rotX, rotY, rotZ, rotW; // The char's rotation (should be Quaternion)
	
	float currentHealth, currentShield, currentImagination; // The current values of health, shield, and imagination
	float maxHealth, maxShield, maxImagination; // The max values for each of these items

	ulong playerLevel; // The level of the character based on U-Score

	// Char details
	ulong hairColor, hairStyle, unknown1; // The head items
	ulong shirtColor, pantsColor, unknown2; // The body items
	ulong eyebrows, eyes, mouth; // The facial items

	ulong factionID; // The faction id of the player??

	ulonglong charAccountID; // This is the ID of the account the character is saved to
	ulonglong llog; // Could be the timestamp??

	ulonglong currentUScore; // The current Universe Score that the player has
	bool freeToPlay; // Is the user free to play?
	bool invisible; // If set to 1, it makes the character invisible
	bool teamMember, isMythran, mythranItems; // Is the character a team leader or mythran?

	// Items
	ulong itemCount; // The count of the items
	ulonglong shirtObjectID, neckObjectID, headObjectID, pantsObjectID, lhObjectID, rhObjectID; // The NetworkIDs of the objects
	ulong shirtID, neckID, headID, pantsID, lhID, rhID; // The LOTs of the objects

	std::vector<std::string> userFriendIDs;

	static Ref< Character > InitializeCharacter(ulonglong objectID);
	void CreateFriendsList(RakPeerInterface *rakServer, const SystemAddress& systemAddr, ulonglong currentObjectID);

};