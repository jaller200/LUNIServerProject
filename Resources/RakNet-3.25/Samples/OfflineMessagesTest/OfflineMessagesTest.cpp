#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include <stdlib.h> // For atoi
#include <cstring> // For strlen
#include <stdio.h>
#include "MessageIdentifiers.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

int nextTest;

RakPeerInterface *peer1=RakNetworkFactory::GetRakPeerInterface();
RakPeerInterface *peer2=RakNetworkFactory::GetRakPeerInterface();

int main(void)
{
	char text[1024];
	bool sentPacket=false;
	nextTest=0;

	printf("This project tests the advertise system and offline ping messages.\n");
	printf("Difficulty: Beginner\n\n");

	peer1->SetMaximumIncomingConnections(1);
	SocketDescriptor socketDescriptor(60001, 0);
	peer1->Startup(1, 0, &socketDescriptor, 1);
	socketDescriptor.port=60002;
	peer2->Startup(1, 0, &socketDescriptor, 1);
	peer1->SetOfflinePingResponse("Offline Ping Data", (int)strlen("Offline Ping Data")+1);

	printf("Systems started.  Waiting for advertise system packet\n");

	// Wait for connection to complete
#ifdef _WIN32
	Sleep(300);
#else
	usleep(300 * 1000);
#endif

	peer1->AdvertiseSystem("127.0.0.1", 60002,"hello world", (int)strlen("hello world")+1);

	while (nextTest!=2)
	{
		peer1->DeallocatePacket(peer1->Receive());
		Packet *packet = peer2->Receive();
		if (packet)
		{
			if (packet->data[0]==ID_ADVERTISE_SYSTEM)
			{
				if (packet->length>1)
					printf("Got Advertise system with data: %s\n", packet->data+1);
				else
					printf("Got Advertise system with no data\n");

				peer2->Ping("127.0.0.1", 60001, false);
				nextTest++;
			}
			else if (packet->data[0]==ID_PONG)
			{
				// Peer or client. Response from a ping for an unconnected system.
				RakNetTime time, dataLength;
				memcpy( ( char* ) & time, packet->data + sizeof( unsigned char ), sizeof( RakNetTime ) );
				dataLength = packet->length - sizeof( unsigned char ) - sizeof( RakNetTime );
				printf("%s", peer2->GetInternalID().ToString());
				if (peer2->IsLocalIP(packet->systemAddress.ToString(false)))
					printf("ID_PONG from our own");
				else
					printf( "ID_PONG from");
				printf(" %s on %p.\nPing is %i\nData is %i bytes long.\n", packet->systemAddress.ToString(), peer2, time, dataLength );

				if ( dataLength > 0 )
					printf( "Data is %s\n", packet->data + sizeof( unsigned char ) + sizeof( RakNetTime ) );

				nextTest++;
				// ProcessUnhandledPacket(packet, ID_PONG,interfaceType);
			}
			peer2->DeallocatePacket(packet);
		}

#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif
	}

	printf("Test complete. Press enter to quit\n");
	gets(text);

	RakNetworkFactory::DestroyRakPeerInterface(peer1);
	RakNetworkFactory::DestroyRakPeerInterface(peer2);

	return 0;
}
