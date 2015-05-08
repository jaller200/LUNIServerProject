/// \file
/// \brief Tests connecting two peers at the same time with the internet simulator running.
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
#include "RakNetworkFactory.h"
#include "PacketLogger.h"
#include "GetTime.h"
#include "RakNetStatistics.h"
#include "Rand.h"
#include "Kbhit.h"
#include <stdio.h> // Printf
#include <windows.h> // Sleep

void DoSpeedTest(PacketReliability packetReliability, unsigned sendSize);

RakNetTime currentTime, lastTime;
unsigned messagesPerSecond,i,lastPrint;
RakPeerInterface *peer1, *peer2;

void main(void)
{
	printf("Current implementations run out of memory before speed.\n");
	return;

	peer1=RakNetworkFactory::GetRakPeerInterface();
	peer2=RakNetworkFactory::GetRakPeerInterface();
	peer1->SetMaximumIncomingConnections(1);
	peer2->SetMaximumIncomingConnections(1);

	//printf("This project is an automatic test of various sends of various sizes.\n");
	//printf("I use it for performance profiling.\n");
	//printf("Difficulty: Beginner\n\n");


	printf("Testing UNRELIABLE with 100 byte messages.\n");
	DoSpeedTest(UNRELIABLE, 100);

	printf("Testing UNRELIABLE with 400 byte messages.\n");
	DoSpeedTest(UNRELIABLE, 400);

	printf("Testing RELIABLE with 100 byte messages.\n");
	DoSpeedTest(RELIABLE, 100);

	printf("Testing RELIABLE with 400 byte messages.\n");
	DoSpeedTest(RELIABLE, 400);

	printf("Testing RELIABLE_ORDERED with 100 byte messages.\n");
	DoSpeedTest(RELIABLE_ORDERED, 100);

	printf("Testing RELIABLE_ORDERED with 400 byte messages.\n");
	DoSpeedTest(RELIABLE_ORDERED, 400);
	
//	printf("Press any key to quit\n");
//	getch();
	RakNetworkFactory::DestroyRakPeerInterface(peer1);
	RakNetworkFactory::DestroyRakPeerInterface(peer2);
}

void DoSpeedTest(PacketReliability packetReliability, unsigned sendSize)
{
	RakNetStatistics *rss1, *rss2;
	unsigned lastMessagesPerSecond1, lastMessagesPerSecond2;
	unsigned riseCount1, riseCount2;
	Packet *p;
	char dummyData[400];
	memset(dummyData,0,400);

	SocketDescriptor socketDescriptor(1234,0);
	peer1->Startup(1,0,&socketDescriptor, 1);
	socketDescriptor.port=1235;
	peer2->Startup(1,0,&socketDescriptor, 1);
	peer1->Connect("127.0.0.1", 1235, 0, 0);
	peer2->Connect("127.0.0.1", 1234, 0, 0);
	Sleep(500);
	
	messagesPerSecond=1000;
	lastTime=currentTime=RakNet::GetTime();
	printf("Messages per second: ");

	riseCount1=riseCount2=0;
	lastMessagesPerSecond1=lastMessagesPerSecond2=0;
	while (riseCount1 < 100 && riseCount2 < 100)
	{
		rss1=peer1->GetStatistics(peer1->GetSystemAddressFromIndex(0));
		rss2=peer2->GetStatistics(peer2->GetSystemAddressFromIndex(0));
		if (rss1==0)
		{
			printf("\nPeer 1 can't get statistics from peer 2!");
			break;
		}
		if (rss2==0)
		{
			printf("\nPeer 2 can't get statistics from peer 1!");
			break;
		}
		if (rss1->messageSendBuffer[HIGH_PRIORITY]>lastMessagesPerSecond1)
		{
			riseCount1++;
			lastMessagesPerSecond1=rss1->messageSendBuffer[HIGH_PRIORITY];
		}
		else
			riseCount1=0;
		if (rss2->messageSendBuffer[HIGH_PRIORITY]>lastMessagesPerSecond1)
		{
			riseCount2++;
			lastMessagesPerSecond1=rss1->messageSendBuffer[HIGH_PRIORITY];
		}
		else
			riseCount1=0;
		currentTime=RakNet::GetTime();
		lastPrint=messagesPerSecond/2000;
		messagesPerSecond+=2*(currentTime-lastTime);
		if (messagesPerSecond/2000 != lastPrint)
			printf("%i ", messagesPerSecond);
		lastTime=currentTime;
		for (i=0; i < messagesPerSecond; i++)
		{
			peer1->Send(dummyData,sendSize, HIGH_PRIORITY, packetReliability, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
			peer2->Send(dummyData,sendSize, HIGH_PRIORITY, packetReliability, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
		p=peer1->Receive();
		while (p)
		{
			peer1->DeallocatePacket(p);
			p=peer1->Receive();
		}
		p=peer2->Receive();
		while (p)
		{
			peer2->DeallocatePacket(p);
			p=peer2->Receive();
		}
		Sleep(0);
	}

	// This means that messages were added faster than they were acknowledged
	printf("\nOutgoing message buffer not cleared at %i messages per second\n", messagesPerSecond);
	peer1->Shutdown(500);
	peer2->Shutdown(500);
}