// Common includes
#include <stdio.h>
#include <stdlib.h>
#include "Kbhit.h"
#include "RakNetworkFactory.h"
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

#include "LobbyServer.h"
#include "LobbyClientInterface.h"
// Replace this repository with your own implementation if you don't want to use PostgreSQL
#include "LobbyServer_PostgreSQL.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

using namespace RakNet;

void ProcessPackets(void);
RakPeerInterface *rakPeer;

void main(int argc, char **argv)
{
	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	printf("Server starting... ");
	SocketDescriptor socketDescriptor(60000,0);
	rakPeer->Startup(8,100,&socketDescriptor, 1);
	rakPeer->SetMaximumIncomingConnections(8);
	printf("started.\n");

	printf("Enter database password:\n");
	char connectionString[512],password[128];
	char username[512];
	strcpy(username, "postgres");
	gets(password);
	strcpy(connectionString, "user=");
	strcat(connectionString, username);
	strcat(connectionString, " password=");
	strcat(connectionString, password);

	LobbyServerPostgreSQL lobbyServer;

	// Connect to the PostgreSQL database.
	if (lobbyServer.ConnectToDB(connectionString)==false)
	{
		printf("Database connection failed.\n");
		return;
	}
	printf("Database connection suceeded.\n");

	// Starts the lobby server threads.
	lobbyServer.Startup();
	printf("Threads started.\n");

	// Last step is to attach the lobby server plugin so it can get packets through RakNet
	rakPeer->AttachPlugin(&lobbyServer);

	char inputStr[512];
	printf("(C)reate tables\n");
	printf("(D)estroy tables\n");
	printf("(A)dd titles\n");
	printf("(Q)uit\n");

	printf("Entering endless loop...\n");
	while (1)
	{
		if (kbhit())
		{
			char ch=getch();
			if (ch=='a')
			{
				/// This functor asynchronously adds a title to the database. Full sample in LobbyDB_PostgreSQLTest
				AddTitle_PostgreSQLImpl *functor = AddTitle_PostgreSQLImpl::Alloc();
				printf("Adds a title to the database\n");

				printf("Enter title name: ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "Hangman EXTREME!");
				functor->titleName = inputStr;
				printf("Enter title password (binary): ");
				gets(inputStr);
				if (inputStr[0]==0) strcpy(inputStr, "SECRET_PER_GAME_LOGIN_PW_PREVIOUSLY_SETUP_ON_THE_DB");
				functor->titlePassword = AddTitle_PostgreSQLImpl::AllocBytes((int) strlen(inputStr));
				functor->titlePasswordLength = (int) strlen(inputStr);
				memcpy(functor->titlePassword, inputStr, functor->titlePasswordLength);
				functor->allowClientAccountCreation=true;
				functor->lobbyIsGameIndependent=true;
				functor->requireUserKeyToLogon=false;
				functor->defaultAllowUpdateHandle=true;
				functor->defaultAllowUpdateCCInfo=true;
				functor->defaultAllowUpdateAccountNumber=true;
				functor->defaultAllowUpdateAdminLevel=true;
				functor->defaultAllowUpdateAccountBalance=true;
				functor->defaultAllowClientsUploadActionHistory=true;

				/// Puts this functor on the processing queue. It will process sometime later in a thread.
				/// See projects LobbyDB_PostgreSQLTest, TitleValidationDB_PostgreSQLTest, RankingServerDBTest for a complete demo of these and other functors
				lobbyServer.PushFunctor(functor);
			}
			else if (ch=='c')
			{
				lobbyServer.CreateTables();
				printf("Creating tables\n");
			}
			else if (ch=='d')
			{
				lobbyServer.DestroyTables();
				printf("Destroying tables\n");
			}
			else if (ch=='q')
			{
				break;
			}
		}
		ProcessPackets();
		RakSleep(100);

	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}

void ProcessPackets(void)
{
	Packet *packet;
	// Read all packets every tick. This also updates all plugins.
	for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION\n");
			break;

		case ID_NEW_INCOMING_CONNECTION:
			// Somebody connected.  We have their IP now
			printf("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString());
			packet->systemAddress; // Record the player ID of the client
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST\n");
			break;
		}

	}

}