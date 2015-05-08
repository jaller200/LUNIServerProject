// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename ChatExample.cpp
// Created by Rakkar Software (rakkar@jenkinssoftware.com) January 24, 2003
// Very basic chat engine example
// ----------------------------------------------------------------------

#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#ifdef _WIN32
#include "Kbhit.h"
#include <windows.h> // Sleep
#else
#include "Kbhit.h"
#include <unistd.h> // usleep
#endif
// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(Packet *p);

int main(void)
{
	RakNetStatistics *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakPeerInterface *client=RakNetworkFactory::GetRakPeerInterface();
//	client->InitializeSecurity(0,0,0,0);
	
	// Holds packets
	Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Just so we can remember where the packet came from
	bool isServer;

	// Record the first client that connects to us so we can pass it to the ping function
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;

	// Crude interface

	// Holds user data
	char ip[30], serverPort[30], clientPort[30];

	// A client
	isServer=false;

	printf("This is a sample implementation of a text based chat client.\n");
	printf("Connect to the project 'Chat Example Server'.\n");
	printf("Difficulty: Beginner\n\n");

	// Get our input
	puts("Enter the client port to listen on");
	gets(clientPort);
	if (clientPort[0]==0)
		strcpy(clientPort, "0");

	puts("Enter IP to connect to");
	gets(ip);
	client->AllowConnectionResponseIPMigration(false);
	if (ip[0]==0)
		strcpy(ip, "127.0.01");
	
		
	puts("Enter the port to connect to");
	gets(serverPort);
	if (serverPort[0]==0)
		strcpy(serverPort, "10000");

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	SocketDescriptor socketDescriptor(atoi(clientPort),0);
	client->Startup(1,30,&socketDescriptor, 1);
	client->SetOccasionalPing(true);
	bool b = client->Connect(ip, atoi(serverPort), "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));	

	if (b)
		puts("Attempting connection");
	else
	{
		puts("Bad connection attempt.  Terminating.");
		exit(1);
	}

	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping. Type to talk.");
	char message[2048];

	// Loop for input
	while (1)
	{
		// This sleep keeps RakNet responsive
#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif


		if (kbhit())
		{
			// Notice what is not here: something to keep our network running.  It's
			// fine to block on gets or anything we want
			// Because the network engine was painstakingly written using threads.
			gets(message);

			if (strcmp(message, "quit")==0)
			{
				puts("Quitting.");
				break;
			}

			if (strcmp(message, "stat")==0)
			{
				
				rss=client->GetStatistics(client->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
				printf("Ping=%i\n", client->GetAveragePing(client->GetSystemAddressFromIndex(0)));
			

				continue;
			}

			if (strcmp(message, "ping")==0)
			{
				if (client->GetSystemAddressFromIndex(0)!=UNASSIGNED_SYSTEM_ADDRESS)
					client->Ping(client->GetSystemAddressFromIndex(0));

				continue;
			}

			// message is the data to send
			// strlen(message)+1 is to send the null terminator
			// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
			// RELIABLE_ORDERED means make sure the message arrives in the right order
			client->Send(message, (int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}

		// Get a packet from either the server or the client

		p = client->Receive();

	
		if (p==0)
			continue; // Didn't get any packets

		// We got a packet, get the identifier with our handy function
		packetIdentifier = GetPacketIdentifier(p);

		// Check if this is a network message packet
		switch (packetIdentifier)
		{
			case ID_DISCONNECTION_NOTIFICATION:
				  // Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				 // Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_MODIFIED_PACKET:
				// Cheater!
				printf("ID_MODIFIED_PACKET\n");
				break;

			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			default:
					// It's a client, so just show the message
					printf("%s\n", p->data);
				break;
		}


		// We're done with the packet
		client->DeallocatePacket(p);
	}

	// Be nice and let the server know we quit.
	client->Shutdown(300);

	// We're done with the network
	RakNetworkFactory::DestroyRakPeerInterface(client);

	return 0;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char) p->data[0];
}
