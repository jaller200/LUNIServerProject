// A class demonstrating using a derived instance of Replica as a member object.
// All Replica class functions are here and are called from replica

class ReplicaMember;
#include "RakNetTypes.h"
#include "PacketPriority.h"
#include "ReplicaEnums.h"

namespace RakNet
{
	class BitStream;
};

class Monster
{
public:
	Monster();
	~Monster();

	// These functions are all from the Replica interface in ReplicaManager.h
	// They are user defined pure virtual implementations that perform processing that the ReplicaManager system requests.
	// In this case, I'm implementing them in the member variable ReplicaMember and calling up to this class so I don't have to inherit from Replica
	ReplicaReturnResult SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp );
	ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
	ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
	ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );

	// The other way to use the replication system is to include that class as a member variable and call SetParent
	ReplicaMember *replica;

	// Game specific data
	int position;
	int health;
};
