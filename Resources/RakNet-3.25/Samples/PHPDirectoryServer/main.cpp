/// \file
/// \brief This file is a sample for using HTTPConnection and PHPDirectoryServer
///
/// This file is part of RakNet Copyright 2008 Kevin Jenkins.
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
#include "HTTPConnection.h"
#include "PHPDirectoryServer.h"
#include "RakSleep.h"
#include "RakString.h"
#include "GetTime.h"
#include "DS_Table.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace RakNet;

void OutputBody(HTTPConnection& http, const char *path, const char *data, TCPInterface& tcp);

int main(void)
{
	printf("PHP Directory server.\n");
	printf("Similar to lightweight database, but uses common shared webservers.\n");
	printf("Set columns and one row for your game, and upload it to a\nviewable and downloadable webpage.\n");
	printf("Difficulty: Intermediate\n\n");

	RakNetTime lastTouched = 0;

    // Start the TCP thread. This is used for general TCP communication, whether it is for webpages, sending emails, or telnet
    TCPInterface tcp;
    tcp.Start(0, 64);

	// This creates an HTTP connection using TCPInterface. It allows you to Post messages to and parse messages from webservers.
	// The connection attempt is asynchronous, and is handled automatically as HTTPConnection::Update() is called
    HTTPConnection httpConnection(tcp, "jenkinssoftware.com");
   
    // This adds specific parsing functionality to HTTPConnection, in order to communicate with DirectoryServer.php
	// See http://www.jenkinssoftware.com/raknet/forum/index.php?topic=1818.0 for why we have currenttimestamp
    PHPDirectoryServer phpDirectoryServer(httpConnection, "/raknet/DirectoryServer.php?unique=currenttimestamp");

	// Set fields with columnname / value
    phpDirectoryServer.SetField("beehive","inthewater");
   
	// Upload previously set fields, along with required parameters game name, game port, and password
	phpDirectoryServer.UploadTable(50, "Game name", 1234, "");


	for (int j=0; j < 5; j++)
	{

		// Download uploaded servers
		phpDirectoryServer.DownloadTable("");

		for(int i=0; i < 333; ++i)
		{
			Packet *packet = tcp.Receive();
			if(packet)
			{
				// In this sample this line is not necessary, but if we were using TCPInterface for other things, we want to make sure we only give it messages intended for this connection
				if (packet->systemAddress==httpConnection.GetServerAddress())
				{
					// Multiple packets may come in for a single reply from a webserver. When the final packet arrives, ProcessFinalTCPPacket will return true
					if (httpConnection.ProcessFinalTCPPacket(packet))
					{
						int code;
						RakNet::RakString data;
						/// Check that the request was handled and is not an error code
						if (httpConnection.HasBadResponse(&code, &data))
						{
							/// The TCP server responded with some bad response code, such as couldn't find the webpage
							printf("Bad HTTP response code %i: %s", code, data.C_String());
						}
						else
						{
							// Good response, let the PHPDirectoryServer class handle the data
							// If resultCode is not an empty string, then we got something other than a table
							// (such as delete row success notification, or the message is for HTTP only and not for this class).
							RakNet::RakString resultCode = phpDirectoryServer.ProcessHTTPRead(httpConnection.Read());
							if (resultCode.IsEmpty())
							{
								/// Got a table which was stored internally. Print it out
								char out[10000];
								const DataStructures::Table *games = phpDirectoryServer.GetLastDownloadedTable();
								games->PrintColumnHeaders(out,sizeof(out),',');
								printf("COLUMNS: %s\n", out);

								// print each row of the table
								for (unsigned i=0; i < games->GetRowCount(); i++)
								{
									games->PrintRow(out,sizeof(out),',',true, games->GetRowByIndex(i,NULL));
									printf("ROW %i: %s\n", i+1, out);
								}
							}
							else
							{
								/// Some other result, could be an error, could be a general HTTP message not used by this class
								printf("processResult: %s\n", resultCode.C_String());
							}
						}
					}
					else
					{
						//			printf("---------------STILL READING\n%s\n---------------STILL READING\n", packet->data);
					}
				}
				// else this packet was for some other connection using the same instance of tcp

				// Deallocate the packet the same way you do with RakPeerInterface
				tcp.DeallocatePacket(packet);
			}
			// Update our two classes so they can do time-based updates
			httpConnection.Update();
			phpDirectoryServer.Update();

			// Prevent 100% cpu usage
			RakSleep(30);
		}

	}

    return 0;
}

void OutputBody(HTTPConnection& http, const char *path, const char *data, TCPInterface &tcp)
{
    http.Post(path,data);

    while(1)
    {
        Packet *packet = tcp.Receive();
		http.Update();
        
        bool fin=false;
		if(packet)
		{
			fin = http.ProcessFinalTCPPacket(packet);
            tcp.DeallocatePacket(packet);
		}
        
        if(fin)
        {
            RakString data = http.Read();
            printf("Server said: %.100s\n\n", data.C_String());
            break;
        }


        RakSleep(30);
    }
}
