#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "Rand.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "FullyConnectedMesh.h"
#include "ConnectionGraph.h"
#include <assert.h>
#include "Kbhit.h"
#include "RakSleep.h"

static const int NUM_PEERS=8;
RakPeerInterface *rakPeer[NUM_PEERS];
FullyConnectedMesh fullyConnectedMeshPlugin[NUM_PEERS];
ConnectionGraph connectionGraphPlugin[NUM_PEERS];
void PrintConnections(void);

int main(void)
{
	int i;

	// TODO - this is very slow using the connection graph.  A lot of crap is being sent around.  Fix that.
	
	for (i=0; i < NUM_PEERS; i++)
		rakPeer[i]=RakNetworkFactory::GetRakPeerInterface();

	printf("This project tests and demonstrates the fully connected mesh plugin.\n");
	printf("No data is actually sent so it's mostly a sample of how to use a plugin.\n");
	printf("Difficulty: Beginner\n\n");

	int peerIndex;

	// Initialize the message handlers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		fullyConnectedMeshPlugin[peerIndex].Startup(0,0);
		rakPeer[peerIndex]->AttachPlugin(&fullyConnectedMeshPlugin[peerIndex]);
		// The fully connected mesh relies on the connection graph plugin also being attached
		rakPeer[peerIndex]->AttachPlugin(&connectionGraphPlugin[peerIndex]);
		rakPeer[peerIndex]->SetMaximumIncomingConnections(NUM_PEERS);
	}

	// Initialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, 0, &socketDescriptor, 1);
	}

	// Give the threads time to properly start
	RakSleep(200);

	printf("Peers initialized. ");
	printf("Connecting each peer to the prior peer\n");
	
	// Connect each peer to the prior peer
	for (peerIndex=1; peerIndex < NUM_PEERS; peerIndex++)
	{
        rakPeer[peerIndex]->Connect("127.0.0.1", 60000+peerIndex-1, 0, 0);
	}

	PrintConnections();

	// Close all connections
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->Shutdown(100);
	}

	// Reinitialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, 0,&socketDescriptor, 1 );
	}

	printf("Connecting each peer to a central peer.\n");
	// Connect each peer to a central peer
	for (peerIndex=1; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->Connect("127.0.0.1", 60000, 0, 0);
	}

	PrintConnections();

	// Close all connections
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->Shutdown(100);
	}

	// Reinitialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, 0, &socketDescriptor, 1);
	}

	printf("Cross connecting each pair of peers, then first and last peer.\n");
	// Connect each peer to a central peer
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->Connect("127.0.0.1", 60000+peerIndex+(((peerIndex%2)==0) ? 1 : -1), 0, 0);
	}

	printf("Pairs Connected\n");
	PrintConnections();
	rakPeer[0]->Connect("127.0.0.1", 60000+NUM_PEERS-1, 0, 0);
	printf("First and last connected\n");
	PrintConnections();

	// Close all connections
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->Shutdown(100);
	}

	// Reinitialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, 0, &socketDescriptor, 1);
	}


	unsigned int seed = (unsigned int) RakNet::GetTime(); 
	seedMT(seed);
	printf("Connecting each peer to a random peer with seed %u.\n", seed);
	int connectTo=0;
	// Connect each peer to a central peer
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		do 
		{
			connectTo=randomMT() % NUM_PEERS;
		} while (connectTo==peerIndex);
		
		rakPeer[peerIndex]->Connect("127.0.0.1", 60000+connectTo, 0, 0);
	}

	PrintConnections();

	for (i=0; i < NUM_PEERS; i++)
		RakNetworkFactory::DestroyRakPeerInterface(rakPeer[i]);

	return 1;
}

void PrintConnections()
{
	int i,j;
	char ch=0;
	SystemAddress systemAddress;
	Packet *packet;
	printf("Connecting.  Press space to see status or c to continue.\n");

	while (ch!='c' && ch!='C')
	{
		if (kbhit())
			ch=getch();

		if (ch==' ')
		{
			printf("--------------------------------\n");
			for (i=0; i < NUM_PEERS; i++)
			{
				if (NUM_PEERS<=10)
				{
					/*
					printf("%i (Mesh): ", 60000+i);
					for (j=0; j < (int)fullyConnectedMeshPlugin[i].GetMeshPeerListSize(); j++)
					{
						systemAddress=fullyConnectedMeshPlugin[i].GetPeerIDAtIndex(j);
						if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
							printf("%i ", systemAddress.port);
					}

					printf("\n");
					*/

					printf("%i (Conn): ", 60000+i);
					for (j=0; j < NUM_PEERS; j++)
					{
						systemAddress=rakPeer[i]->GetSystemAddressFromIndex(j);
						if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
							printf("%i ", systemAddress.port);
					}

					printf("\n");
				}
				else
				{
					int connCount;
					//int meshCount;
					for (connCount=0, j=0; j < NUM_PEERS; j++)
					{
						systemAddress=rakPeer[i]->GetSystemAddressFromIndex(j);
						if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
							connCount++;
					}
					/*
					for (meshCount=0, j=0; j < (int)fullyConnectedMeshPlugin[i].GetMeshPeerListSize(); j++)
					{
						systemAddress=fullyConnectedMeshPlugin[i].GetPeerIDAtIndex(j);
						if (systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
							meshCount++;
					}
					*/
					
					//printf("%i (Mesh): %i peers should be connected\n", 60000+i, meshCount);
					printf("%i (Conn): %i peers are connected\n", 60000+i, connCount);
				}
			}
			printf("\n");
			ch=0;

			printf("--------------------------------\n");
		}

		for (i=0; i < NUM_PEERS; i++)
		{
			packet=rakPeer[i]->Receive();
			if (packet)
			{
				if (packet->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
					printf("%i: ID_CONNECTION_ATTEMPT_FAILED to %i\n", 60000+i, packet->systemAddress.port);
				rakPeer[i]->DeallocatePacket(packet);
			}
		}

		// Keep raknet threads responsive
		RakSleep(30);
	}	
}