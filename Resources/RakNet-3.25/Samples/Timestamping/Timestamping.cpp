#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "GetTime.h"
using namespace RakNet;

#include <cstdio>
#include <memory.h>
#include <cstring>

#ifdef WIN32
#include "Kbhit.h"
#else
#include "Kbhit.h"
#endif


int main(void)
{
	char serverIP[30];

	RakPeerInterface *rakClient=RakNetworkFactory::GetRakPeerInterface();
	RakPeerInterface *rakServer=RakNetworkFactory::GetRakPeerInterface();

	char ch;
	bool isServer;
	
	printf("Demonstrates RakNet's timestamping system, used to get a common\n");
	printf("network time without relying on NTP.\n");
	printf("Difficulty: Beginner\n\n");

	printf("Hit 'c' to run as a client.  Hit 's' to run as a server. Hit 'q' to quit\n");
	char buff[256];

	while (1)
	{
		gets(buff);
		ch = buff[0];

		if (ch=='c')
		{
			// Run as a client.  If you don't have another machine, just run 2 instances of this program and use "127.0.0.1"
			puts ("Enter server IP\n");
			gets(serverIP);
			if (serverIP[0]==0)
				strcpy(serverIP, "127.0.0.1");

			SocketDescriptor socketDescriptor(0,0);
			rakClient->Startup(1, 30, &socketDescriptor, 1);
			rakClient->Connect(serverIP, 2100, 0, 0);
			printf("Connecting client\n");
			isServer=false;
			break;
		}
		else if (ch=='s')
		{
			// Run as a server.
			SocketDescriptor socketDescriptor(2100,0);
			rakServer->Startup(32,30,&socketDescriptor, 1);
			rakServer->SetMaximumIncomingConnections(32);
			printf("Server started\n");
			isServer=true;
			break;
		}
		else if (ch=='q')
			return 0;
		else
		{
			printf("Bad input.  Enter 'c' 's' or 'q'.\n");
		}
	}

	printf("Entering main loop.  Press 'q' to quit\n'c' to send from the client.\n's' to send from the server.\n");
	Packet *packet;
	RakNetTime time;
	ch=0;
	bool packetFromServer;
	while (1)
	{
		if (kbhit())
		{
#ifndef _WIN32
			gets(buff);
			ch=buff[0];
#else
			ch=getch();
#endif
		}

		if (ch=='q')
			break;

		if (ch=='c' && rakClient->GetSystemAddressFromIndex(0)!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			BitStream bitStream;

			// When writing a timestamp, the first byte is ID_TIMESTAMP
			// The next 4 bytes is the timestamp itself.

			bitStream.Write((unsigned char)ID_TIMESTAMP);
			
			time=RakNet::GetTime();
			bitStream.Write(time);
			rakClient->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			printf("Sending message from client at time %i\n", time);
		}
		else if (ch=='s' && rakServer->IsActive())
		{
			BitStream bitStream;
			bitStream.Write((unsigned char)ID_TIMESTAMP);

			time=RakNet::GetTime();
			bitStream.Write(time);
			rakServer->Send(&bitStream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			printf("Sending packet from server at time %i\n", time);
		}

        if (isServer==false)
		{
			packetFromServer=false;
			packet=rakClient->Receive();
		}
		else
		{
			packetFromServer=true;
			packet=rakServer->Receive();
		}
		
		if (packet && packet->data[0]==ID_TIMESTAMP)
		{
			// Write the bytes after the first to a variable.  That is the time the packet was sent.
			RakNet::BitStream timeBS(packet->data+1, sizeof(RakNetTime), false);
			timeBS.Read(time);
			printf("Time difference is %i\n", RakNet::GetTime() - time);
		}

		if (packet)
		{
			if (packetFromServer)
				rakServer->DeallocatePacket(packet);
			else
				rakClient->DeallocatePacket(packet);
		}

		ch=0;
	}

	// Shutdown stuff.  It's ok to call disconnect on the server if we are a client and vice-versa
	rakServer->Shutdown(0);
	rakClient->Shutdown(0);

	RakNetworkFactory::DestroyRakPeerInterface(rakClient);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);

	return 0;
}

