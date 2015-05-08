#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Kbhit.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "MessageIdentifiers.h"
#include "PacketLogger.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "BitStream.h"

#include "Common.h"

RakPeerInterface *rakClient;
CustomPacketLogger clientLogger;
NetworkIDManager clientNetworkIDManager;

void RunAsClient(void)
{

	rakClient = RakNetworkFactory::GetRakPeerInterface();
	// Apple is defined in "Common.h", and it's our class derived from NetworkIDObject
	Apple *serverApple = 0;

#if DOLOG
	clientLogger.SetPrefix("CLIENT> ");
	clientLogger.SetSuffix("\n");
	rakClient->AttachPlugin(&clientLogger);
#endif

	char serverip[256];
	printf("Enter ip of the server (ENTER defaults to 127.0.0.1): ");
	gets(serverip);
	if (serverip[0]==0) strcpy(serverip, "127.0.0.1");
	char serverport[256];
	printf("Enter port of the server (ENTER defaults to 60000): ");
	gets(serverport);
	if (serverport[0]==0) strcpy(serverport, "60000");

	rakClient->Startup(1, 30, &SocketDescriptor(), 1);
	rakClient->Connect(serverip, atoi(serverport), 0,0);

	// In a client/server architecture, only the server can create NetworkIDs
	clientNetworkIDManager.SetIsNetworkIDAuthority(false);

	rakClient->SetNetworkIDManager(&clientNetworkIDManager);

	// Clear keyboard buffer before entering loop
	while(kbhit()) getch();
	do{

		Packet *p = rakClient->Receive();
		while(p){
			unsigned char msgID = p->data[0];

			// Check if the server has sent us the NetworkID of his Apple object
			if (msgID==ID_USER_SERVERAPPLE_CREATED) {
				RakNet::BitStream bs(p->data, p->length, false);
				// Ignore the message ID
				bs.IgnoreBits(8);

				NetworkID appleNetworkID;
				bs.Read(appleNetworkID);

				// Create the object on the client side
				serverApple = new GrannySmith;
				serverApple->SetNetworkIDManager(&clientNetworkIDManager);
				serverApple->SetNetworkID(appleNetworkID);

				// Now that we have the object created, we can use RPC on it.
				const char *hello="Hello World!!!!!";
				rakClient->RPC(CLASS_MEMBER_ID(Apple,func1), hello, (unsigned int) (strlen(hello)+1)*8, HIGH_PRIORITY, RELIABLE_ORDERED,0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, serverApple->GetNetworkID(),0);
				rakClient->RPC(CLASS_MEMBER_ID(Apple,func2), hello, (unsigned int) (strlen(hello)+1)*8, HIGH_PRIORITY, RELIABLE_ORDERED,0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, serverApple->GetNetworkID(),0);
				rakClient->RPC(CLASS_MEMBER_ID(Apple,func3), hello, (unsigned int) (strlen(hello)+1)*8, HIGH_PRIORITY, RELIABLE_ORDERED,0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, serverApple->GetNetworkID(),0);
			}

			rakClient->DeallocatePacket(p);
			p = rakClient->Receive();
		}

		// Sleep for a while, so we don't eat all the CPU time with such a simple program.
		// In a real game, you probably won't need this, as you want your game to run at full speed.
		RakSleep(30);

	} while(!kbhit());


	// Shutdown
	if (serverApple) delete serverApple;
	rakClient->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(rakClient);
}