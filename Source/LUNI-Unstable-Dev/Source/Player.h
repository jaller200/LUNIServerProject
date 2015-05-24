#pragma once

#include "RakNet\ReplicaManager2.h"

#include "serverLoop.h"

using namespace RakNet;

class Player : public Replica2
{
public:
	static DataStructures::List<Player*> players;

	static Player *myPlayer;
	SystemAddress systemAddress;

	signed int LOT;

	Player();
	virtual ~Player();

	static Player* GetPlayerByAddress(SystemAddress systemAddress);
	static void DeletePlayerByAddress(SystemAddress systemAddress);

	void SetLOT(signed int assignedLOT);

	// Replica2 Methods
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);
};

class LUNIServerConnection : public Connection_RM2
{
	Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkID, bool networkIDCollision);
};

class LUNIServerConnectionFactory : public Connection_RM2Factory {
	virtual Connection_RM2* AllocConnection(void) const { return new LUNIServerConnection; }
	virtual void DeallocConnection(Connection_RM2* s) const { delete s; }
};