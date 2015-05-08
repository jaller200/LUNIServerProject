#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include <stdlib.h> // For atoi
#include <cstring> // For strlen
#include "Rand.h"
#include "RakNetStatistics.h"
#include "MessageIdentifiers.h"
#include <stdio.h>
#include "Kbhit.h"
#include "GetTime.h"
#include "RakAssert.h"
#include "RakSleep.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

static const int NUM_CLIENTS=500;
#define SERVER_PORT 66666
#define RANDOM_DATA_SIZE 200
char randomData[RANDOM_DATA_SIZE];

// Connects, sends data over time, disconnects, repeat
class Client
{
	public:
		Client()
		{
			peer = RakNetworkFactory::GetRakPeerInterface();
		}
		~Client()
		{
			RakNetworkFactory::DestroyRakPeerInterface(peer);
		}
		void Startup(void)
		{
			SocketDescriptor socketDescriptor;
			socketDescriptor.port=0;
			nextSendTime=0;
			peer->Startup(1,30,&socketDescriptor,1);
			flipConnectionTime=RakNet::GetTime()+(randomMT()%30000);
			isConnected=false;
		}
		void Connect(void)
		{
			bool b;
			b=peer->Connect("127.0.0.1", (unsigned short) SERVER_PORT, 0, 0, 0);
			if (b==false)
			{
				printf("Client connect call failed!\n");
			}
		}
		void Disconnect(void)
		{
			peer->CloseConnection(peer->GetSystemAddressFromIndex(0),true,0);
			isConnected=false;
		}
		void Update(RakNetTime curTime)
		{
			Packet *p = peer->Receive();
			while (p)
			{
				switch (p->data[0])
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					isConnected=true;
					break;
					// print out errors
				case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Client Error: ID_CONNECTION_ATTEMPT_FAILED\n");
					isConnected=false;
					break;
				case ID_ALREADY_CONNECTED:
					printf("Client Error: ID_ALREADY_CONNECTED\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("Client Error: ID_NO_FREE_INCOMING_CONNECTIONS\n");
					isConnected=false;
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					//printf("ID_DISCONNECTION_NOTIFICATION\n");
					isConnected=false;
					break;
				case ID_CONNECTION_LOST:
					printf("Client Error: ID_CONNECTION_LOST\n");
					isConnected=false;
					break;
				case ID_MODIFIED_PACKET:
					printf("Client Error: ID_MODIFIED_PACKET\n");
					break;
				}
				peer->DeallocatePacket(p);
				p = peer->Receive();
			
			}

			if (curTime>flipConnectionTime)
			{
				if (isConnected)
				{
					Disconnect();
					flipConnectionTime=curTime+5000+(randomMT()%10000);
				}
				else
				{
					Connect();
					flipConnectionTime=curTime+5000+(randomMT()%15000);
				}
			}

			if (curTime>nextSendTime && isConnected)
			{
				peer->Send((const char*)&randomData,RANDOM_DATA_SIZE,HIGH_PRIORITY,RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
				nextSendTime=curTime+500+(randomMT()%1000);
			}
		}

		bool isConnected;
		RakPeerInterface *peer;
		RakNetTime nextSendTime;
		RakNetTime flipConnectionTime;
};

// Just listens for ID_USER_PACKET_ENUM and validates its integrity
class Server
{
	public:
		Server()
		{
			peer = RakNetworkFactory::GetRakPeerInterface();
		}
		~Server()
		{
			RakNetworkFactory::DestroyRakPeerInterface(peer);
		}
		void Start(void)
		{
			SocketDescriptor socketDescriptor;
			socketDescriptor.port=(unsigned short) SERVER_PORT;
			peer->Startup((unsigned short) NUM_CLIENTS,0,&socketDescriptor,1);
			peer->SetMaximumIncomingConnections(NUM_CLIENTS);
		}
		unsigned ConnectionCount(void) const
		{
			unsigned i,count;
			for (i=0,count=0; i < NUM_CLIENTS;i++)
				if (peer->GetSystemAddressFromIndex(i)!=UNASSIGNED_SYSTEM_ADDRESS)
					count++;
			return count;
		}
		void Update(RakNetTime curTime)
		{
			Packet *p = peer->Receive();
			while (p)
			{
				switch (p->data[0])
				{
				case ID_CONNECTION_LOST:
				case ID_DISCONNECTION_NOTIFICATION:
				case ID_NEW_INCOMING_CONNECTION:
					printf("Connections = %i\n", ConnectionCount());
					break;
				case ID_USER_PACKET_ENUM:
					{
						if (memcmp(p->data, randomData, RANDOM_DATA_SIZE)!=0)
						{
							printf("Bad data on server!\n");
						}
						break;
					}
				}
				peer->DeallocatePacket(p);
				p = peer->Receive();
			}
		}
		

		RakPeerInterface *peer;
};

int main(void)
{
	Client clients[NUM_CLIENTS];
	Server server;
//	int clientIndex;
	int mode;

	printf("Connects many clients to a single server.\n");
	printf("Difficulty: Intermediate\n\n");
	printf("Run as (S)erver or (C)lient or (B)oth? ");
	char ch = getche();
	if (ch=='s' || ch=='S')
		mode=0;
	else if (ch=='c' || ch=='c')
		mode=1;
	else
		mode=2;
	printf("\n");

	unsigned i;
	randomData[0]=ID_USER_PACKET_ENUM;
	for (i=0; i < RANDOM_DATA_SIZE; i++)
		randomData[i+1]=i;

	if (mode==0 || mode==2)
	{
		server.Start();
		printf("Started server\n");
	}
	if (mode==1 || mode==2)
	{
		printf("Starting clients...\n");
		for (i=0; i < NUM_CLIENTS; i++)
			clients[i].Startup();
		printf("Started clients\n");
	}
	
	RakNetTime endTime = RakNet::GetTime()+60000*5;
	RakNetTime time = RakNet::GetTime();
	while (time < endTime)
	{
		if (mode==0 || mode==2)
			server.Update(time);
		if (mode==1 || mode==2)
		{
			for (i=0; i < NUM_CLIENTS; i++)
				clients[i].Update(time);
		}

		if (kbhit())
		{
			if (getch()==' ')
			{
				unsigned count=0;
				for (i=0; i < NUM_CLIENTS; i++)
					if (clients[i].peer->IsConnected(clients[i].peer->GetSystemAddressFromIndex(0)))
						count++;
				printf("%i clients connected", count);
			}			
		}

		time = RakNet::GetTime();
		Sleep(30);
	}

	printf("Test completed");
	return 0;
}
