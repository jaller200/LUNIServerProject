#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "RakNetStatistics.h"
#include <cstdio>
#include <memory.h>
#include <cstring>
#include <stdlib.h>

#ifdef WIN32
#include "Kbhit.h"
#else
#include "Kbhit.h"
#endif

int main(void)
{
	RakPeerInterface *rakServer;
	RakPeerInterface *rakClient;
	char ch;
	char str[255], remoteIP[255];
	char randomData[8192];
	int localPort, remotePort;
	int packetSize;
	int sendinterval;
	RakNetTime time;
	Packet *p;
	RakNetTime lastPacketReceipt, lastNotification, lastSend;
	#ifndef _WIN32
	char buff[256];
	#endif
	
	memset(randomData, 255, sizeof(randomData));
	
	printf("This project is used to test two systems sending two each other with\n");
	printf("variable message sends rates and payload sizes\n");
	printf("Difficulty: Beginner\n\n");

	printf("Start relay (s)erver or start (c)lient?\n");
#ifndef _WIN32
	gets(buff);
	ch=buff[0];
#else
	ch=getch();
#endif
	if (ch=='s' || ch=='S')
	{
		printf("Acting as server.\n");
		rakServer=RakNetworkFactory::GetRakPeerInterface();
		rakClient=0;
	}
	else
	{
		printf("Acting as client.\n");
		rakClient=RakNetworkFactory::GetRakPeerInterface();
		rakServer=0;
	}

	printf("Enter local port: ");
	gets(str);
	if (str[0]==0)
	{
		if (rakServer)
			localPort=60000;
		else
			localPort=0;
	}
	else
		localPort=atoi(str);

	if (rakServer)
	{
		printf("(H)igh priority thread or (R)egular?\n");
#ifndef _WIN32
		gets(buff);
		ch=buff[0];
#else
		ch=getch();
#endif
		if (ch=='h' || ch=='H')
		{
			SocketDescriptor socketDescriptor(localPort,0);
			rakServer->Startup(100, 0, &socketDescriptor, 1);			
			printf("Server started under high priority\n");
		}
		else
		{
			SocketDescriptor socketDescriptor(localPort,0);
			rakServer->Startup(100, 30, &socketDescriptor, 1);
			printf("Server started under regular priority\n");
		}
		rakServer->SetMaximumIncomingConnections(100);
	}
	else
	{
		printf("Enter remote IP: ");
		gets(remoteIP);
		if (remoteIP[0]==0)
			strcpy(remoteIP, "127.0.0.1");
		printf("Enter remote port: ");
		gets(str);
		if (str[0]==0)
			remotePort=60000;
		else
			remotePort=atoi(str);
		printf("(H)igh priority thread or (R)egular?\n");
#ifndef _WIN32
		gets(buff);
		ch=buff[0];
#else
		ch=getch();
#endif
		
		if (ch=='h' || ch=='H')
		{
			SocketDescriptor socketDescriptor(localPort,0);
			rakClient->Startup(1, 0, &socketDescriptor, 1);
			printf("Client started under high priority\n");
		}
		else
		{
			SocketDescriptor socketDescriptor(localPort,0);
			rakClient->Startup(1, 30, &socketDescriptor, 1);
			printf("Client started under regular priority.  Attempting connection...\n");
		}
		rakClient->Connect(remoteIP, remotePort, 0, 0);
	}

	printf("Entering loop.\nHit 'h' for help.\nHit 'q' to quit\n'i' to increase send interval\n'd' to decrease send interval\n'+' to increase packet size\n'-' to decrease packet size.\nSpace to show current statistics\n");

	sendinterval=128;
	packetSize=64;
	lastPacketReceipt=lastNotification=RakNet::GetTime();
	lastSend=0;

	while (1)
	{
		time=RakNet::GetTime();

		if (kbhit())
		{
#ifndef _WIN32
			gets(buff);
			ch=buff[0];
#else
			ch=getch();
#endif
			if (ch=='q')
			{
				printf("Quitting\n");
				break;
			}
			else if (ch=='i')
			{
				sendinterval*=2;				
				printf("Send interval is now %i\n", sendinterval);
			}
			else if (ch=='d')
			{
				if (sendinterval>1)
					sendinterval/=2;
				printf("Send interval is now %i\n", sendinterval);
			}
			if (ch=='h')
				printf("Hit 'h' for help.\nHit 'q' to quit\n'i' to increase send interval\n'd' to decrease send interval\n'+' to increase packet size\n'-' to decrease packet size.\nSpace to show current statistics\n");
			else if (ch=='+')
			{
				if (packetSize < 8192)
					packetSize*=2;
				printf("Packet size is now %i\n", packetSize);
			}
			else if (ch=='-')
			{
				if (packetSize>1)
					packetSize/=2;
				printf("Packet size is now %i\n", packetSize);
			}
			else if (ch==' ')
			{
				if (rakServer)
				{
					StatisticsToString(rakServer->GetStatistics(rakServer->GetSystemAddressFromIndex(0)), randomData, 1);
					printf("%s", randomData);
				}
				else
				{
					StatisticsToString(rakClient->GetStatistics(rakClient->GetSystemAddressFromIndex(0)), randomData, 1);
					printf("%s", randomData);
				}

				printf("Send interval is %i\n", sendinterval);
				printf("Packet size is %i\n", packetSize);
				printf("\n");
			}

			
			ch=0;
		}

		// get packets
		if (rakServer)
			p = rakServer->Receive();
		else
			p=rakClient->Receive();

		if (p)
		{
			lastPacketReceipt=RakNet::GetTime();

			switch (p->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;

			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION\n");
				break;

			case ID_MODIFIED_PACKET:
				// Cheater!
				printf("ID_MODIFIED_PACKET\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;
			default:
				// Relay
				if (rakServer)
					rakServer->Send((char*)p->data, p->length, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);
			}

			if (rakServer)
				rakServer->DeallocatePacket(p);
			else
				rakClient->DeallocatePacket(p);
		}

		if ((rakServer && rakServer->NumberOfConnections()>0) ||
			(rakClient && rakClient->NumberOfConnections()>0))
		{
			// Do sends
			if (lastSend + (RakNetTime)sendinterval < time)
			{
				if (rakServer)
				{
					rakServer->Send((char*)randomData, packetSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else if (rakClient)
				{
					rakClient->Send((char*)randomData, packetSize, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}

				lastSend=time;
			}		

			if (lastPacketReceipt + 500 < (unsigned int)time && lastNotification + 500 < (unsigned int)time)
			{
				lastNotification=time;
				printf("Warning: No packets for %i ms.  Possibly a spike.\n", time - lastPacketReceipt);
			}
		}
	}

	if (rakServer)
		RakNetworkFactory::DestroyRakPeerInterface(rakServer);
	else
		RakNetworkFactory::DestroyRakPeerInterface(rakClient);

	return 1;
}
