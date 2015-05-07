#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include "Kbhit.h"
#include <windows.h> // Sleep
#else
#include "Kbhit.h"
#include <unistd.h> // usleep
#endif

void main(void)
{
	Packet *packet;
	RakPeerInterface *rakPeer;
	bool isConnected=false;
	rakPeer=RakNetworkFactory::GetRakPeerInterface();
	char command[512];
	printf("This sample demonstrates connecting to the command console.\n");
	printf("using the RakNet transport protocol\n");
	printf("It's the equivalent of a secure telnet client\n");
	printf("See the 'CommandConsoleServer' project.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("RakNet secure command console.\n");
	printf("Commands:\n");
	printf("/Connect\n");
	printf("/Disconnect\n");
	printf("/Quit\n");
	printf("Any other command goes to the remote console\n");
	while (1)
	{
		if (kbhit())
		{
			gets(command);

			if (stricmp(command, "/quit")==0)
			{
				printf("Goodbye.\n");
				rakPeer->Shutdown(500, 0);
				return;
			}
			else if (stricmp(command, "/disconnect")==0)
			{
				if (isConnected)
				{
					rakPeer->Shutdown(500, 0);
					isConnected=false;
					printf("Disconnecting.\n");
				}
				else
				{
					printf("Not currently connected.\n");
				}
			}
			else if (stricmp(command, "/connect")==0)
			{
				if (isConnected)
				{
					printf("Disconnect first.\n");
				}
				else
				{
					char ip[128];
					char remotePort[64];
					char password[512];
					char localPort[64];
					printf("Enter remote IP: ");
					do {
						gets(ip);
					} while(ip[0]==0);
					printf("Enter remote port: ");
					do {
						gets(remotePort);
					} while(remotePort[0]==0);
					printf("Enter local port (enter for 0): ");
					gets(localPort);
					if (localPort[0]==0)
					{
						strcpy(localPort, "0");
					}
					printf("Enter console password (enter for none): ");
					gets(password);
					SocketDescriptor socketDescriptor((int) atoi(localPort),0);
					if (rakPeer->Startup(1, 100, &socketDescriptor, 1))
					{
						int passwordLen;
						if (password[0])
							passwordLen=(int) strlen(password)+1;
						else
							passwordLen=0;
						if (rakPeer->Connect(ip, (int) atoi(remotePort), password, passwordLen))
							printf("Connecting...\nNote: if the password is wrong the other system will ignore us.\n");
						else
						{
							printf("Connect call failed.\n");
							rakPeer->Shutdown(0, 0);
						}
					}
					else
						printf("Initialize call failed.\n");					
					
				}				
			}
			else
			{
				if (isConnected)
				{
					RakNet::BitStream str;
					str.Write((unsigned char) ID_TRANSPORT_STRING);
					str.Write(command, (int) strlen(command)+1);
					rakPeer->Send(&str, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else
				{
					printf("You must be connected to send commands.\n");
				}
			}
		}

		packet = rakPeer->Receive();
		if (packet)
		{
			switch (packet->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				printf("The server disconnected us.\n");
				isConnected=false;
				break;
			case ID_CONNECTION_BANNED:
				printf("We are banned from this server.\n");
				isConnected=false;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed.\nThe password was wrong or there is no responsive machine at that IP/port.\n");
				isConnected=false;
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("Server is full.\n");
				isConnected=false;
				break;
			case ID_CONNECTION_LOST:
				printf("We lost the connection.\n");
				isConnected=false;
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Connection accepted.\n");
				isConnected=true;
				break;
			case ID_TRANSPORT_STRING:
				printf("%s", packet->data+1);
				break;
			}

			rakPeer->DeallocatePacket(packet);
		}

		// This sleep keeps RakNet responsive
#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif
	}
}