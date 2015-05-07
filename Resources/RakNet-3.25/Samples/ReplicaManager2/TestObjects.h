#ifndef __TEST_OBJECTS_H
#define __TEST_OBJECTS_H

class User;

#include "ReplicaManager2.h"

// NetworkIDManager2 is in the namespace RakNet
using namespace RakNet;

// Soldier is a class that a User can create
class Soldier : public Replica2
{
public:
	// All created soldiers
	static DataStructures::List<Soldier*> soldiers;
	// My soldier (client only)
	static Soldier *mySoldier;
	// The user that owns this soldier
	User *owner;
	// Name of this soldier
	char name[128];
	// Is this soldier cloaked? While cloaked, changes will not be sent with the Serialize() function
	bool isCloaked;

	// Track this pointer, which user owns me, and the initial value for my name
	Soldier();
	// Stop tracking this pointer
	virtual ~Soldier();

	// Return the user that owns me (shouldn't be NULL)
	User* GetUser(void) const;
	// Set a new name for myself
	void SetName(char *newName);
	// Cloak or uncloak. Other systems won't get changes to this soldier while cloaked.
	void SetCloak(bool b);

	// Implemented member of Replica2: This function encodes the identifier for this class, so the class factory can create it
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	// Implemented member of Replica2: Write the data members of this class. ReplicaManager2 works with strings as well as pointers
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	// Implemented member of Replica2: Read what I wrote in Serialize() immediately above
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);
	// Implemented member of Replica2: Should this object be visible to this connection? If not visible, Serialize() will not be sent to that system.
	virtual BooleanQueryResult QueryVisibility(Connection_RM2 *connection);
	// Implemented member of Replica2: Called when our visibility changes. While not visible, we will not get updates from Serialize() for affected connection(s)
	virtual void DeserializeVisibility(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);
	
	// Overriding so the client can delete this object. By default, only the server could
	virtual bool QueryIsDestructionAuthority(void) const;

	// Overriding so the client can control if this object is visible or not (for cloaking). By default, only the server control this
	// Objects that are not visible are not serialized automatically with ReplicaManager2::AutoSerialize()
	virtual bool QueryIsVisibilityAuthority(void) const;

	// Overriding so the client can send serialization changes for its own soldier. By default, only the server can send serialization changes.
	virtual bool QueryIsSerializationAuthority(void) const;
};

// User represents a human behind a computer. One instance is created on the server per new connection.
class User : public Replica2
{
public:
	// Holds a pointer to my soldier.
	Soldier *soldier;
	// All users that have been created
	static DataStructures::List<User*> users;
	// Does this user represent me? Used on the client only.
	static User *myUser;
	// System address of this user
	SystemAddress systemAddress;

	// User starts out with no soldier. Also, track this pointer.
	User();
	// Remove myself from the static users list
	virtual ~User();

	// Helper function to free memory when someone disconnections.
	static void DeleteUserByAddress(SystemAddress systemAddress);

	// Helper function to lookup the user in users by systemAddress
	static User* GetUserByAddress(SystemAddress systemAddress);

	// Each user can have a soldier. Create my soldier if I don't have one already
	Soldier* CreateSoldier(void);
	// Return my soldier
	Soldier* GetMySoldier(void) const;

	// Implemented member of Replica2: This function encodes the identifier for this class, so the class factory can create it
	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	// Implemented member of Replica2: Write the data members of this class. ReplicaManager2 works with pointers as well as any other kind of data
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext);
	// Implemented member of Replica2: Read what I wrote in Serialize() immediately above
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp);
};

// One instance of Connection_RM2 is implicitly created per connection that uses ReplicaManager2. The most important function to implement is Construct() as this creates your game objects.
// It is designed this way so you can override per-connection behavior in your own game classes
class ReplicaManager2DemoConnection : public Connection_RM2
{
	// Callback used to create objects
	// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
	Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision);

	// Callback when we finish downloading all objects from a new connection
	// See Connection_RM2::DeserializeDownloadComplete in ReplicaManager2.h for a full explanation of each parameter
	virtual void DeserializeDownloadComplete(RakNet::BitStream *objectData, SystemAddress sender, ReplicaManager2 *replicaManager, RakNetTime timestamp, SerializationType serializationType);

};

// This is a required class factory, that creates and destroys instances of ReplicaManager2DemoConnection
class ReplicaManager2DemoConnectionFactory : public Connection_RM2Factory {
	virtual Connection_RM2* AllocConnection(void) const {return new ReplicaManager2DemoConnection;}
	virtual void DeallocConnection(Connection_RM2* s) const {delete s;}
};

#endif
