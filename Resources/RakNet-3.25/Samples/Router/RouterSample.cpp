#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include <stdlib.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "StringCompressor.h"
#include "ConnectionGraph.h"
#include "Router.h"

#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h" // Developers of a certain platform will know what to do here.
#elif defined(_WIN32)
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

#define CHAIN_LENGTH 5
#define CHAIN_NODES_LENGTH 2

void TestRPC(RPCParameters *rpcParameters)
{
	printf("In TestRPC to test routed RPC calls. Sender was %s\n", rpcParameters->sender.ToString());
}

void GetInputIndices(int *chainIndex, int *chainNodeIndex);

int main(void)
{
	RakPeerInterface *chain[CHAIN_LENGTH];
	RakPeerInterface *chainNodes[CHAIN_NODES_LENGTH * CHAIN_LENGTH];
	ConnectionGraph connectionGraph[CHAIN_NODES_LENGTH * CHAIN_LENGTH+CHAIN_LENGTH];
	Router router[CHAIN_NODES_LENGTH * CHAIN_LENGTH+CHAIN_LENGTH];
	unsigned i,j;

	printf("Demonstration of the router plugin.\n");
	printf("The router plugin exposes a Send function allowing you to send to\n");
	printf("systems you are not directly connected to.  It also causes a system to route\n");
	printf("messages that weren't directed to it.  It also uses multicasting to send the\n");
	printf("minimum amount of data for when there are multiple recipients for the\n");
	printf("same message.\n");
	printf("Difficulty: Advanced\n\n");

	// This topology is a 'chain' of instances of RakPeer - just a straight line
	for (i=0; i < CHAIN_LENGTH; i++)
	{
		chain[i]=RakNetworkFactory::GetRakPeerInterface();
		SocketDescriptor socketDescriptor(50000+i,0);
		chain[i]->Startup(CHAIN_NODES_LENGTH+2, 30, &socketDescriptor, 1);
		chain[i]->SetMaximumIncomingConnections(CHAIN_NODES_LENGTH+1);
		chain[i]->AttachPlugin(&connectionGraph[i]);
		chain[i]->AttachPlugin(&router[i]);
		REGISTER_STATIC_RPC(chain[i], TestRPC);
		router[i].SetRestrictRoutingByType(true);
		router[i].AddAllowedType(ID_USER_PACKET_ENUM); // Allow this type of message
		router[i].AddAllowedType(ID_RPC); // Allow this type of message
		router[i].SetConnectionGraph(connectionGraph[i].GetGraph());
		
	}

	// Each element in the chain is essentially a server, with a number of 'chain nodes' attached to it.
	for (i=0; i < CHAIN_NODES_LENGTH * CHAIN_LENGTH; i++)
	{
		chainNodes[i]=RakNetworkFactory::GetRakPeerInterface();
		SocketDescriptor socketDescriptor(60000+i,0);
		chainNodes[i]->Startup(1, 30,&socketDescriptor, 1);
		chainNodes[i]->SetMaximumIncomingConnections(0);
		chainNodes[i]->AttachPlugin(&connectionGraph[CHAIN_LENGTH+i]);
		chainNodes[i]->AttachPlugin(&router[CHAIN_LENGTH+i]);
		REGISTER_STATIC_RPC(chainNodes[i], TestRPC);
		router[i+CHAIN_LENGTH].SetConnectionGraph(connectionGraph[CHAIN_LENGTH+i].GetGraph());
	}

	// Connect the chain
	for (i=1; i < CHAIN_LENGTH; i++)
		chain[i]->Connect("127.0.0.1", 50000+i-1, 0, 0);

	// Connect the chain nodes
	for (i=0; i < CHAIN_LENGTH; i++)
		for (j=0; j < CHAIN_NODES_LENGTH; j++)
			chainNodes[i*CHAIN_NODES_LENGTH+j]->Connect("127.0.0.1", 50000+i, 0, 0);

	Sleep(500);

	printf("Commands:\n(Q)uit\n(S)end message\n");

	Packet *p;
	while (1)
	{
		for (i=0; i < CHAIN_LENGTH; i++)
		{
			p=chain[i]->Receive();
			if (p)
			{
				if (p->data[0]==ID_USER_PACKET_ENUM)
					printf("Chain %i (port %i) got %s from port %i\n", i, 50000+i, p->data+1,p->systemAddress.port);
				chain[i]->DeallocatePacket(p);
			}
		}

		for (i=0; i < CHAIN_NODES_LENGTH * CHAIN_LENGTH; i++)
		{
			p=chainNodes[i]->Receive();
			if (p)
			{
				if (p->data[0]==ID_USER_PACKET_ENUM)
					printf("Chain Node %i (port %i) got %s from port %i\n", i, 60000+i, p->data+1,p->systemAddress.port);
				chainNodes[i]->DeallocatePacket(p);
			}			
		}

		if (kbhit())
		{
			char ch=getch();
			if (ch=='q')
				break;
			if (ch=='s')
			{
				SystemAddressList systemList;
				int senderChainIndex, senderChainNodeIndex;
				int recipientChainIndex, recipientChainNodeIndex;
				SystemAddress recipient;
				char str[256];
				recipient.SetBinaryAddress("127.0.0.1");
				recipient.port=60000;
				printf("Sender:\n");
				GetInputIndices(&senderChainIndex, &senderChainNodeIndex);
			//	senderChainIndex=0; senderChainNodeIndex=0;
				if (senderChainNodeIndex!=-1)
					printf("Sending from port %i\n", 60000+senderChainIndex*CHAIN_NODES_LENGTH+senderChainNodeIndex);
				else
					printf("Sending from port %i\n", 50000+senderChainIndex);
				
				do 
				{
					printf("Recipient %i:\n", systemList.GetList()->Size());
					GetInputIndices(&recipientChainIndex, &recipientChainNodeIndex);
				//	recipientChainIndex=CHAIN_LENGTH-1; recipientChainNodeIndex=CHAIN_NODES_LENGTH-1;
					if (recipientChainNodeIndex!=-1)
						recipient.port=60000+recipientChainIndex*CHAIN_NODES_LENGTH+recipientChainNodeIndex;
					else
						recipient.port=50000+recipientChainIndex;
					systemList.AddSystem(recipient);
					if (recipientChainNodeIndex!=-1)
						printf("Added recipient on port %i\n", 60000+recipientChainIndex*CHAIN_NODES_LENGTH+recipientChainNodeIndex);
					else
						printf("Added recipient on port %i\n", 50000+recipientChainIndex);
					printf("Hit 'y' to enter another recipient or anything else to stop.\n");
				} while(getch()=='y');

				printf("Enter string to send: ");
				gets(str);
				if (str[0]==0)
					strcpy(str, "Hello World");
				RakNet::BitStream out;
				out.Write((unsigned char)ID_USER_PACKET_ENUM);
				out.Write(str,(const int)strlen(str)+1); // Write clear text rather than with string compressor so I can debug more easily

				bool outcome;
                if (senderChainNodeIndex!=-1)
				{
				//	connectionGraph[CHAIN_LENGTH + CHAIN_NODES_LENGTH*senderChainIndex + senderChainNodeIndex].GetGraph()->Print();
					outcome=router[CHAIN_LENGTH + CHAIN_NODES_LENGTH*senderChainIndex + senderChainNodeIndex].Send((char*)out.GetData(), out.GetNumberOfBitsUsed(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, &systemList);

					chainNodes[CHAIN_NODES_LENGTH*senderChainIndex + senderChainNodeIndex]->RPC("TestRPC", 0, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemList.GetList()->operator[](0), false, 0, UNASSIGNED_NETWORK_ID, 0);
				}
				else
				{
				//	connectionGraph[senderChainIndex].GetGraph()->Print();
					outcome=router[senderChainIndex].Send((char*)out.GetData(), out.GetNumberOfBitsUsed(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, &systemList);

					// TODO
					// Test autorouted RPC
					chain[senderChainIndex]->RPC("TestRPC", 0, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemList.GetList()->operator[](0), false, 0, UNASSIGNED_NETWORK_ID, 0);
				}
				if (outcome)
					printf("Send successful\n");
				else
					printf("Send failed\n");

				
			}
		}
		
#ifdef _WIN32
		Sleep(0);
#else
		usleep(0 * 1000);
#endif
	}

	for (i=0; i < CHAIN_LENGTH; i++)
		RakNetworkFactory::DestroyRakPeerInterface(chain[i]);

	for (i=0; i < CHAIN_NODES_LENGTH * CHAIN_LENGTH; i++)
		RakNetworkFactory::DestroyRakPeerInterface(chainNodes[i]);

	return 1;
}

void GetInputIndices(int *chainIndex, int *chainNodeIndex)
{
	char str[256];
	printf("Select chain index (0 to %i): ", CHAIN_LENGTH-1);
	gets(str);
	if (str[0]==0)
		strcpy(str, "0");
	*chainIndex=atoi(str);
	printf("Enter chain node index or -1 for none (-1 to %i): ", CHAIN_NODES_LENGTH-1);
	gets(str);
	if (str[0]==0)
		strcpy(str, "-1");
	*chainNodeIndex=atoi(str);
}
