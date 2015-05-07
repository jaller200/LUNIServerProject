// Client / Server test of the ReplicaManager class.  See ReplicaManagerP2P for the peer to peer test.
// The ReplicaManager is a very lightweight and efficient system that provides object creation, destruction, and serialization updates such that
// You have all copies of all desired objects on all desired systems semi-automatically

#include "StringTable.h"
// Monster and player are the same, but monster implements the Replica virtual interfaces with a member object, while player derives from Replica
#include "Monster.h"
#include "Player.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include "Rand.h" // RandomMT
#include "ReplicaMember.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "ReplicaEnums.h"
#include "Replica.h"
#include "ReplicaManager.h"
#include "NetworkIDManager.h"

void ShowStatus(Monster *monster, Player *player);

bool isServer;
RakPeerInterface *rakPeer;

// ReplicaManager manages the object replication system
ReplicaManager replicaManager;
// My two game instances
Monster *monster;
Player *player;

#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h" // Developers of a certain platform will know what to do here.
#elif defined(_WIN32)
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

/// Stores a list of network object pointers, and indices to look them up by.
/// This class needs to be registered with both RakPeer and with each created NetworkIDObject
NetworkIDManager networkIDManager;

// This is a required callback I registered to get called when a remote participant calls ReplicaManager::Register on a new object.
// It's equivalent to creating our local copy of an object when a new object is created on a remote system
class ReplicaConstructor : public ReceiveConstructionInterface
{
public:
	ReplicaReturnResult ReceiveConstruction(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, NetworkIDObject *existingObject, SystemAddress senderId, ReplicaManager *caller)
	{
		char output[255];
		// Security - The server needs to pay attention to what objects the client asks to create and reject those that are invalid.
		// In this case the client isn't allowed to create anything
		if (isServer)
			return REPLICA_PROCESSING_DONE;

		// I encoded all the data in inBitStream in Monster::SendConstruction and Player::SendConstruction
		// I am using the string table to send strings, which is a way to send pre-known strings in a single byte.
		// I registered "Player" and "Monster" in main().
		// The stringTable system has the limitation that all systems must register all the same strings in the same order.
		// I could have also used stringCompressor, which would always work but is less efficient to use when we have known strings
		RakNet::StringTable::Instance()->DecodeString(output, 255, inBitStream);
		if (strcmp(output, "Player")==0)
		{
			// The system automatically prevents duplicate creations
			assert(player==0);

			player = new Player;
			// Calling SetNetworkID is important and necessary, because all calls other than construction rely on being able to lookup an object by its network ID
			// If we didn't do this, we would never get scope, serialize, or destruction calls because the ReplicaManager wouldn't be able to find this object
			// networkID was generated from ReplicaManager by calling GetNetworkID on the remote object and automatically including that value in this packet
			player->SetNetworkIDManager(&networkIDManager);
			player->SetNetworkID(networkID);

			if (isServer==false)
			{
				// In order to get Replica::Receive packets the pointer must be registered.  Otherwise these calls are ignored as a security measure
				// Note: If you wanted to actually perform sends on this object to remote systems, call Construct with the isCopy parameter as true instead.
				// replicaManager.ReferencePointer(player);

				// If we want to update an object created remotely, we can call Construct with the last parameter isCopy as true.
				replicaManager.Construct(player, true, senderId, false);

				// Since SendConstruction is not called for copies and we were calling SetScope there, we need to call it here instead.
				replicaManager.SetScope(player, true, senderId, false);
			}

			printf("New player created\n");
		}
		else if (strcmp(output, "Monster")==0)
		{
			// The system automatically prevents duplicate creations
			assert(monster==0);

			monster = new Monster;
			// Calling SetNetworkID is important and necessary, because all calls other than construction rely on being able to lookup an object by its network ID
			// If we didn't do this, we would never get scope, serialize, or destruction calls because the ReplicaManager wouldn't be able to find this object
			// networkID was generated from ReplicaManager by calling GetNetworkID on the remote object and automatically including that value in this packet
			monster->replica->SetNetworkIDManager(&networkIDManager);
			monster->replica->SetNetworkID(networkID);

			if (isServer==false)
			{
				// In order to get Replica::Receive packets the pointer must be registered.  Otherwise these calls are ignored as a security measure
				// Note: If you wanted to actually perform sends on this object to remote systems, call Construct with the isCopy parameter as true instead.
				//replicaManager.ReferencePointer(monster->replica);

				// If we want to update an object created remotely, we can call Construct with the last parameter isCopy as true.
				replicaManager.Construct(monster->replica, true, senderId, false);

				// Since SendConstruction is not called for copies and we were calling SetScope there, we need to call it here instead.
				replicaManager.SetScope(monster->replica, true, senderId, false);
			}		

			printf("New monster created\n");
		}
		else
		{
			// Unknown string
			assert(0);
		}
		return REPLICA_PROCESSING_DONE;
	} 
} constructionCB;


class ReplicaSender : public  SendDownloadCompleteInterface
{
	ReplicaReturnResult SendDownloadComplete(RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress senderId, ReplicaManager *caller)
	{
		// I could write data to outBitStream if I wanted, and it would arrive in inBitStream in ReceiveDownloadCompleteCB
		return REPLICA_PROCESSING_DONE;
	}
} sendDownloadCompleteCB;

class ReplicaReceiver : public  ReceiveDownloadCompleteInterface
{
	ReplicaReturnResult ReceiveDownloadComplete(RakNet::BitStream *inBitStream, SystemAddress senderId, ReplicaManager *caller)
	{
		if (isServer==false) // Server doesn't care about this message, although we get it anyway
			printf("Object downloads complete\n");
		return REPLICA_PROCESSING_DONE;
	}
} receiveDownloadCompleteCB;

int main(void)
{
	char ch;
	char userInput[256];

	rakPeer = RakNetworkFactory::GetRakPeerInterface();

	// You have to attach ReplicaManager for it to work, as it is one of the RakNet plugins
	rakPeer->AttachPlugin(&replicaManager);

	// Used to store the list of created game objects and to look them up by index
	rakPeer->SetNetworkIDManager(&networkIDManager);

	// Anytime we get a new connection, call AddParticipant() on that connection
	replicaManager.SetAutoParticipateNewConnections(true);

	// Anytime we get a new participant, automatically call Construct() for them with all known objects
	replicaManager.SetAutoConstructToNewParticipants(true);

	// When an object goes in scope, serialize it
	replicaManager.SetAutoSerializeInScope(true);

	// Note there is also:
	// replicaManager.SetDefaultScope(true);
	// which I'm not using just for illustration
	// It eliminates the need to call replicaManager.SetScope(this, true, systemAddress); in Replica::SendConstruction.

	// Set the function that will get messages from Replica::SendConstruction
	replicaManager.SetReceiveConstructionCB(&constructionCB);

	// Set the optional callbacks to send and receive download complete notifications
	replicaManager.SetDownloadCompleteCB(&sendDownloadCompleteCB, &receiveDownloadCompleteCB);

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is done in the monster and player SendConstruction functions
	// The decoding is done in the ConstructionCB callback
	// The stringTable class will also send strings that weren't registered but this just falls back
	// to the stringCompressor and wastes 1 extra bit on top of that
	RakNet::StringTable::Instance()->AddString("Player", false); // 2nd parameter of false means a static string so it's not necessary to copy it
	RakNet::StringTable::Instance()->AddString("Monster", false); // 2nd parameter of false means a static string so it's not necessary to copy it

	// Just start the client, or the server
	printf("Demonstration of ReplicaManager for client / server (DEPRECIATED)\n");
	printf("The replica manager provides a framework to make it easier to synchronize\n");
	printf("object creation, destruction, and member object updates\n");
	printf("Difficulty: Intermediate\n\n");
	printf("Run as (s)erver or (c)lient? ");
	gets(userInput);
	if (userInput[0]=='s' || userInput[0]=='S')
	{
		isServer=true;
		SocketDescriptor socketDescriptor(60000,0);
		rakPeer->Startup(8,0,&socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(8);
		printf("Server started.\n");
	}
	else
	{
		isServer=false;
		SocketDescriptor socketDescriptor(0,0);
		rakPeer->Startup(1,0,&socketDescriptor, 1);
		printf("Enter IP to connect to: ");
		gets(userInput);
		if (userInput[0]==0)
		{
			strcpy(userInput, "127.0.0.1");
			printf("%s\n", userInput);
		}
		if (!rakPeer->Connect(userInput, 60000, 0, 0))
		{
			printf("Connect call failed!\n");
			return 1;
		}
		printf("Connecting...\n");
	}

	// The network ID manager will automatically index pointers of object instance NetworkIDObject if
	// SetIsNetworkIDAuthority is called with true.  Otherwise, it will rely on another system setting the IDs
	networkIDManager.SetIsNetworkIDAuthority(isServer);

	printf("Commands:\n(Q)uit\n(Space) Show status\n(R)andomize health and position\n");
	if (isServer)
	{
		printf("Toggle (M)onster\nToggle (p)layer\n");
		printf("Toggle (S)cope of player\n");
	}

	Packet *p;

	// Main loop, go until user signals to quit
	while (1)
	{
		p = rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION || p->data[0]==ID_CONNECTION_LOST)
			{
				if (isServer==false)
				{
					printf("Server connection lost.  Deleting objects\n");
					// Lets have the clients delete all their objects when they lose connection to the server
					if (monster)
					{
						delete monster; // Destructor of monster calls Dereplicate
					}
					if (player)
					{
						delete player; // Destructor of player calls Dereplicate
					}
				}
			}
			rakPeer->DeallocatePacket(p);
			p = rakPeer->Receive();
		}

		if (kbhit())
		{
			ch=getch();
			if (ch=='q' || ch=='Q')
			{
				printf("Quitting.\n");
				break;
			}
			else if (ch==' ')
				ShowStatus(monster, player);
			else if (ch=='r' || ch=='R')
			{
				// Randomize health and position of the player and monster
				if (player)
				{
					player->health=randomMT();
					player->position=randomMT();

					// This system requires that you tell the manager when object memory has changed, so it will queue up a serialize call
					replicaManager.SignalSerializeNeeded(player, UNASSIGNED_SYSTEM_ADDRESS, true); // UNASSIGNED_SYSTEM_ADDRESS, true means everybody
				}
				if (monster)
				{
					monster->health=randomMT();
					monster->position=randomMT();

					// This system requires that you tell the manager when object memory has changed, so it will queue up a serialize call
					replicaManager.SignalSerializeNeeded(monster->replica, UNASSIGNED_SYSTEM_ADDRESS, true); // UNASSIGNED_SYSTEM_ADDRESS, true means everybody
				}
				printf("Randomized player and monster health and position\n");
				ShowStatus(monster, player);
			}
			else if (isServer)
			{
				if (ch=='m' || ch=='M')
				{
					if (monster==0)
					{
						printf("Creating monster\n");
						monster = new Monster;
					}
					else
					{

						delete monster;
						printf("Deleted monster\n");
						monster=0;
					}
				}
				else if (ch=='p' || ch=='P')
				{
					if (player==0)
					{
						printf("Creating player\n");
						player = new Player;

					}
					else
					{
						delete player;
						printf("Deleted player\n");
						player=0;
					}
				}
				else if (ch=='s' || ch=='S')
				{
					if (player)
					{
						bool currentScope;
						currentScope=replicaManager.IsInScope(player, rakPeer->GetSystemAddressFromIndex(0));
						if (currentScope==false)
							printf("Setting scope for player to true for all remote systems.\n");
						else
							printf("Setting scope for player to false for all remote systems.\n");
						replicaManager.SetScope(player, !currentScope, UNASSIGNED_SYSTEM_ADDRESS, true);
					}
					else
					{
						printf("No player to set scope for\n");
					}
				}
			}
		}
#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif
	}

	delete monster;
	delete player;
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 1;
}

void ShowStatus(Monster *monster, Player *player)
{
	printf("\nSTATUS:\n");
	if (monster)
		printf("Monster is at position %i with health %i\n", monster->position, monster->health);
	else
		printf("There is no monster\n");

	if (player)
		printf("Player is at position %i with health %i\n", player->position, player->health);
	else
		printf("There is no player\n");

	if (isServer)
	{
		bool monsterInScope, playerInScope;
		if (monster)
		{
			// Note that when using a member object, I pass that member object to the ReplicaManager functions
			monsterInScope=replicaManager.IsInScope(monster->replica, rakPeer->GetSystemAddressFromIndex(0));

			// If an object is in scope we will send memory updates for that object to that player.
			if (monsterInScope)
				printf("Monster in scope to system 0.\n");
			else
				printf("Monster NOT in scope to system 0.\nChanges to monster variables will not be sent to that system.\n");
		}
		if (player)
		{
			// For regular inheritance I don't have to do that.
			playerInScope=replicaManager.IsInScope(player, rakPeer->GetSystemAddressFromIndex(0));

			if (playerInScope)
				printf("Player in scope to system 0.\n");
			else
				printf("Player NOT in scope to system 0.\nChanges to player variables will not be sent to that system.\n");
		}
	}
	printf("\n");
}
