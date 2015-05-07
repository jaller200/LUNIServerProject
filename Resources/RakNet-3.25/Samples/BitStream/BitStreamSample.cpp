// ----------------------------------------------------------------------
// RakNet version 1.411
// BitStreamSample.cpp
// Created by Rakkar Software (rakkar@jenkinssoftware.com) December 8, 2003
// Shows simple use of the bitstream class
// ----------------------------------------------------------------------

#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include "RakSleep.h"
#include <stdlib.h> // For atoi
#include <cstring> // For strlen
#include <stdio.h>
#include <string.h>
#include "Kbhit.h"

#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h" // Developers of a certain platform will know what to do here.
#elif defined(_PS3)
#include "Console2SampleIncludes.h"
#elif defined(_WIN32)
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

bool quit;

using namespace RakNet;

// We want to send information about a person and their job.
// We are interested in their name, age, salary, and years employed.
// For this sample we encode the data as follows:
// 1. Number of characters of the person's name
// 2. Person's name (not null terminated)
// 3. Person's age (compressed unsigned char)
// 4. The EmploymentStruct (below)
struct EmploymentStruct
{
	int salary;
	unsigned char yearsEmployed;
};

// You can parse the input data in two ways.
// Either cast input to a struct (such as if you sent a struct)
// Or create a BitStream instance with input as data such as
// BitStream myBitStream(input, (numberOfBitsOfData-1)/8+1);
// where (numberOfBitsOfData-1)/8+1 is the number of bytes of data
// If you didn't pass any data then of course you don't do either
// Here we pass a bitstream
void clientRPC(RPCParameters *rpcParameters)
{
	// Since we were passed a bitstream, to make things easier to parse convert the input into into a BitStream.
	// We can either do this in the constructor which takes the data and the number of BYTES
	// BitStream b(input, (numberOfBitsOfData-1)/8+1);
	// or we can do it by writing out the bits
	// BitStream b;
	// b.WriteBits(input, numberOfBitsOfData);

	// Here I will do the constructor version because it is easier.  numberOfBitsOfData is always > 0
	// The third parameter of false means don't internally copy input but just maintain a pointer to it.  This is fine because we never change it
	BitStream b(rpcParameters->input, BITS_TO_BYTES(rpcParameters->numberOfBitsOfData), false);
	char name[200];

//	printf("GOT RPC:\n");
//	b.PrintBits();

	// The length of the person's name.  It is critical to read the same types in the same order using the same level of compression as we wrote them
	unsigned char nameLength;
	b.Read(nameLength);
	if (b.Read(name, nameLength)==false) // Name is not null terminated
		// Note we always check if the read functions return false.  This is important in a real example in case hackers or programmer error causes too-short bitstreams
		// You would probably also log this event or throw an assert in a real game
	{
		printf("Name was not null-terminated!\n");
		return;
	}

	name[nameLength]=0; // Name is now null terminated

	printf("In clientRPC:\n");
	printf("Name is %s\n", name);

	unsigned int age;
	// We used WriteCompressed for the age, so have to use ReadCompressed to get it
	if (b.ReadCompressed(age)==false)
		return;

	printf("Age is %i\n", age);
	fflush(stdout);

	bool wroteEmploymentStruct;
	if (b.Read(wroteEmploymentStruct)==false)
	{
		return;
	}

	if (wroteEmploymentStruct)
	{
		printf("We are employed.\n");

		EmploymentStruct employmentStruct;
		// Reading a struct consists of deserializing the bitstream:
		if (b.Read(employmentStruct.salary)==false) return;
		if (b.Read(employmentStruct.yearsEmployed)==false) return;

		printf("Salary is %i.  Years employed is %i\n", employmentStruct.salary, (int)employmentStruct.yearsEmployed);
	}
	else
		printf("We are between jobs :)\n");

	quit=true;
}

#ifdef _PS3
_PS3_SetSystemProcessParams
#endif

int main(void)
{
	RakPeerInterface *rakClient=RakNetworkFactory::GetRakPeerInterface();
	RakPeerInterface *rakServer=RakNetworkFactory::GetRakPeerInterface();
#ifndef WIN32
#define getch getchar
#endif

#ifdef _PS3
	PS3LoadModules();
#endif
	quit=false;
	char text[255];

	// Defined in RakNetTypes.h.
	// You can register a function anytime
	REGISTER_STATIC_RPC(rakClient, clientRPC);

	//rakServer->InitializeSecurity(0,0,0,0);

	SocketDescriptor socketDescriptor(10000,0);
	if (rakServer->Startup(1,30,&socketDescriptor, 1)==false)
	{
		printf("Start call failed!\n");

		fflush(stdout);
		fgets(text, sizeof(text), stdin);
		printf("\n");
		return 0;
	}
	rakServer->SetMaximumIncomingConnections(1);
	socketDescriptor.port=0;
	rakClient->Startup(1, 30, &socketDescriptor, 1);
	if (rakClient->Connect("127.0.0.1", 10000, 0, 0)==false)
	{
		printf("Connect call failed\n");
		fflush(stdout);
		fgets(text, sizeof(text), stdin);
		printf("\n");
		return 0;
	}

	BitStream outgoingBitstream;
	unsigned int age;

	printf("A sample on how to use RakNet's bitstream class\n");
	printf("Difficulty: Beginner\n\n");

	printf("Enter your name.\n");
	fflush(stdout);
	fgets(text, sizeof(text), stdin);
	printf("\n");
	if (text[0]==0)
		strcpy(text, "Unnamed!");
	// Write the number of characters of the name into the bitstream
	// We put the unsigned char cast to use the overload that writes 1 byte.
	outgoingBitstream.Write((unsigned char)strlen(text));

	// Now write the name to the bitstream.  This overload takes a char* and the number of bytes to write
	outgoingBitstream.Write(text, (int) strlen(text));

	printf("Enter your age (numbers only).\n");
	fflush(stdout);
	fgets(text, sizeof(text), stdin);
	printf("\n");
	if (text[0]==0)
		age=0;
	else
		age=atoi(text);

	// Write age to the bitstream.  Since the range of age is
	// probably pretty low compared to the range of the variable (uint),
	// we use a compressed write.  This can reduce the number
	// of bits used.
	outgoingBitstream.WriteCompressed(age);

	// Now demonstrate dynamic packets by choosing one of two paths in which to write different amounts / types of data.
	printf("Are you employed (y/n)?\n");
	fflush(stdout);
	fgets(text, sizeof(text), stdin);
	printf("\n");
	if (text[0]=='y')
	{
		outgoingBitstream.Write(true); // Writing a bool takes 1 bit

		// Read some data into a struct
		EmploymentStruct employmentStruct;
		printf("What is your salary (enter a number only)?\n");
		fflush(stdout);
		fgets(text, sizeof(text), stdin);
		printf("\n");
		employmentStruct.salary = atoi(text);
		printf("How many years have you been employed (enter a number only)?\n");
		fflush(stdout);
		fgets(text, sizeof(text), stdin);
		printf("\n");
		employmentStruct.yearsEmployed = atoi(text);

		// We can write structs to a bitstream but this is not portable due to:
		//  1. Different-endian CPUs
		//  2. Different 'padding' of structs depending on compiler, etc
		// The only safe way to send a struct is by using the BitStream
		// to write out every single member which you want to send.
		outgoingBitstream.Write(employmentStruct.salary);
		outgoingBitstream.Write(employmentStruct.yearsEmployed);
		// We're done writing to the struct
	}
	else
	{
		//printf("Number of bits before [false]: %d\n",
		//outgoingBitstream.GetNumberOfBitsUsed() );
		outgoingBitstream.Write(false); // Writing a bool takes 1 bit
		// We're done writing to the struct.  Compare this to the example above - we wrote quite a bit less.
	}

	printf("Waiting for connection...\n");
	while (rakClient->GetSystemAddressFromIndex(0)==UNASSIGNED_SYSTEM_ADDRESS)
		RakSleep(30);
	printf("Connected.\n");

//	printf("SEND RPC:\n");
//	outgoingBitstream.PrintBits();

	// RPC functions as well as send can take bitstreams directly
	bool success = rakServer->RPC("clientRPC",&outgoingBitstream, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true, 0, UNASSIGNED_NETWORK_ID, 0); // broadcast to everyone, which happens to be our one client
	if (!success)
		printf("RPC call failed\n");

	while (!quit)
	{
		rakClient->DeallocatePacket(rakClient->Receive());
		rakServer->DeallocatePacket(rakServer->Receive());

		RakSleep(30);
	}
	printf("Press enter to quit\n");
	fflush(stdout);
	fgets(text, sizeof(text), stdin);
	printf("\n");

	rakClient->Shutdown(100,0);
	rakServer->Shutdown(100,0);

	// This is not necessary since on shutdown everything is unregistered.  This is just here to show usage
	UNREGISTER_STATIC_RPC(rakClient, clientRPC);

	RakNetworkFactory::DestroyRakPeerInterface(rakClient);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);

	return 0;
}
