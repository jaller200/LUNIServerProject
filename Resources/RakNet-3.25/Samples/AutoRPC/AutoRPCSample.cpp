#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include <stdlib.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "StringCompressor.h"
#include "RakSleep.h"
#include "NetworkIDObject.h"
#include "AutoRPC.h"
#include "NetworkIDManager.h"
#include "GetTime.h"

struct ThreeBytes
{
	char bytes[3];
};
struct TwentyBytes
{
	char bytes[20];
};

using namespace RakNet;

AutoRPC autoRpc;

void AUTO_RPC_CALLSPEC cFunc0(AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc0");
	printf("cFunc0: sender=%s\n", networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc1(int a, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc1",a);
	printf("cFunc1: %i, sender=%s\n", a, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc2(char a, short b, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc2",a,b);
	printf("cFunc2: %i, %i, sender=%s\n", a, b, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc3(char a, short b, int c, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc3",a,b,c);
	printf("cFunc3: %i, %i, %i, sender=%s\n", a, b, c, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc4(char a, short b, int c, long d, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc4",a,b,c,d);
	printf("cFunc4: %i, %i, %i, %i, sender=%s\n", a, b, c, d, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc5(char a, short b, int c, long d, unsigned long long e, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc5",a,b,c,d,e);
	printf("cFunc5: %i, %i, %i, %i, %i, sender=%s\n", a, b, c, d, (unsigned long) e, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc6(char a, short b, int c, long d, unsigned long long e, ThreeBytes* f, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc6",a,b,c,d,e,f);
	printf("cFunc6: %i, %i, %i, %i, %i, %i, sender=%s\n", a, b, c, d, (unsigned long) e, f->bytes[0], networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc7(char a, short b, int c, long d, unsigned long long e, ThreeBytes* f, TwentyBytes g, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc7",a,b,c,d,e,f,g);
	printf("cFunc7: %i, %i, %i, %i, %i, %i, %i, sender=%s\n", a, b, c, d, (unsigned long) e, f->bytes[0], g.bytes[0], networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}
void AUTO_RPC_CALLSPEC cFunc8(char a, short b, int c, long d, unsigned long long e, ThreeBytes* f, TwentyBytes g, SystemAddress i, AutoRPC* networkCaller)
{
	if (networkCaller==0)
		autoRpc.Call("cFunc8",a,b,c,d,e,f,g,i);
	printf("cFunc8: %i, %i, %i, %i, %i, %i, %i, %s, sender=%s\n", a, b, c, d, (unsigned long) e, f->bytes[0], g.bytes[0], i.ToString(), networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
}

class BaseClass
{
public:
	virtual int AUTO_RPC_CALLSPEC objectMemberFunc(int a, const char *str, AutoRPC* networkCaller) {printf("Shouldn't see this"); return 0;}
};

class MultipleInheritance
{
public:
	MultipleInheritance() {a=1234;}
	int a;
};

class DerivedClass : public MultipleInheritance, public BaseClass, public NetworkIDObject
{
public:
	DerivedClass() {memberVariable=12;}
	~DerivedClass() {}

	virtual int AUTO_RPC_CALLSPEC objectMemberFunc(int a, const char *str, AutoRPC* networkCaller) {
		if (networkCaller==0)
		{
			autoRpc.SetRecipientObject(GetNetworkID());
			autoRpc.Call("DerivedClass::objectMemberFunc",a, str);
			autoRpc.SetRecipientObject(UNASSIGNED_NETWORK_ID);
		}
		else
		{
			printf("%s\n", str);
		}
		printf("object member a=%i memberVariable=%i sender=%s\n", a, memberVariable, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
		return 0;
	}

	int memberVariable;
};

int main(void)
{
	RakPeerInterface *rakPeer;
	unsigned i;
	ThreeBytes threeBytes;
	TwentyBytes twentyBytes;
	for (i=0; i < sizeof(threeBytes); i++)
		threeBytes.bytes[i]=i;
	for (i=0; i < sizeof(twentyBytes); i++)
		twentyBytes.bytes[i]=i;
	SystemAddress tempAddr = UNASSIGNED_SYSTEM_ADDRESS;
	
	RakNetTime stage2=0;
	NetworkIDManager networkIDManager;
	networkIDManager.SetIsNetworkIDAuthority(true);
	autoRpc.SetNetworkIDManager(&networkIDManager);
	DerivedClass derivedClass;
	NetworkID idZero;
	idZero.localSystemAddress=0;
	derivedClass.SetNetworkIDManager(&networkIDManager);
	derivedClass.SetNetworkID(idZero);

	printf("----------------\n");
	printf("DEPRECIATED: Use DependentExtensions\\RPC3 \n");
	printf("----------------\n");
	printf("\n");
	printf("Demonstration of the AutoRPC plugin.\n");
	printf("It is similar to Raknet's RPC system, but automatically\n");
	printf("serializes and deserializes the parameters to the function call\n");
	printf("On Linux compile with -Wno-pmf-conversions.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("(S)erver or (C)lient?: ");
	bool isServer;
	char str[256];
	gets(str);
	if (str[0]=='s' || str[0]=='S')
		isServer=true;
	else
		isServer=false;

	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	if (isServer)
	{
		SocketDescriptor socketDescriptor(50000,0);
		rakPeer->Startup(10, 30, &socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(10);
		printf("Server started.\n");
	}
	else
	{
		SocketDescriptor socketDescriptor(0,0);
		rakPeer->Startup(1, 30, &socketDescriptor, 1);

		// Specifying the last parameter (the number of parameters), is not strictly necessary for C functions, but it allows the system to validate the number of parameters
		// passed to Call() is correct, so it's a good idea. If incorrect, you will get back the packet 
		autoRpc.RegisterFunction("cFunc0", (void*) cFunc0, false, 0);
		autoRpc.RegisterFunction("cFunc1", (void*) cFunc1, false, 1);
		autoRpc.RegisterFunction("cFunc2", (void*) cFunc2, false, 2);
		autoRpc.RegisterFunction("cFunc3", (void*) cFunc3, false, 3);
		autoRpc.RegisterFunction("cFunc4", (void*) cFunc4, false, 4);
		autoRpc.RegisterFunction("cFunc5", (void*) cFunc5, false, 5);
		autoRpc.RegisterFunction("cFunc6", (void*) cFunc6, false, 6);
		autoRpc.RegisterFunction("cFunc7", (void*) cFunc7, false, 7);
		autoRpc.RegisterFunction("cFunc8", (void*) cFunc8, false, 8);

		ARPC_REGISTER_CPP_FUNCTION3(&autoRpc, "DerivedClass::objectMemberFunc", int, BaseClass, objectMemberFunc, int a, const char *str, AutoRPC* networkCaller);

		// Send out a LAN broadcast to find other instances on the same computer
		rakPeer->Ping( "255.255.255.255", 50000, true, 0 );

		printf("Client started. Will automatically connect to running servers.\n");

		rakPeer->Connect("yaesu.chickenware.com", 50000,0,0,0);
	}
	rakPeer->AttachPlugin(&autoRpc);
	
	Packet *p;
	while (1)
	{
		for (p=rakPeer->Receive(); p; rakPeer->DeallocatePacket(p), p=rakPeer->Receive())
		{
			switch (p->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_PONG:
				// Found the server
				rakPeer->Connect(p->systemAddress.ToString(false),p->systemAddress.port,0,0,0);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				// Directed send, default is broadcast
			//	autoRpc.SetRecipientAddress(p->systemAddress,false);
				// A new client connected, call all the functions, which should be mirrored to all clients
				cFunc0(0);
				cFunc1(true,0);
				cFunc2(1,2,0);
				cFunc3(1,2,3,0);
				cFunc4(1,2,3,4,0);
				cFunc5(1,2,3,4,5,0);
				cFunc6(1,2,3,4,5,&threeBytes,0);
				cFunc7(1,2,3,4,5,&threeBytes,twentyBytes,0);
				cFunc8(1,2,3,4,5,&threeBytes,twentyBytes,tempAddr,0);
				derivedClass.objectMemberFunc(999, "Hello world 1", 0);
				stage2=RakNet::GetTime()+500;
				break;
			case ID_RPC_REMOTE_ERROR:
				{
					// Receipient system returned an error
					switch (p->data[1])
					{
					case RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
						printf("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n");
						break;
					case RPC_ERROR_OBJECT_DOES_NOT_EXIST:
						printf("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n");
						break;
					case RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
						printf("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n");
						break;
					case RPC_ERROR_FUNCTION_NOT_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NOT_REGISTERED\n");
						break;
					case RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n");
						break;
					case RPC_ERROR_CALLING_CPP_AS_C:
						printf("RPC_ERROR_CALLING_CPP_AS_C\n");
						break;
					case RPC_ERROR_CALLING_C_AS_CPP:
						printf("RPC_ERROR_CALLING_C_AS_CPP\n");
						break;
					case RPC_ERROR_STACK_TOO_SMALL:
						printf("RPC_ERROR_STACK_TOO_SMALL\n");
						break;
					case RPC_ERROR_STACK_DESERIALIZATION_FAILED:
						printf("RPC_ERROR_STACK_DESERIALIZATION_FAILED\n");
						break;
					case RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS:
						printf("RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS\n");
						break;
					}

					char outputBuff[256];
					stringCompressor->DecodeString(outputBuff,256,&RakNet::BitStream(p->data+sizeof(MessageID)+1,p->length-sizeof(MessageID)-1,false),0);
					printf("Function: %s\n", outputBuff);
				}
			}
		}

		if (stage2 && stage2 < RakNet::GetTime())
		{
			stage2=0;

			// Call all functions again, ensuring that the function ID lookup works
			cFunc0(0);
			cFunc1(true,0);
			cFunc2(1,2,0);
			cFunc3(1,2,3,0);
			cFunc4(1,2,3,4,0);
			cFunc5(1,2,3,4,5,0);
			cFunc6(1,2,3,4,5,&threeBytes,0);
			cFunc7(1,2,3,4,5,&threeBytes,twentyBytes,0);
			cFunc8(1,2,3,4,5,&threeBytes,twentyBytes,tempAddr,0);
			derivedClass.objectMemberFunc(999, "Hello world 2", 0);			
		}

		RakSleep(0);
	}
	
	rakPeer->Shutdown(100,0);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 1;
}
