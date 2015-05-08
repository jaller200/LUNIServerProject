#include "TestObjects.h"

#include "StringTable.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "FormatString.h"
#include "RakAssert.h"
#include "StringCompressor.h"

DataStructures::List<User*> User::users;
User *User::myUser;
DataStructures::List<Soldier*> Soldier::soldiers;
Soldier *Soldier::mySoldier;

// ReplicaManager2 requires NetworkIDManager to lookup pointers
extern NetworkIDManager networkIdManager;
// Is this console app running as a server? false means client
extern bool isServer;
// Each application has one instance of RakPeerInterface
extern RakPeerInterface *rakPeer;
// The system that performs most of our functionality for this demo
extern ReplicaManager2 replicaManager;

// Track this pointer, which user owns me, and the initial value for my name
Soldier::Soldier()
{
	soldiers.Insert(this);
	owner=0;
	strcpy(name, "DefaultName");
	isCloaked=false;
}
// Stop tracking this pointer
Soldier::~Soldier()
{
	soldiers.RemoveAtIndex(soldiers.GetIndexOf(this));
	if (Soldier::mySoldier==this)
	{
		printf("My soldier deleted.\n");
		Soldier::mySoldier=0;
	}
	else
		printf("Soldier named %s deleted.\n", name);
	if (owner)
		owner->soldier=0;
}

// Return the user that owns me (shouldn't be NULL)
User* Soldier::GetUser(void) const
{
	return owner;
}
// Set a new name for myself
void Soldier::SetName(char *newName)
{
	strcpy(name, newName);
}

// Cloak or uncloak. Other systems won't get changes to this soldier while cloaked.
void Soldier::SetCloak(bool b)
{
	isCloaked=b;
}

// Implemented member of Replica2: This function encodes the identifier for this class, so the class factory can create it
bool Soldier::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("Soldier",128,bitStream);
	return true;
}

// Implemented member of Replica2: Write the data members of this class. ReplicaManager2 works with strings as well as pointers
bool Soldier::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	// Both the client and the server write the name.
	// The client writes it to send it to the server
	// The server writes the name when sending to systems other than the one that owns this soldier.
	if (isServer==false || serializationContext->recipientAddress!=owner->systemAddress)
		StringCompressor::Instance()->EncodeString(name,sizeof(name),bitStream);

	// Only the owner client needs to write the status of cloak, since this variable is only used by the server
	if (isServer==false && Soldier::mySoldier==this)
		bitStream->Write(isCloaked);
	
	// Only the server writes the owner
	if (isServer)
		bitStream->Write(owner->GetNetworkID());

	return true;
}

// Implemented member of Replica2: Read what I wrote in Serialize() immediately above
void Soldier::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	NetworkID ownerNetworkId;

	// Similar to Serialize(), only read the name if the server, or if we are not the owner
	if (isServer || Soldier::mySoldier!=this)
		StringCompressor::Instance()->DecodeString(name,sizeof(name),bitStream);

	// Only the server reads isCloaked
	if (isServer)
		bitStream->Read(isCloaked);

	// Only clients read the owner
	if (isServer==false)
	{
		bitStream->Read(ownerNetworkId);
		owner=(User*) networkIdManager.GET_OBJECT_FROM_ID(ownerNetworkId);
		owner->soldier=this;
	}

	printf("Soldier %s updated.\n", name);
}

// Implemented member of Replica2: Should this object be visible to this connection? If not visible, Serialize() will not be sent to that system.
BooleanQueryResult Soldier::QueryVisibility(Connection_RM2 *connection)
{
	if (isServer)
		return isCloaked==false ? BQR_YES : BQR_NO;
	else
		return BQR_ALWAYS; // You never cloak your own updates to the server
}
// Implemented member of Replica2: Called when our visibility changes. While not visible, we will not get updates from Serialize() for affected connection(s)
void Soldier::DeserializeVisibility(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	if (owner)
	{
		if (SerializationContext::IsVisible(serializationType))
			printf("Soldier named %s owned by user at address %s has become visible.\n", name, owner->systemAddress.ToString());
		else
			printf("Soldier named %s owned by user at address %s no longer visible.\n", name, owner->systemAddress.ToString());
	}
	else
	{
		// Visibility message comes in before deserialize, and it is deserialize that contains the owner.
		// However, construction comes before visibility.
		// We could have thus just written the owner in Soldier::SerializeConstruction() and read and assigned it in ReplicaManager2DemoConnection::Construct()
		if (SerializationContext::IsVisible(serializationType))
			printf("Soldier named %s has become visible.\n", name);
		else
			printf("Soldier named %s is no longer visible.\n", name);
	}
}

bool Soldier::QueryIsDestructionAuthority(void) const
{
	// Since we allow the controlling client to locally destroy the soldier, and transmit this across the network, the client must be the destruction authority
	// Were this function not overridden, if the client deleted the soldier it would only be deleted locally.
	// If the server were the only system deleting this object, it would not be necessary to override this function
	return isServer || Soldier::mySoldier==this;
}

bool Soldier::QueryIsVisibilityAuthority(void) const
{
	// Same as QueryIsDestructionAuthority, but for QueryVisibility.
	return isServer || Soldier::mySoldier==this;
}

bool Soldier::QueryIsSerializationAuthority(void) const
{
	// Same as QueryIsDestructionAuthority, but for Serialize and AddAutoSerializeTimer.
	return isServer || Soldier::mySoldier==this;
}

// User starts out with no soldier. Also, track this pointer.
User::User()
{
	soldier=0;
	users.Insert(this);
	systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
}

// Remove myself from the static users list
User::~User()
{
	users.RemoveAtIndex(users.GetIndexOf(this));

	// When the user logs off his soldier should go away too
	if (soldier)
	{
		// Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
		// It is in the derived class QueryIsDestructionAuthority() that we give the client authority to network delete the object
		soldier->BroadcastDestruction();

		delete soldier;
	}

	// Store myUser so the client knows which user refers to him
	if (User::myUser==this)
	{
		// Shouldn't ever see this
		printf("My user deleted.\n");
		Soldier::mySoldier=0;
	}
	else
	{
		printf("User with address %s deleted.\n", systemAddress.ToString());
	}
}

// Helper function to free memory when someone disconnections.
void User::DeleteUserByAddress(SystemAddress systemAddress)
{
	for (unsigned i=0; i < users.Size(); i++)
	{
		if (users[i]->systemAddress==systemAddress)
		{
			// Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
			// It is in the derived class QueryIsDestructionAuthority() that we give the client authority to network delete the object
			users[i]->BroadcastDestruction();

			delete users[i];
			return;
		}
	}
}

// Helper function to lookup the user in users by systemAddress
User* User::GetUserByAddress(SystemAddress systemAddress)
{
	for (unsigned i=0; i < users.Size(); i++)
	{
		if (users[i]->systemAddress==systemAddress)
		{
			return users[i];
		}
	}
	return 0;
}

// Each user can have a soldier. Create my soldier if I don't have one already
Soldier* User::CreateSoldier(void)
{
	if (soldier==0)
	{
		soldier = new Soldier;
		return soldier;
	}
	return 0;
}
// Return my soldier
Soldier* User::GetMySoldier(void) const
{
	return soldier;
}

// Implemented member of Replica2: This function encodes the identifier for this class, so the class factory can create it
bool User::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	StringTable::Instance()->EncodeString("User",128,bitStream);
	return true;
}
// Implemented member of Replica2: Write the data members of this class. ReplicaManager2 works with pointers as well as any other kind of data
bool User::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	if (soldier)
		bitStream->Write(soldier->GetNetworkID());
	else
		bitStream->Write(UNASSIGNED_NETWORK_ID);
	bitStream->Write(systemAddress);
	return true;
}
// Implemented member of Replica2: Read what I wrote in Serialize() immediately above
void User::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	NetworkID soldierNetworkId;
	bitStream->Read(soldierNetworkId);
	soldier=(Soldier*) networkIdManager.GET_OBJECT_FROM_ID(soldierNetworkId);
	bitStream->Read(systemAddress);
	// The client stores a pointer to its own user, for convenience
	if (systemAddress==rakPeer->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS))
		User::myUser=this;
	// The soldier pointer may be NULL, since you spawn in soldiers, rather than have them created immediately on connection as are Users
	if (soldier)
		printf("User at address %s updated. Has soldier with name: %s.\n", systemAddress.ToString(), soldier->name);
	else
		printf("User at address %s updated. No soldier spawned.\n", systemAddress.ToString(), soldier->name);
}
// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
Replica2* ReplicaManager2DemoConnection::Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
{
	char objectName[128];
	// In our two Serialize() functions above, we used StringTable to encode the name of the class. Here we read it out, and create the desired object.
	StringTable::Instance()->DecodeString(objectName,128,replicaData);
	if (networkIDCollision) return 0;
	if (strcmp(objectName,"User")==0) return new User;
	if (strcmp(objectName,"Soldier")==0) {
		Soldier *soldier = new Soldier;
		if (isServer)
		{
			// If the server, then we can assign the owner by the address of the guy sending to us
			// For the client, we read this in Soldier::Deserialize
			soldier->owner=User::GetUserByAddress(sender);
			soldier->owner->soldier=soldier;
		}		
		return soldier;
	};
	return 0;
}
// Callback when we finish downloading all objects from a new connection
// See Connection_RM2::ReceiveDownload in ReplicaManager2.h for a full explanation of each parameter
void ReplicaManager2DemoConnection::DeserializeDownloadComplete(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType)
{
	// Since the system is inherently peer to peer, download notifications go both ways, not just server to client.
	// Just ignore download notifications from clients in a client/server app
	if (isServer==false)
		printf("Download complete\n");
}
