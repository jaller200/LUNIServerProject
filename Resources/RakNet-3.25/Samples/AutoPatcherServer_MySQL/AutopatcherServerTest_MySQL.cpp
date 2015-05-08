// Common includes
#include <stdio.h>
#include <stdlib.h>
#include "Kbhit.h"
#include "RakNetworkFactory.h"
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "FileListTransfer.h"
#include "FileList.h" // FLP_Printf
#include "AutopatcherServer.h"
#include "AutopatcherMySQLRepository.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

int main(int argc, char **argv)
{
	// Avoids the Error: Got a packet bigger than 'max_allowed_packet' bytes
	printf("Important: Requires that you first set the DB schema and the max packet size on the server.\n");
	printf("See DependentExtensions/AutopatcherMySQLRepository/readme.txt\n");
	
	RakPeerInterface *rakPeer;
	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	printf("Server starting... ");
	SocketDescriptor socketDescriptor(60000,0);
	rakPeer->Startup(8,0,&socketDescriptor, 1);
	rakPeer->SetMaximumIncomingConnections(8);
	printf("started.\n");

	AutopatcherServer autopatcherServer;
	rakPeer->AttachPlugin(&autopatcherServer);
	AutopatcherMySQLRepository repository;
	autopatcherServer.SetAutopatcherRepositoryInterface(&repository);
	FLP_Printf progressIndicator;
	FileListTransfer fileListTransfer;
	fileListTransfer.SetCallback(&progressIndicator);
	autopatcherServer.SetFileListTransferPlugin(&fileListTransfer);
	rakPeer->AttachPlugin(&fileListTransfer);
	printf("Enter database password:\n");
	char password[128];
	char username[256];
	strcpy(username, "root");
	gets(password);
	if (password[0]==0)
		strcpy(password,"aaaa");
	char db[256];
	printf("Enter DB schema: ");
	gets(db);
	if (db[0]==0)
		strcpy(db,"autopatcher");
	if (!repository.Connect("localhost", username, password, db, 0, NULL, 0))
	{
		printf("Database connection failed.\n");
		return 1;
	}
	printf("Database connection suceeded.\n");
	printf("(D)rop database\n(C)reate database.\n(A)dd application\n(U)pdate revision.\n(R)emove application\n(Q)uit\n");

	char ch;
	Packet *p;
	while (1)
	{
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
				printf("ID_NEW_INCOMING_CONNECTION\n");
			else if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
				printf("ID_DISCONNECTION_NOTIFICATION\n");
			else if (p->data[0]==ID_CONNECTION_LOST)
				printf("ID_CONNECTION_LOST\n");

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (kbhit())
		{
			ch=getch();
			if (ch=='q')
				break;
			else if (ch=='c')
			{
				if (repository.CreateAutopatcherTables()==false)
					printf("Error: %s\n", repository.GetLastError());
				else
					printf("Created\n");
			}
			else if (ch=='d')
			{
				if (repository.DestroyAutopatcherTables()==false)
					printf("Error: %s\n", repository.GetLastError());
				else
					printf("Destroyed\n");
			}
			else if (ch=='a')
			{
				printf("Enter application name to add: ");
				char appName[512];
				gets(appName);
				if (appName[0]==0)
					strcpy(appName, "TestApp");

				if (repository.AddApplication(appName, username)==false)
					printf("Error: %s\n", repository.GetLastError());
				else
					printf("Done\n");
			}
			else if (ch=='r')
			{
				printf("Enter application name to remove: ");
				char appName[512];
				gets(appName);
				if (appName[0]==0)
					strcpy(appName, "TestApp");

				if (repository.RemoveApplication(appName)==false)
					printf("Error: %s\n", repository.GetLastError());
				else
					printf("Done\n");
			}
			else if (ch=='u')
			{
				printf("Enter application name: ");
				char appName[512];
				gets(appName);
				if (appName[0]==0)
					strcpy(appName, "TestApp");

				printf("Enter application directory: ");
				char appDir[512];
				gets(appDir);
				if (appDir[0]==0)
					strcpy(appDir, "C:/temp");

				if (repository.UpdateApplicationFiles(appName, appDir, username, &progressIndicator)==false)
				{
					printf("Error: %s\n", repository.GetLastError());
				}
				else
				{
					printf("Update success.\n");
				}
			}
		}

		RakSleep(30);


	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}
