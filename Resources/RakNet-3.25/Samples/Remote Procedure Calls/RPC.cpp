#include "RakPeer.h"
#include "RakPeer.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
using namespace RakNet;

RakPeerInterface *rakClient;
RakPeerInterface *rakServer;
bool quit;

void serverRPC(RPCParameters *rpcParameters)
{
	if (rpcParameters->input)
		printf("In serverRPC Message = %s\n", rpcParameters->input);
	else
		printf("In serverRPC no input");

	rakServer->RPC("clientRPC", "From serverRPC", (int) (strlen("From serverRPC")+1)*8, HIGH_PRIORITY, RELIABLE, 0, rpcParameters->sender, false, 0, UNASSIGNED_NETWORK_ID, 0);
	RakSleep(1000);
	rpcParameters->replyToSender->Write(true);
//	quit=true;
}

void clientRPC(RPCParameters *rpcParameters)
{
	if (rpcParameters->input)
		printf("In clientRPC Message = %s\n", rpcParameters->input);
	else
		printf("In clientRPC no input");
//	quit=true;
}

int main(void)
{
	bool isServer;
	//int state=1;
	char str[256];
	//RakNetTime nextActionTime;

	printf("A sample of how to use remote procedure calls.\n");
	printf("This only demonstrates calling C style functions. See 'ObjectMemberRPC'\n");
	printf("to call class instance member functions.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Act as (s)erver or (c)lient?\n");
	gets(str);
	if (str[0]==0)
		return 1;

	if (str[0]=='s' || str[0]=='S')
	{
		isServer=true;
	}
	else
	{
		isServer=false;
	}

	if (isServer)
	{
		rakServer=RakNetworkFactory::GetRakPeerInterface();
		REGISTER_STATIC_RPC(rakServer, serverRPC);
		SocketDescriptor socketDescriptor(2000,0);
		rakServer->Startup(32,30,&socketDescriptor, 1);
		rakServer->SetMaximumIncomingConnections(32);
	}
	else
	{
		rakClient=RakNetworkFactory::GetRakPeerInterface();

		// You can register a function anytime
		// Note that although you are passing a function pointer, the name you are passing matters.  The macro uses
		// this name as a unique identifier.  Don't do *f = clientRPC and then register f
		REGISTER_STATIC_RPC(rakClient, clientRPC);
		SocketDescriptor socketDescriptor(0,0);
		rakClient->Startup(1,30,&socketDescriptor, 1);
		rakClient->Connect("127.0.0.1", 2000, 0, 0);
	}	
		

	printf("RPC demo\n");

	bool sent=false;
	quit=false;
	while (!quit)
	{
		if (isServer)
			rakServer->DeallocatePacket(rakServer->Receive());
		if (isServer==false)
			rakClient->DeallocatePacket(rakClient->Receive());

		if (isServer==false && sent==false)
		{
			// This shows the first form of RPC, which is just sending a char* and the number of bits
			// used.
			char str[200];
			printf("Enter any text string to show on the server:\n");
			gets(str);
			// Don't forget that RPC takes bits as the data length, not bytes!
			RakNet::BitStream reply;
			rakClient->RPC("serverRPC",str, (int)((strlen(str)+1)*8), HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID,&reply);
			quit=true;
		}
	}


	printf("Example complete.  Press enter to quit\n");
	gets(str);

	if (isServer)
	{	rakServer->Shutdown(0);

		UNREGISTER_STATIC_RPC(rakServer, serverRPC);
		RakNetworkFactory::DestroyRakPeerInterface(rakServer);
	}
	else
	{
		rakClient->Shutdown(0);

		// This is not necessary since on shutdown everything is unregistered.  This is just here to show usage
		UNREGISTER_STATIC_RPC(rakClient, clientRPC);
		RakNetworkFactory::DestroyRakPeerInterface(rakClient);
	}
	

	return 0;
}

