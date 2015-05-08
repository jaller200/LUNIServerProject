#include "ReplicaMember.h"
#include "Monster.h"

extern bool isServer;

// This file demonstrates using an object of class Replica as a member of another object, rather than inheriting it.
// All I do here is pass all calls up to the parent.
// In this case I know the parent is of class Monster, but in a real game you'd use Entity or Object or whatever your base network class is.
ReplicaReturnResult ReplicaMember::SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )
{
	return ((Monster*)GetParent())->SendConstruction(currentTime, systemAddress, flags, outBitStream, includeTimestamp);
}
ReplicaReturnResult ReplicaMember::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
{
	return ((Monster*)GetParent())->SendDestruction( outBitStream, systemAddress, includeTimestamp);
}
ReplicaReturnResult ReplicaMember::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	return ((Monster*)GetParent())->ReceiveDestruction(inBitStream, systemAddress, timestamp);
}
ReplicaReturnResult ReplicaMember::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
{
	return ((Monster*)GetParent())->SendScopeChange(inScope, outBitStream, currentTime, systemAddress, includeTimestamp);
}
ReplicaReturnResult ReplicaMember::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	return ((Monster*)GetParent())->ReceiveScopeChange(inBitStream, systemAddress, timestamp);
}
ReplicaReturnResult ReplicaMember::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
{
	return ((Monster*)GetParent())->Serialize(sendTimestamp, outBitStream, lastSendTime, priority, reliability, currentTime, systemAddress, flags);
}
ReplicaReturnResult ReplicaMember::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
{
	return ((Monster*)GetParent())->Deserialize(inBitStream, timestamp, lastDeserializeTime, systemAddress);
}
bool ReplicaMember::RequiresSetParent(void) const
{
	return true;
}
