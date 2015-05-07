#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "Rand.h"
#include "RSACrypt.h"
#include "DataBlockEncryptor.h"
#include "Rand.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include <assert.h>

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

void PrintOptions(void)
{
	printf("1. Generate RSA keys and save to disk.\n");
	printf("2. Load RSA keys from disk.\n");
	printf("3. Test peers with key.\n");
	printf("(H)elp.\n");
	printf("(Q)uit.\n");
}

RakPeerInterface *rakPeer1, *rakPeer2;

void PrintPacketHeader(Packet *packet)
{
	switch (packet->data[0])
	{
		case ID_RSA_PUBLIC_KEY_MISMATCH:
			printf("Public key mismatch.\nThe connecting system's public key does not\nmatch what the sender sent.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			printf("Connection request accepted.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("New incoming connection.\n");
			break;
		case ID_MODIFIED_PACKET:
			printf("Packet checksum invalid.  Either RSA decrypt function gave the wrong value\nor the packet was tampered with.\n");
			break;
		default:
			printf("%s\n", packet->data);
			break;
	}
}

int main(void)
{
	char str[256];
	bool keyLoaded; // Does D,E,N have values?

	// RSACrypt is a class that handles RSA encryption/decryption internally
	big::RSACrypt<RSA_BIT_SIZE> rsacrypt;

	// These are the sizes necessary for e,n,p,q
	// e,n is the public key
	// p,q is the private key
	big::u32 e;
	RSA_BIT_SIZE n;
	BIGHALFSIZE(RSA_BIT_SIZE, p);
	BIGHALFSIZE(RSA_BIT_SIZE, q);

	FILE *fp;
	RakNetTime time;
	rakPeer1=RakNetworkFactory::GetRakPeerInterface();
	rakPeer2=RakNetworkFactory::GetRakPeerInterface();
	Packet *packet;
	bool peer1GotMessage, peer2GotMessage;

	keyLoaded=false;

	printf("Demonstrates how to setup RakNet to use secure connections\n");
	printf("Also shows how to read and write RSA keys to and from disk\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Select option:\n");
	PrintOptions();

	while (1)
	{
		gets(str);

		if (str[0]=='1')
		{
            printf("Generating %i byte key.  This will take a while...\n", sizeof(RSA_BIT_SIZE));
			rsacrypt.generateKeys();
			rsacrypt.getPublicKey(e,n);
			rsacrypt.getPrivateKey(p,q);
			keyLoaded=true;
			printf("Key generated.  Save to disk? (y/n)\n");
			gets(str);
			if (str[0]=='y' || str[0]=='Y')
			{
				printf("Enter filename to save public keys to: ");
				gets(str);
                if (str[0])
				{
					printf("Writing public key... ");
					fp=fopen(str, "wb");
					fwrite((char*)&e, sizeof(e), 1, fp);
					fwrite((char*)n, sizeof(n), 1, fp);
					fclose(fp);
					printf("Done.\n");
				}
				else
					printf("\nKey not written.\n");

				printf("Enter filename to save private key to: ");
				gets(str);
				if (str[0])
				{
					printf("Writing private key... ");
					fp=fopen(str, "wb");
					fwrite(p, sizeof(RSA_BIT_SIZE)/2,1,fp);
					fwrite(q, sizeof(RSA_BIT_SIZE)/2, 1, fp);
					fclose(fp);
					printf("Done.\n");
				}
				else
					printf("\nKey not written.\n");
			}
			PrintOptions();
		}
		else if (str[0]=='2')
		{
			printf("Enter filename to load public keys from: ");
			gets(str);
			if (str[0])
			{
				fp=fopen(str, "rb");
				if (fp)
				{
					printf("Loading public keys... ");
					fread((char*)(&e), sizeof(e), 1, fp);
					fread((char*)(n), sizeof(n), 1, fp);
					fclose(fp);
					printf("Done.\n");

					printf("Enter filename to load private key from: ");
					gets(str);
					if (str[0])
					{
						fp=fopen(str, "rb");
						if (fp)
						{
							printf("Loading private key... ");
							fread(p, sizeof(RSA_BIT_SIZE)/2, 1, fp);
							fread(q, sizeof(RSA_BIT_SIZE)/2, 1, fp);
							fclose(fp);
							printf("Done.\n");
							keyLoaded=true;
						}
						else
						{
							printf("Failed to open %s.\n", str);
						}
					}
					else
						printf("Not loading private key.\n");
				}
				else
				{
					printf("Failed to open %s.\n", str);
				}
			}
			else
				printf("Not loading public keys.\n");

			PrintOptions();
		}
		else if (str[0]=='3')
		{
			if (keyLoaded)
			{
				printf("(G)enerate new keys automatically or use (e)xisting?\n");
				gets(str);
				if (str[0]=='g' || str[0]=='G')
				{
					printf("Generating 32 byte keys.  Please wait.\n");
					rakPeer1->InitializeSecurity(0,0,0,0);
					printf("Keys generated.\n");
				}
				else
				{
					rakPeer1->InitializeSecurity(0,0,(char*)p, (char*)q);
					printf("Tell the connecting system the public keys in advance?\n(Y)es, better security.\n(N)o, worse security but everything works automatically.\n");
					gets(str);
					if (str[0]=='y' || str[0]=='Y')
					{
						printf("Using preloaded keys for the connecting system.\n");
						rakPeer2->InitializeSecurity((char*)&e, (char*)n, 0, 0);
					}
					else
					{
						printf("Relying on server to transmit public keys to the connecting system.\n");

						// Clear out any old saved public keys
						rakPeer2->DisableSecurity();
					}
				}
			}
			else
			{
				printf("Generating key automatically on host.  Please wait.\n");
				rakPeer1->InitializeSecurity(0, 0, 0, 0);

				// Clear out any old saved public keys
				rakPeer2->DisableSecurity();
				printf("Key generation complete.\n");
			}
			
			printf("Initializing peers.\n");
			SocketDescriptor socketDescriptor(1234,0);
			rakPeer1->Startup(8,0,&socketDescriptor, 1);
			rakPeer1->SetMaximumIncomingConnections(8);
			socketDescriptor.port=0;
			rakPeer2->Startup(1,0,&socketDescriptor, 1);
			rakPeer2->Connect("127.0.0.1", 1234, 0, 0);
			printf("Running connection for 5 seconds.\n");

			peer1GotMessage=false;
			peer2GotMessage=false;
			time = RakNet::GetTime() + 5000;
			while (RakNet::GetTime() < time)
			{
				packet=rakPeer1->Receive();
				if (packet)
				{
					peer1GotMessage=true;
					printf("Host got: ");
					PrintPacketHeader(packet);
					rakPeer1->DeallocatePacket(packet);
				}
				packet=rakPeer2->Receive();
				if (packet)
				{
					peer2GotMessage=true;
					printf("Connecting system got: ");
					PrintPacketHeader(packet);
					rakPeer2->DeallocatePacket(packet);
				}
#ifdef WIN32
				Sleep(30);
#else
				usleep(30*1000);
#endif
			}

			if (peer1GotMessage==false)
				printf("Error, host got no packets.\n");
			if (peer2GotMessage==false)
				printf("Error, connecting system got no packets.\n");

			if (peer1GotMessage && peer2GotMessage)
				printf("Test successful as long as you got no error messages.\n");
			rakPeer2->Shutdown(0);
			rakPeer1->Shutdown(0);
			PrintOptions();
		}
		else if (str[0]=='h' || str[0]=='H')
		{
			PrintOptions();
		}
		else if (str[0]=='q' || str[0]=='Q')
			break;

		str[0]=0;
	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer1);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer2);
}
