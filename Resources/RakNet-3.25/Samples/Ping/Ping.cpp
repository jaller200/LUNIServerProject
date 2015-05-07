// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename Ping.cpp
// Created by Rakkar Software (rakkar@jenkinssoftware.com) January 24, 2003
// Very basic chat engine example
// ----------------------------------------------------------------------
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "GetTime.h"
#include "BitStream.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#ifdef WIN32
#include "Kbhit.h"
#else
#include "Kbhit.h"
#endif

int main(void)
{
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakPeerInterface *client=RakNetworkFactory::GetRakPeerInterface();
	RakPeerInterface *server=RakNetworkFactory::GetRakPeerInterface();

	int i = server->GetNumberOfAddresses();

	// Holds packets
	Packet* p;

	// Record the first client that connects to us so we can pass it to the ping function
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;
	bool packetFromServer;
	char portstring[30];

	printf("This is a sample of how to send offline pings and get offline ping\n");
	printf("responses.\n");
	printf("Difficulty: Beginner\n\n");

	// A server
	puts("Enter the server port to listen on");
	gets(portstring);
	if (portstring[0]==0)
		strcpy(portstring,"60000");

	// Enumeration data
	puts("Enter offline ping response data (for return by a LAN discovery for example)");
	puts("Hit enter for none.");
	char enumData[512];
	gets(enumData);
	if (enumData[0])
		server->SetOfflinePingResponse(enumData, (const unsigned int) strlen(enumData)+1);

	puts("Starting server.");

	// The server has to be started to respond to pings.
	SocketDescriptor socketDescriptor(atoi(portstring),0);
	bool b = server->Startup(2, 30, &socketDescriptor, 1);
	server->SetMaximumIncomingConnections(2);
	if (b)
		puts("Server started, waiting for connections.");
	else
	{ 
		puts("Server failed to start.  Terminating.");
		exit(1);
	}

	socketDescriptor.port=0;
	client->Startup(1,30,&socketDescriptor, 1);

	puts("'q' to quit, any other key to send a ping from the client.");
	char buff[256];

	// Loop for input
	while (1)
	{
		if (kbhit())
		{
			// Holds user data
			char ip[30], serverPort[30], clientPort[30];

			if (gets(buff)&&(buff[0]=='q'))
				break;
			else
			{

				// Get our input
				puts("Enter the client port to listen on, or 0");
				gets(clientPort);
				if (clientPort[0]==0)
					strcpy(clientPort, "0");
				puts("Enter IP to ping");
				gets(ip);
				if (ip[0]==0)
					strcpy(ip, "127.0.0.1");
				puts("Enter the port to ping");
				gets(serverPort);
				if (serverPort[0]==0)
					strcpy(serverPort, "60000");

				client->Ping(ip, atoi(serverPort), false);

				puts("Pinging");
			}
		}

		// Get a packet from either the server or the client
		p = server->Receive();

		if (p==0)
		{
			packetFromServer=false;
			p = client->Receive();
		}
		else
			packetFromServer=true;

		if (p==0)
			continue;


		// Check if this is a network message packet
		switch (p->data[0])
		{
			case ID_PONG:
				RakNetTime time, dataLength;
				RakNet::BitStream pong( p->data+1, sizeof(RakNetTime), false);
				pong.Read(time);
				dataLength = p->length - sizeof(unsigned char) - sizeof(RakNetTime);
				printf("ID_PONG from SystemAddress:%u:%u.\n", p->systemAddress.binaryAddress, p->systemAddress.port);
				printf("Time is %i\n",time);
				printf("Ping is %i\n", (unsigned int)(RakNet::GetTime()-time));
				printf("Data is %i bytes long.\n", dataLength);
				if (dataLength > 0)
					printf("Data is %s\n", p->data+sizeof(unsigned char)+sizeof(RakNetTime));
				break;
			
			// In this sample since the client is not running a game we can save CPU cycles by
			// Stopping the network threads after receiving the pong.
			client->Shutdown(100);
		}

		// We're done with the packet
		if (packetFromServer)
			server->DeallocatePacket(p);
		else
			client->DeallocatePacket(p);
	}

	// We're done with the network
	RakNetworkFactory::DestroyRakPeerInterface(server);
	RakNetworkFactory::DestroyRakPeerInterface(client);

	return 0;
}
