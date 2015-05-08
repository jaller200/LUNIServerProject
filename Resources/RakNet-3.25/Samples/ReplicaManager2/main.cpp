// Demonstrates ReplicaManager 2: A system to automatically create, destroy, and serialize objects
// In this sample, the server creates a User class instance for each new login. The user is constructed on all systems, and the SystemAddress of this user is stored.
// Clients that connect create a Soldier class instance. The name member is set by the client. The User pointer is set by the server.
// Once a soldier is created, clients can cloak or uncloak the soldier by setting the isCloaked member variable. The server uses this to determine visibility.
// Finally, when a client disconnects, the User and Soldier associated with that client are deleted.
// The sample uses the autoSerialization feature to determine when to transmit new names, or new User pointers.

#include "StringTable.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "ReplicaManager2.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "FormatString.h"
#include "StringCompressor.h"
#include "TestObjects.h"

// ReplicaManager2 is in the namespace RakNet
using namespace RakNet;

// ReplicaManager2 requires NetworkIDManager to lookup pointers from numerical.
NetworkIDManager networkIdManager;
// Is this console app running as a server? false means client
bool isServer;
// Each application has one instance of RakPeerInterface
RakPeerInterface *rakPeer;
// The system that performs most of our functionality for this demo
ReplicaManager2 replicaManager;

class User;

// Instance of the class that creates the object we use to represent connections
ReplicaManager2DemoConnectionFactory connectionFactory;

int main(void)
{
	char ch;
	SocketDescriptor sd;
	char ip[128];
	static const int SERVER_PORT=12345;
	Soldier::mySoldier=0;
	User::myUser=0;

	printf("Demonstration of ReplicaManager2.\n");
	printf("1. Demonstrates creating objects created by the server and client.\n");
	printf("2. Demonstrates automatic serialization of pointers and data members\n");
	printf("3. Demonstrates serialization visibility scoping.\n");
	printf("This sample uses client/server.\nPeer to peer and clustered servers are also supported.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Start as (c)lient or (s)erver? ");
	ch=getche();

	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	if (ch=='c' || ch=='C')
	{
		isServer=false;
		sd.port=0;
	}
	else
	{
		isServer=true;
		sd.port=SERVER_PORT;
	}

	// ObjectMemberRPC, AutoRPC for objects, and ReplicaManager2 require that you call SetNetworkIDManager()
	rakPeer->SetNetworkIDManager(&networkIdManager);
	// The network ID authority is the system that creates the common numerical identifier used to lookup pointers.
	// For client/server this is the server
	// For peer to peer this would be true on every system, and you would also have call NetworkID::peerToPeerMode=true;
	networkIdManager.SetIsNetworkIDAuthority(isServer);
	// Start RakNet, up to 32 connections if the server
	rakPeer->Startup(isServer ? 32 : 1,100,&sd,1);
	rakPeer->AttachPlugin(&replicaManager);
	// Just test this
	replicaManager.SetAutoAddNewConnections(false);
	// Register our custom connection factory
	replicaManager.SetConnectionFactory(&connectionFactory);
	// The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
	if (isServer)
		rakPeer->SetMaximumIncomingConnections(32);

	printf("\n");
	if (isServer==false)
	{
		printf("Enter server IP: ");
		gets(ip);
		if (ip[0]==0)
			strcpy(ip, "127.0.0.1");
		rakPeer->Connect(ip,SERVER_PORT,0,0,0);
		printf("Connecting...\n");
	}
	

	// Here I use the string table class to efficiently send strings I know in advance.
	// The encoding is used in in Replica2::SerializeConstruct
	// The decoding is used in in Connection_RM2::Construct
	// The stringTable class will also send strings that weren't registered but this just falls back to the stringCompressor and wastes 1 extra bit on top of that
	// 2nd parameter of false means a static string so it's not necessary to copy it
	RakNet::StringTable::Instance()->AddString("Soldier", false);
	RakNet::StringTable::Instance()->AddString("User", false);

	if (isServer)
		printf("Commands:\n(Q)uit\n'P'rint out all objects\n");
	else
		printf("Commands:\n(Q)uit\n'P'rint out all objects\n(S)pawn / despawn soldier\n(C)loak toggle\n(R)ename your soldier\n");

	// Enter infinite loop to run the system
	Packet *packet;
	bool quit=false;
	while (!quit)
	{
		for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("ID_CONNECTION_ATTEMPT_FAILED\n");
				quit=true;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				quit=true;
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				// Necessary with SetAutoAddNewConnections(false);
				replicaManager.AddNewConnection(packet->systemAddress);
				break;
			case ID_NEW_INCOMING_CONNECTION:
				{
					printf("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString());
					// Necessary with SetAutoAddNewConnections(false);
					replicaManager.AddNewConnection(packet->systemAddress);
					// The server should create a User class for each new connection.
					User *newUser = new User;
					// In order to use any networked member functions of Replica2, you must first call SetReplicaManager
					newUser->SetReplicaManager(&replicaManager);
					// Store which address this user is associated with
					newUser->systemAddress=packet->systemAddress;
					// Tell the user to automatically serialize our data members every 100 milliseconds (if changed)
					// This way if we change the system address or the Soldier* we don't have to call user->BroadcastSerialize();
					newUser->AddAutoSerializeTimer(100);
					// Send out this new user to all systems. Unlike the old system (ReplicaManager) all sends are done immediately.
					newUser->BroadcastConstruction();
				}				
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				// Destruction broadcast done automatically in the destructor, from Replica2
				User::DeleteUserByAddress(packet->systemAddress);
				break;
			case ID_CONNECTION_LOST:
				printf("ID_CONNECTION_LOST\n");
				// Destruction broadcast done automatically in the destructor, from Replica2
				User::DeleteUserByAddress(packet->systemAddress);
				break;
			}
		}

		if (kbhit())
		{
			ch=getch();
			if (ch=='q' || ch=='Q')
			{
				printf("Quitting.\n");
				quit=true;
			}
			else if (ch=='p' || ch=='P' || ch==' ')
			{
				printf("%i Soldiers:\n", Soldier::soldiers.Size());
				for (unsigned i=0; i < Soldier::soldiers.Size(); i++)
					printf("%i. ptr=%p name=%s isCloaked=%i owner=%p\n", i+1, Soldier::soldiers[i], Soldier::soldiers[i]->name, Soldier::soldiers[i]->isCloaked, Soldier::soldiers[i]->owner);

				printf("%i Users:\n", User::users.Size());
				for (unsigned i=0; i < User::users.Size(); i++)
					printf("%i. ptr=%p systemAddress=%s soldier=%p\n", i+1, User::users[i], User::users[i]->systemAddress.ToString(), User::users[i]->soldier);

			}
			if (isServer==false)
			{
				if (ch=='s' || ch=='S')
				{
					if (Soldier::mySoldier!=0)
					{
						printf("Despawning our soldier.\n");

						// Unfortunately BroadcastDestruction() cannot be called automatically in the destructor of Replica2, because virtual functions can not call to derived classes.
						// It is in the derived class QueryIsDestructionAuthority() that we give the client authority to network delete the object
						Soldier::mySoldier->BroadcastDestruction();

						delete Soldier::mySoldier;
					}
					else
					{
						// Spawn a new soldier
						Soldier::mySoldier = new Soldier;
						printf("Creating our soldier. What name should (s)he have?: ");
						gets(Soldier::mySoldier->name);
						// In order to use any networked member functions of Replica2, you must first call SetReplicaManager
						Soldier::mySoldier->SetReplicaManager(&replicaManager);
						// Tell the system to automatically serialize our data members every 100 milliseconds (if changed)
						// This way if we change the soldier's name we don't have to call Soldier::mySoldier->BroadcastSerialize();
						Soldier::mySoldier->AddAutoSerializeTimer(100);
						// Send out this new user to all systems. Unlike the old system (ReplicaManager) all sends are done immediately.
						Soldier::mySoldier->BroadcastConstruction();
					}
				}
				else if (ch=='c' || ch=='C')
				{
					if (Soldier::mySoldier!=0)
					{
						Soldier::mySoldier->isCloaked=!Soldier::mySoldier->isCloaked;

						if (Soldier::mySoldier->isCloaked)
							printf("Cloaked. This variable on the server will block calls to Serialize().\n");
						else
							printf("No longer cloaked. Other systems will immediately update.\n");
					}
					else
					{
						printf("No soldier to cloak! Create a soldier with 's'\n");
					}
				}
				else if (ch=='r' || ch=='R')
				{
					if (Soldier::mySoldier!=0)
					{
						// The new soldier name will be automatically sent out (if different) due to AddAutoSerializeTimer
						printf("Enter new soldier name: ");
						gets(Soldier::mySoldier->name);
					}
					else
					{
						printf("No soldier to rename! Create a soldier with 's'\n");
					}
				}
			}

		}

		RakSleep(100);
	}

	rakPeer->Shutdown(100,0);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}
