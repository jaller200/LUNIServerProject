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
#include "Rand.h"
#include "Kbhit.h"
#include <stdio.h> // Printf
#include <windows.h> // Sleep

void main(void)
{
	printf("An internal test to test two peers connecting to each other\n");
	printf("at the same time.  This causes bugs so I fix them here\n");
	printf("Difficulty: Beginner\n\n");

	PacketLogger pl1, pl2;
	RakPeerInterface *peer1, *peer2;
	peer1=RakNetworkFactory::GetRakPeerInterface();
	peer2=RakNetworkFactory::GetRakPeerInterface();
	peer1->AttachPlugin(&pl1);
	peer2->AttachPlugin(&pl2);
	peer1->ApplyNetworkSimulator(128000, 0, 100);
	peer2->ApplyNetworkSimulator(128000, 0, 100);
	SocketDescriptor socketDescriptor(1234,0);
	peer1->Startup(1,0,&socketDescriptor, 1);
	socketDescriptor.port=1235;
	peer2->Startup(1,0,&socketDescriptor, 1);
	peer1->SetMaximumIncomingConnections(1);
	peer2->SetMaximumIncomingConnections(1);
	seedMT(0);
	pl1.LogHeader();
	peer1->Connect("127.0.0.1", 1235, 0, 0);
	peer2->Connect("127.0.0.1", 1234, 0, 0);
	Sleep(5000);
	peer1->Shutdown(1000);
	peer2->Shutdown(1000);
	Sleep(1000);
	printf("Press any key to quit\n");
	getch();
	RakNetworkFactory::DestroyRakPeerInterface(peer1);
	RakNetworkFactory::DestroyRakPeerInterface(peer2);
}
