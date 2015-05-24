#include "serverLoop.h"

#include "Database.h"
#include "User.h"
#include "legoPackets.h"
#include "AuthPackets.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"

using namespace RakNet;

Ref< User > HandleUserLogin(RakPeerInterface* rakServer, Packet* packet, CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers) {
	if (packet->data[3] == LOGIN_REQUEST) {	// thats just a formality so far since no other numbers occured for this case
		std::string username, password;

		// Get the username of the user that logged in...
		if (packet->length > 0x8) {	// This is where username always starts
			int i = 0;
			while (packet->data[0x8 + i] != '\0' && packet->length > (uint)(0x8 + i)) {
				username.push_back(packet->data[8 + i]);
				i += 2;
			}
		}

		// Get the password of the user the logged in...
		if (packet->length > 0x4a) {	// This is where password starts

			int i = 0;
			while (packet->data[0x4a + i] != '\0' && packet->length > (uint)(0x4a + i)) {
				password.push_back(packet->data[0x4a + i]);
				i += 2;
			}
		}

		// Login the user to the server
		auto usr = User::Login(username, password, packet->systemAddress);

		// Send the character success packet (while making sure user is not null)
		if (usr == NULL) {
			cout << "USER IS NULL!!!!";

			UserSuccess successChar = UserSuccess::INVALID_USER;
			SendStatusPacket(rakServer, packet->systemAddress, successChar, cfg->redirectIp, cfg->redirectPort, 324);
		} else {
			UserSuccess successChar = (UserSuccess)usr->successState;
			SendStatusPacket(rakServer, packet->systemAddress, successChar, cfg->redirectIp, cfg->redirectPort, 324);

			if (successChar == UserSuccess::SUCCESS) {
				// If the login was a success, insert the user into the UsersPool
				OnlineUsers->Insert(usr, packet->systemAddress);
			}

			return usr;
		}
	}
	return NULL;
}

void AuthLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< std::string > > OutputQueue) {
	// Initialize the RakPeerInterface used throughout the entire server
	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();

	// Initialize the PacketFileLogger plugin (for the logs)
	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		rakServer->AttachPlugin(msgFileHandler);
	}

	// Initialize security IF user has enabled it in config.ini
	InitSecurity(rakServer, cfg->useEncryption);

	// Initialize the SocketDescriptor
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);

	// If the startup of the server is successful, print it to the console
	// Otherwise, print an error
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		std::stringstream s;
		s << "auth started! Listening on: " << cfg->listenPort << "\n";
		OutputQueue->Insert(s.str());
	} else QuitError("auth server init error!");

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is initalized, use it to log the server in ./logs/auth
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\auth");

	// Initialize the Packet class
	Packet* packet;

	// LUNIterminate is the bool used to terminate threads.
	// While it is false, the thread runs, but if it is true, the thread exits
	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Get the packets from the client
		if (packet == NULL) continue; // If the packet is null, just continue without processing anything
		PrintPacketInfo(packet, msgFileHandler); // Print the packet information (if packet is not NULL)

		// Figure out which packet it is...
		switch (packet->data[0]) {
			case ID_LEGO_PACKET:

				switch (packet->data[1]) {
					case GENERAL:
						if (packet->data[3] == 0) {
							// Send the Init packet to the client
							SendInitPacket(rakServer, packet->systemAddress, true);
						}
						break;

					case AUTH: //user logging into server
						{
							// Handle the user login using the above method
							auto usr = HandleUserLogin(rakServer, packet, cfg, OnlineUsers);

							// If username is not null, print the username that logged in.
							// Otherwise, print the error message
							if (usr != NULL) {
								OutputQueue->Insert("\n" + usr->GetUsername() + " Logged-in\n");
							}
							else OutputQueue->Insert("\n Login failed!\n");
						}
						break;

					// The default if the packet the server is recieving has an unidentified ID
					default:
						std::stringstream s;
						s << "\nAuth received unknown packet: " << RawDataToString(packet->data, packet->length) << std::endl;
						OutputQueue->Insert(s.str());
				}

				break;

			// If the server is recieving a new connection, print it.
			case ID_NEW_INCOMING_CONNECTION:
#			ifdef DEBUG
				OutputQueue->Insert("\n Auth is receiving a new connection...\n");
			#endif
				break;

			// If someone is disconnecting from the auth server, print it
			case ID_DISCONNECTION_NOTIFICATION:
				OutputQueue->Insert("\n User disconnected from Auth server... \n");
				break;
				
			// If the packet has an unidentified RakNet ID (one not listed here), print the
			// packet info
			default:
				std::stringstream s;
				s << "\nAuth received unknown packet: " << RawDataToString(packet->data, packet->length) << std::endl;
				OutputQueue->Insert(s.str());
		}

		// Deallocate the packet to conserve memory
		rakServer->DeallocatePacket(packet);
	}

	// If LUNIServer Auth has been terminated, print it
	std::stringstream s;
	s << "Quitting auth\n";
	OutputQueue->Insert(s.str());

	// Shut down Auth server
	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}
