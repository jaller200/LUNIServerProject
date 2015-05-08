/// \file
/// \brief Tests the NAT-punchthrough plugin
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
#include "RakSleep.h"
#include <stdio.h>
#include <stdlib.h>
#include "RakNetworkFactory.h"
#include "NatPunchthrough.h"
#include <string.h>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "ConnectionGraph.h"
#include "PacketLogger.h"
#include "StringCompressor.h"
#include "GetTime.h"

#define NAT_PUNCHTHROUGH_FACILITATOR_PORT 60500
#define NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD ""
#define DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP "127.0.0.1"

// This sample starts one of three systems: A facilitator, a recipient, and a sender.  The sender wants to connect to the recipient but
// cannot because both the sender and recipient are behind NATs.  So they both connect to the facilitator first.
// The facilitator will synchronize a countdown timer so that the sender will try to connect to the recipient at the same time
// the recipient will send an offline message to the sender.  This will, in theory, punch through the NAT.
//
// IMPORTANT: The recipient and sender need to know each other's external IP addresses to be able to try to connect to each other.
// The facilitator should transmit this on connection, such as with a lobby server, which I do here using the ConnectionGraph plugin.
// That plugin will cause the lobby server to send ID_REMOTE_* system notifications to its connected systems.
void main(void)
{
	// Another program to test this is http://midcom-p2p.sourceforge.net/

	RakPeerInterface *rakPeer=RakNetworkFactory::GetRakPeerInterface();
	NatPunchthrough natPunchthrough;
	ConnectionGraph connectionGraph;
	NatPunchthroughLogger l;
	natPunchthrough.SetLogger(&l);
	char mode[64], facilitatorIP[64];
	rakPeer->AttachPlugin(&natPunchthrough);
	rakPeer->AttachPlugin(&connectionGraph);

	printf("Tests the NAT Punchthrough plugin\n");
	printf("Difficulty: Intermediate\n\n");
	printf("Act as (S)ender, (R)ecipient, or (F)acilitator?\n");
	gets(mode);
	if (mode[0]=='s' || mode[0]=='S')
	{
		SocketDescriptor socketDescriptor(50064,0);
		rakPeer->Startup(2,0,&socketDescriptor, 1);
		printf("Enter facilitator IP: ");
		gets(facilitatorIP);
		if (facilitatorIP[0]==0)
			strcpy(facilitatorIP, DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP);
		printf("Connecting to facilitator.\n");
		rakPeer->Connect(facilitatorIP, NAT_PUNCHTHROUGH_FACILITATOR_PORT, NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD, strlen(NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD));
	}
	else if (mode[0]=='r' || mode[0]=='R')
	{
		SocketDescriptor socketDescriptor(50065,0);
		rakPeer->Startup(2,0,&socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(1);
		printf("Enter facilitator IP: ");
		gets(facilitatorIP);
		if (facilitatorIP[0]==0)
			strcpy(facilitatorIP, DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP);
		printf("Connecting to facilitator.\n");
		rakPeer->Connect(facilitatorIP, NAT_PUNCHTHROUGH_FACILITATOR_PORT, NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD, strlen(NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD));
	}
	else
	{
		SocketDescriptor socketDescriptor(NAT_PUNCHTHROUGH_FACILITATOR_PORT,0);
		rakPeer->Startup(32,0,&socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(32);
		rakPeer->SetIncomingPassword(NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD, strlen(NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD));
		printf("Ready.\n");
	}

	printf("Press 'q' to quit.\n");
	Packet *p;
	while (1)
	{
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_REMOTE_NEW_INCOMING_CONNECTION && (mode[0]=='s' || mode[0]=='S'))
			{
				printf("Connecting to ID_REMOTE_NEW_INCOMING_CONNECTION of %s.\n", p->systemAddress.ToString(true));
				natPunchthrough.Connect(p->systemAddress, 0, 0, rakPeer->GetSystemAddressFromIndex(0));
			}
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
				printf("ID_DISCONNECTION_NOTIFICATION\n");
			else if (p->data[0]==ID_CONNECTION_LOST)
				printf("ID_CONNECTION_LOST\n");
			else if (p->data[0]==ID_NO_FREE_INCOMING_CONNECTIONS)
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			else if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
			{
				if (mode[0]=='r' || mode[0]=='R')
					printf("Test successful. Sender connected.\nPress space to send a test string.\n");
				else
					printf("ID_NEW_INCOMING_CONNECTION from %s\n", p->systemAddress.ToString());
			}
			else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				if (mode[0]=='s' || mode[0]=='S')
				{
					if (p->systemAddress.port==NAT_PUNCHTHROUGH_FACILITATOR_PORT)
						printf("ID_CONNECTION_REQUEST_ACCEPTED from %s.\nMy external IP is %s\n", p->systemAddress.ToString(), rakPeer->GetExternalID(p->systemAddress).ToString());
					else
						printf("Test successful.\nYou are connected to the receiver on address %s.\nPress space to send a test string.\n", p->systemAddress.ToString());
				}
				else
					printf("ID_CONNECTION_REQUEST_ACCEPTED from %s.\nMy external IP is %s\n", p->systemAddress.ToString(), rakPeer->GetExternalID(p->systemAddress). ToString());
			}
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
			{
				if (p->systemAddress.port==NAT_PUNCHTHROUGH_FACILITATOR_PORT)
					printf("ID_CONNECTION_ATTEMPT_FAILED (facilitator)\n");
				else
					printf("ID_CONNECTION_ATTEMPT_FAILED (recipient)\n");
			}
			else if (p->data[0]==ID_NAT_TARGET_NOT_CONNECTED)
			{
				SystemAddress systemAddress;
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				b.Read(systemAddress);
				printf("ID_NAT_TARGET_NOT_CONNECTED to %s\n", systemAddress.ToString());
			}
			else if (p->data[0]==ID_NAT_TARGET_CONNECTION_LOST)
			{
				SystemAddress systemAddress;
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				b.Read(systemAddress);
				printf("ID_NAT_TARGET_CONNECTION_LOST to %s\n", systemAddress.ToString());
			}
			else if (p->data[0]==ID_PONG)
			{
				RakNetTime time;
				memcpy((char*)&time, p->data+1, sizeof(RakNetTime));
				printf("Got pong from %s with time %i\n", p->systemAddress.ToString(), RakNet::GetTime() - time);
			}
			else if (p->data[0]==ID_NAT_IN_PROGRESS)
			{
				SystemAddress systemAddress;
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				b.Read(systemAddress);
				printf("ID_NAT_IN_PROGRESS to %s\n", systemAddress.ToString());
			}
			else if (p->data[0]==ID_USER_PACKET_ENUM)
			{
				char message[1024];
				RakNet::BitStream b(p->data, p->length, false);
				b.IgnoreBits(8); // Ignore the ID_...
				stringCompressor->DecodeString(message,1024,&b,0);
				printf("%s\n", message);
			}

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (kbhit())
		{
			char ch=getch();
			if (ch==' ')
			{
				RakNet::BitStream bs;
				bs.Write((MessageID)ID_USER_PACKET_ENUM);
				printf("Sent hello world to connected systems.\n");
				if (mode[0]=='s' || mode[0]=='S')
					stringCompressor->EncodeString("Hello world from sender.",1024,&bs,0);
				else if (mode[0]=='r' || mode[0]=='R')
					stringCompressor->EncodeString("Hello world from receiver.",1024,&bs,0);
				else
					stringCompressor->EncodeString("Hello world from facilitator.",1024,&bs,0);
				rakPeer->Send(&bs,HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);					 
			}
			if (ch=='q' || ch=='Q')
				break;
		}

		RakSleep(30);
	}

	rakPeer->Shutdown(100,0);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}
