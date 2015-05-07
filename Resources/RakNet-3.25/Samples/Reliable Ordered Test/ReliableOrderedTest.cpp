#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include <cstdio>
#include <memory.h>
#include <cstring>
#include <stdlib.h>
#include "Rand.h"
#include "RakNetStatistics.h"

#ifdef _WIN32
#include "Kbhit.h"
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

int main(void)
{
	RakPeerInterface *sender, *receiver;
	unsigned int packetNumber[32], receivedPacketNumber, receivedTime;
	char str[256];
	char ip[32];
	RakNetTime sendInterval, nextSend, currentTime, quitTime;
	unsigned short remotePort, localPort;
	unsigned char streamNumber;
	RakNet::BitStream bitStream;
	Packet *packet;
	bool doSend=false;

	for (int i=0; i < 32; i++)
		packetNumber[i]=0;
	
	printf("This project tests RakNet's reliable ordered sending system.\n");
	printf("Difficulty: Beginner\n\n");

	printf("Act as (s)ender or (r)eceiver?\n");
	gets(str);
	if (str[0]==0)
		return 1;

	if (str[0]=='s' || str[0]=='S')
	{
		sender = RakNetworkFactory::GetRakPeerInterface();
		receiver = 0;

		printf("Enter number of ms to pass between sends: ");
		gets(str);
		if (str[0]==0)
			sendInterval=5;
		else
			sendInterval=atoi(str);

		printf("Enter remote IP: ");
		gets(ip);
		if (ip[0]==0)
			strcpy(ip, "127.0.0.1");
		
		printf("Enter remote port: ");
		gets(str);
		if (str[0]==0)
			strcpy(str, "60000");
		remotePort=atoi(str);

		printf("Enter local port: ");
		gets(str);
		if (str[0]==0)
			strcpy(str, "0");
		localPort=atoi(str);


		printf("Connecting...\n");
		SocketDescriptor socketDescriptor(localPort,0);
		sender->Startup(1, 30, &socketDescriptor, 1);
		sender->Connect(ip, remotePort, 0, 0);
	}
	else
	{
		receiver = RakNetworkFactory::GetRakPeerInterface();
		sender=0;

		printf("Enter local port: ");
		gets(str);
		if (str[0]==0)
			strcpy(str, "60000");
		localPort=atoi(str);

		printf("Waiting for connections...\n");
		SocketDescriptor socketDescriptor(localPort,0);
		receiver->Startup(32, 30, &socketDescriptor, 1);
		receiver->SetMaximumIncomingConnections(32);
	}

//	if (sender)
//		sender->ApplyNetworkSimulator(128000, 50, 100);
//	if (receiver)
//		receiver->ApplyNetworkSimulator(128000, 50, 100);

	
	printf("How long to run this test for, in seconds?\n");
	gets(str);
	if (str[0]==0)
		strcpy(str, "12000");
	
	currentTime = RakNet::GetTime();
	quitTime = atoi(str) * 1000 + currentTime;

	nextSend=currentTime;

	printf("Test running.\n");

	while (currentTime < quitTime)
	{
#ifdef _WIN32
		if (kbhit())
		{
			char ch=getch();
			if (ch=='q')
				return 1;
			else if (ch==' ')
			{
				RakNetStatistics *rss;
				char message[2048];
				if (sender)
					rss=sender->GetStatistics(sender->GetSystemAddressFromIndex(0));
				else
					rss=receiver->GetStatistics(receiver->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
			}
		}
#endif
		if (sender)
		{
			packet = sender->Receive();
			while (packet)
			{
				// PARSE TYPES
				switch(packet->data[0])
				{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
					doSend=true;
					nextSend=currentTime;
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_CONNECTION_LOST:
					printf("ID_CONNECTION_LOST\n");
					break;
				case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Connection attempt failed\n");
					break;
				}

				sender->DeallocatePacket(packet);
				packet = sender->Receive();
			}
			
			while (doSend && currentTime > nextSend)
			{
				streamNumber=0;
			//	streamNumber = randomMT() % 32;
				// Do the send
				bitStream.Reset();
				bitStream.Write((unsigned char) (ID_USER_PACKET_ENUM+1));
				bitStream.Write(packetNumber[streamNumber]++);
				bitStream.Write(streamNumber);
				bitStream.Write(currentTime);
				char *pad;
				int padLength = 1000; // (randomMT() % 5000) + 1;
				pad = new char [padLength];
				bitStream.Write(pad, padLength);
				delete [] pad;
				// Send on a random priority with a random stream
				// if (sender->Send(&bitStream, HIGH_PRIORITY, (PacketReliability) (RELIABLE + (randomMT() %2)) ,streamNumber, UNASSIGNED_SYSTEM_ADDRESS, true)==false)
				if (sender->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED ,streamNumber, UNASSIGNED_SYSTEM_ADDRESS, true)==false)
					packetNumber[streamNumber]--; // Didn't finish connecting yet?

				
				if (sender)
				{
					RakNetStatistics *rssSender;
					rssSender=sender->GetStatistics(sender->GetSystemAddressFromIndex(0));
					printf("Snd: %i waiting. %i waiting on ack. Got %i acks. KBPS=%.1f. Ploss=%.1f. Full=%i.\n", rssSender->messageSendBuffer[HIGH_PRIORITY], rssSender->messagesOnResendQueue,rssSender->acknowlegementsReceived, rssSender->bitsPerSecond/1000, 100.0f * ( float ) rssSender->messagesTotalBitsResent / ( float ) rssSender->totalBitsSent, rssSender->bandwidthExceeded);
				}

				nextSend+=sendInterval;
			}
		}
		else
		{
			packet = receiver->Receive();
			while (packet)
			{
				switch(packet->data[0])
				{
				case ID_NEW_INCOMING_CONNECTION:
					printf("ID_NEW_INCOMING_CONNECTION\n");
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_CONNECTION_LOST:
					printf("ID_CONNECTION_LOST\n");
					break;
				case ID_USER_PACKET_ENUM+1:
					bitStream.Reset();
					bitStream.Write((char*)packet->data, packet->length);
					bitStream.IgnoreBits(8); // Ignore ID_USER_PACKET_ENUM+1
					bitStream.Read(receivedPacketNumber);
					bitStream.Read(streamNumber);
					bitStream.Read(receivedTime);

					if (receivedPacketNumber!=packetNumber[streamNumber])
						printf("ERROR! Expecting %i got %i (channel %i).",packetNumber[streamNumber], receivedPacketNumber, streamNumber);
					else
						printf("Got %i.Channel %i.Len %i.", packetNumber[streamNumber], streamNumber, packet->length);

					printf("Sent=%u Received=%u Diff=%i.\n", receivedTime, currentTime, (int)currentTime - (int) receivedTime);

					packetNumber[streamNumber]++;
					break;
				}

				
				receiver->DeallocatePacket(packet);
				packet = receiver->Receive();
			}
		}

// DO NOT COMMENT OUT THIS SLEEP!
// This sleep keeps RakNet responsive
#ifdef _WIN32
		Sleep(0);
#else
		usleep(0);
#endif
		currentTime=RakNet::GetTime();
	}

	printf("Press any key to continue\n");
	gets(str);

	if (sender)
		RakNetworkFactory::DestroyRakPeerInterface(sender);
	if (receiver)
		RakNetworkFactory::DestroyRakPeerInterface(receiver);

	return 1;
}