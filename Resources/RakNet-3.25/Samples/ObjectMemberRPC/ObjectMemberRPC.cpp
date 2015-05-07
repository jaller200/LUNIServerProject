#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include "Kbhit.h"

#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "PacketConsoleLogger.h"
#include "GetTime.h"

/*
 Both Server and Client are compiled into this sample, but to better understand the use of RPC,
 consider RunAsServer, and RunAsClient like two separated programs.

 To test, run two instances of the program. Run one as the Server, and then one as the Client.
*/
void RunAsClient(void);
void RunAsServer(void);

void main(void)
{

	printf("S - Run as Server\n");
	printf("C - Run as Client\n");	
	int ch;
	do {
		ch = getch();
	} while(!((ch=='s')||(ch=='S')||(ch=='c')||(ch=='C')));
	
	if ((ch=='s')||(ch=='S')){
		RunAsServer();
	} else {
		RunAsClient();
	}
}