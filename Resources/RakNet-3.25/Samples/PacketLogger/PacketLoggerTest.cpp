#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "Rand.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "PacketLogger.h"
#include <assert.h>
#include "Kbhit.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

static const int NUM_PEERS=2;
RakPeerInterface *rakPeer[NUM_PEERS];
PacketLogger messageHandler[NUM_PEERS];
void PrintConnections(void);

void TestRPC(RPCParameters *rpcParameters)
{
}


int main(void)
{
	int i;
	Packet *p;
	
	for (i=0; i < NUM_PEERS; i++)
		rakPeer[i]=RakNetworkFactory::GetRakPeerInterface();

	printf("Packet Logger Test.\n");
	printf("Displays all packets being sent or received.\n");
	printf("Overwrite PacketLogger::Log to render output into your own program.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Comma delimited log format:\n");
	printf("1. Send or receive,\n");
	printf("2. Raw (direct socket send) OR Ack (Acknowledgement) OR\nTms (Timestamped packet) OR Rpc (Remote Procedure Call) OR\nRpT (RPC with timestamp) OR Nrm (Normal send through reliability layer),\n");
	printf("3. Message number,\n");
	printf("4. Packet Number (Independent for send & receive).\n(Each Packet may contain multiple messages),\n");
	printf("5. Packet ID (or a string for RPC calls),\n");
	printf("6. Bits used by the message (does not include 2-4 byte RakNet header),\n");
	printf("7. Time the message is sent,\n");
	printf("8. Local System (binary IP followed by port),\n");
	printf("9. Remote System (binary IP followed by port)\n\n");

	int peerIndex;

	// Initialize the message handlers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		rakPeer[peerIndex]->AttachPlugin(&messageHandler[peerIndex]);
		rakPeer[peerIndex]->SetMaximumIncomingConnections(NUM_PEERS);
	}


	REGISTER_STATIC_RPC(rakPeer[0], TestRPC);


	// Initialize the peers
	for (peerIndex=0; peerIndex < NUM_PEERS; peerIndex++)
	{
		SocketDescriptor socketDescriptor(60000+peerIndex,0);
		rakPeer[peerIndex]->Startup(NUM_PEERS, 0, &socketDescriptor, 1);
	}

	printf("Connecting two systems...\n\n");

	messageHandler[0].LogHeader();
	
	// Connect each peer to the prior peer
	for (peerIndex=1; peerIndex < NUM_PEERS; peerIndex++)
	{
        rakPeer[peerIndex]->Connect("127.0.0.1", 60000+peerIndex-1, 0, 0);
	}

#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif

	rakPeer[1]->RPC("TestRPC",0, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID,0);

#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif
	// RPC packets are handled in the user thread, from the Receive call.
	// Doing this just so it calls the RPC
	while (p=rakPeer[0]->Receive())
		rakPeer[0]->DeallocatePacket(p);


#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif

	rakPeer[1]->RPC("TestRPC",0, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID,0);

#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif

	// RPC packets are handled in the user thread, from the Receive call.
	// Doing this just so it calls the RPC
	while (p=rakPeer[0]->Receive())
		rakPeer[0]->DeallocatePacket(p);

#ifdef WIN32
	Sleep(5000);
#else
	usleep(5000*1000);
#endif

	for (i=0; i < NUM_PEERS; i++)
		RakNetworkFactory::DestroyRakPeerInterface(rakPeer[i]);

	printf("Press enter to continue.\n");
	char temp[256];
	gets(temp);

	return 1;
}
