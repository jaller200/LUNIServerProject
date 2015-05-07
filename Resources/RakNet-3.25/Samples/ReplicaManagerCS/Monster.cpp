#include "Monster.h"
#include "ReplicaMember.h"
#include "BitStream.h"
#include "StringTable.h"
#include "ReplicaManager.h"
#include <stdio.h>

extern bool isServer;
extern ReplicaManager replicaManager;
extern Monster *monster;

Monster::Monster()
{
	// Setup my object
	position=1;
	health=2;

	replica = new ReplicaMember;
	// Only necessary if using the replication system by containing a member variable
	replica->SetParent(this);

	// Objects are only processed by the system after you tell the manager to replicate them.
	// Here I do it in the constructor, but in a real game you would probably do it after the object is done loading
	// This does NOT call serialize automatically - it only sends a call to create the remote object.
	if (isServer)
		replicaManager.Construct(replica, false, UNASSIGNED_SYSTEM_ADDRESS, true);

	// Calling
	// replicaManager.SetScope(replica, true, UNASSIGNED_SYSTEM_ADDRESS)
	// here would work for existing players, but not for future players.
	// So we call it in SendConstruction instead, which is called once for all players.

	if (isServer)
	{
		// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
		// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
		replicaManager.DisableReplicaInterfaces(replica, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE);
	}
	else
	{
		// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
		// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
		replicaManager.DisableReplicaInterfaces(replica, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE);
	}
}
Monster::~Monster()
{
	printf("Inside ~Monster\n");
	// Remove the object before it is destroyed, or you will crash the next time the ReplicaManager updates
	replicaManager.Destruct(replica, UNASSIGNED_SYSTEM_ADDRESS, true);
	replicaManager.DereferencePointer(replica);
	delete replica;
	monster=0;
}

ReplicaReturnResult Monster::SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )
{
	// This string was pre-registered in main with RakNet::StringTable::Instance()->AddString so we can send it with the string table and save bandwidth
	RakNet::StringTable::Instance()->EncodeString("Monster", 255, outBitStream);

	// This means that this object starts in scope to this player.  In a real game, you would only start in scope if an object
	// were immediately visible to that player.  This only really applies to the server in a client/server game, since clients wouldn't hide their
	// own object updates from the server.
	// We could have left this line out by calling ReplicaManager::SetDefaultScope(true); in main()
	replicaManager.SetScope(replica, true, systemAddress, false);

	printf("Sending monster to %i:%i\n", systemAddress.binaryAddress, systemAddress.port);

	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
{
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	delete monster;

	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
{
	if (inScope)
		printf("Sending scope change to true in Monster\n");
	else
		printf("Sending scope change to false in Monster\n");
	// Up to you to write this.  If you write nothing, the system will treat that as if you wanted to cancel the scope change
	outBitStream->Write(inScope);
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
{
	bool inScope;
	inBitStream->Read(inScope);
	if (inScope)
		printf("Received message that scope is now true in Monster\n");
	else
		printf("Received message that scope is now false in Monster\n");
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
{
	if (lastSendTime==0)
		printf("First call to Monster::Serialize for %i:%i\n", systemAddress.binaryAddress, systemAddress.port);

	outBitStream->Write(position);
	outBitStream->Write(health);
	return REPLICA_PROCESSING_DONE;
}
ReplicaReturnResult Monster::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
{
	if (lastDeserializeTime==0)
		printf("First call to Monster::Deserialize\n");
	else
		printf("Got Monster::Deserialize\n");

	inBitStream->Read(position);
	inBitStream->Read(health);
	return REPLICA_PROCESSING_DONE;
}