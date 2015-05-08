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

// Server only includes
#include "AutopatcherServer.h"
// Replace this repository with your own implementation if you don't want to use PostgreSQL
#include "AutopatcherPostgreRepository.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

void main(int argc, char **argv)
{
	RakPeerInterface *rakPeer;
	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	printf("Server starting... ");
	SocketDescriptor socketDescriptor(60000,0);
	rakPeer->Startup(8,0,&socketDescriptor, 1);
	rakPeer->SetMaximumIncomingConnections(8);
	printf("started.\n");

	AutopatcherServer autopatcherServer;
	rakPeer->AttachPlugin(&autopatcherServer);
	AutopatcherPostgreRepository postgre;
	autopatcherServer.SetAutopatcherRepositoryInterface(&postgre);
	FLP_Printf progressIndicator;
	FileListTransfer fileListTransfer;
	// Use printf to write what happens to the console window
	fileListTransfer.SetCallback(&progressIndicator);
	autopatcherServer.SetFileListTransferPlugin(&fileListTransfer);
	rakPeer->AttachPlugin(&fileListTransfer);
	printf("Enter database password:\n");
	char connectionString[256],password[128];
	char username[256];
	strcpy(username, "postgres");
	gets(password);
	strcpy(connectionString, "user=");
	strcat(connectionString, username);
	strcat(connectionString, " password=");
	strcat(connectionString, password);
	if (postgre.Connect(connectionString)==false)
	{
		printf("Database connection failed.\n");
		return;
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
				if (postgre.CreateAutopatcherTables()==false)
					printf("%s", postgre.GetLastError());
			}
			else if (ch=='d')
			{
                if (postgre.DestroyAutopatcherTables()==false)
					printf("%s", postgre.GetLastError());
			}
			else if (ch=='a')
			{
				printf("Enter application name to add: ");
				char appName[512];
				gets(appName);
				if (appName[0]==0)
					strcpy(appName, "TestApp");

				if (postgre.AddApplication(appName, username)==false)
					printf("%s", postgre.GetLastError());
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

				if (postgre.RemoveApplication(appName)==false)
					printf("%s", postgre.GetLastError());
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

				if (postgre.UpdateApplicationFiles(appName, appDir, username, &progressIndicator)==false)
				{
					printf("%s", postgre.GetLastError());
				}
				else
				{
					printf("Update success.\n");
				}
			}
		}

		Sleep(30);
	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}
