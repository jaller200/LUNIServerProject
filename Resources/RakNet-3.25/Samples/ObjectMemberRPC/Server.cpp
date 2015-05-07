#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Kbhit.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "PacketLogger.h"
#include "RakNetworkFactory.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "BitStream.h"
#include "Common.h"

RakPeerInterface *rakServer;

CustomPacketLogger serverLogger;
NetworkIDManager serverNetworkIDManager;

void RunAsServer(void)
{
	rakServer = RakNetworkFactory::GetRakPeerInterface();

#if DOLOG
	serverLogger.SetPrefix("SERVER> ");
	serverLogger.SetSuffix("\n");
	rakServer->AttachPlugin(&serverLogger);
#endif

	char port[256];
	printf("Enter port to run the server (ENTER defaults to 60000): ");
	gets(port);
	if (port[0]==0) strcpy(port, "60000");

	rakServer->Startup(SERVER_MAXCLIENTS, 30, &SocketDescriptor(atoi(port), 0), 1);
	rakServer->SetMaximumIncomingConnections(SERVER_MAXCLIENTS);
	printf("Running Server...\n");

	// Register RPCs
	REGISTER_CLASS_MEMBER_RPC(rakServer, Apple, func1);
	REGISTER_CLASS_MEMBER_RPC(rakServer, Apple, func2);
	REGISTER_CLASS_MEMBER_RPC(rakServer, Apple, func3);

	// In a client/server architecture, only the server can create NetworkIDs
	serverNetworkIDManager.SetIsNetworkIDAuthority(true);  // The server has the authority to create NetworkIDs

	rakServer->SetNetworkIDManager(&serverNetworkIDManager);

	// Create the object that we'll use with RPC
	Apple *apple = new GrannySmith;
	apple->SetNetworkIDManager(&serverNetworkIDManager);

	// Clean keyboard buffer before loop
	while (kbhit()) getch();
	do {
		
		Packet *p = rakServer->Receive();
		while(p){
			unsigned char msgID = p->data[0];

			// If a new system connects, send to that system the NetworkID of our Apple object,
			// so that he can use RPC on that object
			if (msgID==ID_NEW_INCOMING_CONNECTION)
			{
				RakNet::BitStream bs;
				bs.Write(unsigned char(ID_USER_SERVERAPPLE_CREATED));
				bs.Write(apple->GetNetworkID());
				rakServer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
			}

			rakServer->DeallocatePacket(p);
			p = rakServer->Receive();
		}

		// Sleep for a while, so we don't eat all the CPU time with such a simple program.
		// In a real game, you probably won't need this, as you want your game to run at full speed.
		RakSleep(30);

	} while(!kbhit());


	delete apple;

	// Shutdown
	rakServer->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}