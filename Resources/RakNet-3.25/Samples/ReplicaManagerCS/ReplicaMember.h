#include "Replica.h"

// An implementation of Replica that passes all calls up to a parent class
// Used if I don't want my game class to inherit from Replica
class ReplicaMember : public Replica
{
	virtual ReplicaReturnResult SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp );
	virtual ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp );
	virtual ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	virtual ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
	virtual ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
	virtual ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
	virtual ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );
	// Safety check so the user does not forget to call SetParent
	virtual bool RequiresSetParent(void) const;
	virtual int GetSortPriority(void) const {return 0;}
};
