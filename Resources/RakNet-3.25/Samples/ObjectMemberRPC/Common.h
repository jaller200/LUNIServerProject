#ifndef __COMMON_H
#define __COMMON_H

#include "NetworkIDObject.h"
#include "MessageIdentifiers.h"
#include "PacketLogger.h"
#include "BitStream.h"

#define SERVER_MAXCLIENTS 4
#define DOLOG 1


// Define our custom message ID's
enum {
	ID_USER_SERVERAPPLE_CREATED = ID_USER_PACKET_ENUM+1
};



#if (defined(__GNUC__)  || defined(__GCCXML__))
#define __cdecl
#endif


// Create a logger that knows about our own message IDs
// That way we can see what's happening
class CustomPacketLogger : public PacketLogger
{
public:
	CustomPacketLogger(){
	}

	const char* UserIDTOString(unsigned char id){
		switch (id){
			case ID_USER_SERVERAPPLE_CREATED : return "ID_USER_SERVERAPPLE_CREATED"; break;
			default: return "Unknown user message ID";
		}
	}

};

// Classes that use class member RPCs must derive from NetworkIDObject
class Apple : public NetworkIDObject
{
public:

	// RPC member Functions MUST be marked __cdecl!
	virtual void __cdecl func1(RPCParameters *rpcParms)
	{
		if (rpcParms->input)
			printf("\nBase Apple func1: %s\n", rpcParms->input);
		else
			printf("\nBase Apple func1\n");
	}

	// RPC member Functions MUST be marked __cdecl!
	virtual void __cdecl func1(char *blah)
	{
		printf("\nFunc1.  Does not match function signature and should never be called.\n");
	}

	// RPC member Functions MUST be marked __cdecl!
	virtual void __cdecl func2(RPCParameters *rpcParms)
	{
		if (rpcParms->input)
			printf("\nBase Apple func2: %s\n", rpcParms->input);
		else
			printf("\nBase Apple func2\n");
	}

	// RPC member Functions MUST be marked __cdecl!
	virtual void __cdecl func3(RPCParameters *rpcParms)
	{
		if (rpcParms->input)
			printf("\nBase Apple func3: %s\n", rpcParms->input);
		else
			printf("\nBase Apple func3\n");
	}
};

class GrannySmith : public Apple
{
public:
	
	void __cdecl func1(RPCParameters *rpcParms)
	{
		printf("\nDerived GrannySmith func1: %s\n", rpcParms->input);
	}
};


#endif