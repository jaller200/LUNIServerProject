/// \file
/// \brief Tests multiple readers and writers on the same instance of RakPeer.  Define _RAKNET_THREADSAFE in RakNetDefines.h before running this.
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
#include "GetTime.h"
#include "RakNetStatistics.h"
#include "MessageIdentifiers.h"
#include "Kbhit.h"
#include <stdio.h> // Printf
#include <windows.h> // Sleep
#include <process.h>

RakPeerInterface *peer1, *peer2;
bool endThreads;

unsigned __stdcall ProducerThread( LPVOID arguments )
{
	char i = *((char *) arguments);
	char out[2];
	out[0]=ID_USER_PACKET_ENUM;
	out[1]=i;

	while (endThreads==false)
	{
//		printf("Thread %i writing...\n", i);
		if (i&1)
			peer1->Send(out, 2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
		else
			peer2->Send(out, 2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);

//		printf("Thread %i done writing\n", i);
		Sleep(0);
	}
	return 1;
}

unsigned __stdcall ConsumerThread( LPVOID arguments )
{
	char i = *((char *) arguments);
	Packet *p;
	while (endThreads==false)
	{
//		printf("Thread %i reading...\n", i);
		if (i&1)
			p=peer1->Receive();
		else
			p=peer2->Receive();
	//	printf("Thread %i done reading...\n", i);

		if (p)
		{
			if (p->data[0]==ID_USER_PACKET_ENUM)
				printf("Got data from thread %i\n", p->data[1]);
			peer1->DeallocatePacket(p);
		}

        Sleep(0);		
	}
	return 1;
}

void main(void)
{
	peer1=RakNetworkFactory::GetRakPeerInterface();
	peer2=RakNetworkFactory::GetRakPeerInterface();
	peer1->SetMaximumIncomingConnections(1);
	peer2->SetMaximumIncomingConnections(1);
	SocketDescriptor socketDescriptor(1234,0);
	peer1->Startup(1,0,&socketDescriptor, 1);
	socketDescriptor.port=1235;
	peer2->Startup(1,0,&socketDescriptor, 1);
	peer1->Connect("127.0.0.1", 1235, 0, 0);
	peer2->Connect("127.0.0.1", 1234, 0, 0);
	Sleep(500);


	printf("Tests multiple threads sharing the same instance of RakPeer\n");
	printf("Difficulty: Beginner\n\n");

	printf("Don't forget to define _RAKNET_THREADSAFE in RakNetDefines.h for this test!\n");


	endThreads=false;
	unsigned threadId;
	unsigned i;
	char count[20];
	printf("Starting threads\n");
	for (i=0; i< 10; i++)
	{
		count[i]=i;
		_beginthreadex( NULL, 0, ProducerThread, count+i, 0, &threadId );
	}
	for (; i < 20; i++)
	{
		count[i]=i;
		_beginthreadex( NULL, 0, ConsumerThread, count+i, 0, &threadId );
	}

	printf("Running test\n");
	RakNetTime endTime = 60 * 1000 + RakNet::GetTime();
	while (RakNet::GetTime() < endTime)
	{

		Sleep(0);
	}
	endThreads=true;
	printf("Test done!\n");

	RakNetworkFactory::DestroyRakPeerInterface(peer1);
	RakNetworkFactory::DestroyRakPeerInterface(peer2);
}
