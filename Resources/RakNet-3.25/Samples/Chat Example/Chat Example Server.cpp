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
#include "RakSleep.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#ifdef _WIN32
#include "Kbhit.h"
#endif
#include <stdio.h>
#include <string.h>

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
#endif

// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(Packet *p);

#ifdef _CONSOLE_2
_CONSOLE_2_SetSystemProcessParams
#endif

int main(void)
{
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakPeerInterface *server=RakNetworkFactory::GetRakPeerInterface();
	RakNetStatistics *rss;
	//server->InitializeSecurity(0,0,0,0);
	int i = server->GetNumberOfAddresses();
	server->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));

	// Holds packets
	Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Record the first client that connects to us so we can pass it to the ping function
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;

	// Holds user data
	char portstring[30];

	printf("This is a sample implementation of a text based chat server.\n");
	printf("Connect to the project 'Chat Example Client'.\n");
	printf("Difficulty: Beginner\n\n");

	// A server
//	puts("Enter the server port to listen on");
//	gets(portstring);
//	if (portstring[0]==0)
		strcpy(portstring, "10000");
	
	puts("Starting server.");
	// Starting the server is very simple.  2 players allowed.
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	SocketDescriptor socketDescriptor(atoi(portstring),0);
	bool b = server->Startup(32, 30, &socketDescriptor, 1 );
	server->SetMaximumIncomingConnections(2);
	if (b)
		puts("Server started, waiting for connections.");
	else
	{ 
		puts("Server failed to start.  Terminating.");
		exit(1);
	}
	server->SetOccasionalPing(true);

	printf("My IP is %s\n", server->GetLocalIP(0));


	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping.\n'ban' to ban an IP from connecting.\n'kick to kick the first connected player.\nType to talk.");
	char message[2048];

	// Loop for input
	while (1)
	{

	// This sleep keeps RakNet responsive
	RakSleep(30);

#ifdef _WIN32
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
				rss=server->GetStatistics(server->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
				printf("Ping %i\n", server->GetAveragePing(server->GetSystemAddressFromIndex(0)));
		
				continue;
			}

			if (strcmp(message, "ping")==0)
			{
				server->Ping(clientID);

				continue;
			}

			if (strcmp(message, "kick")==0)
			{
				server->CloseConnection(clientID, true, 0);

				continue;
			}


			if (strcmp(message, "ban")==0)
			{
				printf("Enter IP to ban.  You can use * as a wildcard\n");
				gets(message);
				server->AddToBanList(message);
				printf("IP %s added to ban list.\n", message);

				continue;
			}


			// Message now holds what we want to broadcast
			char message2[420];
			// Append Server: to the message so clients know that it ORIGINATED from the server
			// All messages to all clients come from the server either directly or by being
			// relayed from other clients
			message2[0]=0;
			strcpy(message2, "Server: ");
			strcat(message2, message);
		
			// message2 is the data to send
			// strlen(message2)+1 is to send the null terminator
			// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
			// RELIABLE_ORDERED means make sure the message arrives in the right order
			// We arbitrarily pick 0 for the ordering stream
			// UNASSIGNED_SYSTEM_ADDRESS means don't exclude anyone from the broadcast
			// true means broadcast the message to everyone connected
			server->Send(message2, (const int) strlen(message2)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
#endif

		// Get a packet from either the server or the client

		p = server->Receive();
		
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

		
			case ID_NEW_INCOMING_CONNECTION:
				 // Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION from %s\n", p->systemAddress.ToString());
				clientID=p->systemAddress; // Record the player ID of the client
				break;

			case ID_MODIFIED_PACKET:
				// Cheater!
				printf("ID_MODIFIED_PACKET\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			default:
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				printf("%s\n", p->data);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				sprintf(message, "%s", p->data);
				server->Send(message, (const int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);
			
				break;
		}

		// We're done with the packet
		server->DeallocatePacket(p);
	}

	server->Shutdown(300);
	// We're done with the network
	RakNetworkFactory::DestroyRakPeerInterface(server);

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
