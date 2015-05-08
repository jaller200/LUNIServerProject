/// \file
/// \brief Tests the connection graph.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "RakPeerInterface.h"
#include "ConnectionGraph.h"
#include "RakNetworkFactory.h"
#include "Kbhit.h"
#include <stdio.h> // Printf
#include <windows.h> // Sleep
#include "Rand.h"

#include "PacketLogger.h"

#define NUM_PEERS 8

void main(void)
{
	ConnectionGraph connectionGraph[NUM_PEERS];
	RakPeerInterface *rakPeer[NUM_PEERS];
	unsigned i, peerConnectIndex=1;

	printf("This sample demonstrates the connection graph plugin\n");
	printf("The plugin is used to maintain a list of all connections on the network\n");
	printf("It can be used directly and is also used by\n");
	printf("The fully connected mesh and the routing plugins\n");
	printf("Difficulty: Beginner\n\n");
	printf("Press space to see the connections, 'c' to connect a new peer\n'd' to disconnect a random peer 'r' to test a ring topology\n'q' to quit\n");

	for (i=0; i < NUM_PEERS; i++)
	{
		rakPeer[i]=RakNetworkFactory::GetRakPeerInterface();
		SocketDescriptor socketDescriptor(60000+i,0);
		rakPeer[i]->Startup(2, 30, &socketDescriptor, 1);
		rakPeer[i]->SetMaximumIncomingConnections(2);
		rakPeer[i]->AttachPlugin(&connectionGraph[i]);
	}

	Sleep(100);

	while (1)
	{
		for (i=0; i < NUM_PEERS; i++)
		{
			rakPeer[i]->DeallocatePacket(rakPeer[i]->Receive());
		}
		Sleep(0);

		if (kbhit())
		{
			char ch=getch();
			if (ch==' ')
			{
				for (i=0; i < NUM_PEERS; i++)
				{
					printf("System on port %i:\n", 60000+i);
					connectionGraph[i].GetGraph()->Print();
					printf("\n");
				}
			}
			else if (ch=='q')
				break;
			else if (ch=='c')
			{
				if (peerConnectIndex < NUM_PEERS)
				{
					printf("Connecting peer %i\n", peerConnectIndex);
					rakPeer[peerConnectIndex]->Connect("127.0.0.1", 60000+peerConnectIndex-1, 0, 0);
					peerConnectIndex++;
				}
				else
				{
					rakPeer[0]->Connect("127.0.0.1", 60000+NUM_PEERS-1, 0, 0);
					printf("All peers connected\n");
				}
			}
			else if (ch=='r')
			{
				printf("Connecting as ring\n");

				// Ring topology
				for (i=1; i < NUM_PEERS; i++)
					rakPeer[i]->Connect("127.0.0.1", 60000+i-1, 0, 0);
				if (NUM_PEERS>2)
					rakPeer[0]->Connect("127.0.0.1", 60000+NUM_PEERS-1,0,0);
			}
			else if (ch=='d')
			{
				int i = randomMT()%NUM_PEERS;
				printf("Disconnecting peer %i\n", i+60000);
				rakPeer[i]->Shutdown(200,0);
			}
		}
	}
}
