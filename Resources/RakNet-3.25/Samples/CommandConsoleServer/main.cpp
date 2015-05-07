/// \file
/// \brief Test the command console implementations
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "TCPInterface.h"
#include "ConsoleServer.h"
#include "RakNetCommandParser.h"
#include "TelnetTransport.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "LogCommandParser.h"
#include "GetTime.h"
#include "RakNetTransport.h"
#include "LinuxStrings.h"

#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h"
#define printf(x,y) 
#elif defined(_WIN32)
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#include <stdio.h>
#endif


void TestTCPInterface(void);
void TestCommandServer(TransportInterface *ti, unsigned short port);

int main(void)
{
	TelnetTransport tt;
	// RakNetTransport rt; // Raknet defaults to port whatever you want
	TestCommandServer(&tt, 23); // Uncomment to use Telnet as a client.  Telnet uses port 23 by default.
	// TestCommandServer(&rt, 60000); // Uncomment to use RakNet as a client
	return 1;
}

void TestCommandServer(TransportInterface *ti, unsigned short port)
{
    ConsoleServer consoleServer;
	RakNetCommandParser rcp;
	LogCommandParser lcp;
	RakNetTime lastLog=0;
	RakPeerInterface *rakPeer = RakNetworkFactory::GetRakPeerInterface();

	printf("This sample demonstrates the command console server, which can be.\n");
	printf("a standalone application or part of your game server.  It allows you to\n");
	printf("easily parse text strings sent from a client using either secure RakNet\n");
	printf("or Telnet.\n");
	printf("See the 'CommandConsoleClient' project for the RakNet client.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Command server started on port %i.\n", port);
	consoleServer.AddCommandParser(&rcp);
	consoleServer.AddCommandParser(&lcp);
	consoleServer.SetTransportProvider(ti, port);
	rcp.SetRakPeerInterface(rakPeer);
	lcp.AddChannel("TestChannel");
	while (1)
	{
		consoleServer.Update();

		if (RakNet::GetTime() > lastLog + 4000)
		{
			lcp.WriteLog("TestChannel", "Test of logger");
			lastLog=RakNet::GetTime();
		}

#ifdef _WIN32
		Sleep( 30 );
#else
		usleep( 30 * 1000 );
#endif
	}	
}