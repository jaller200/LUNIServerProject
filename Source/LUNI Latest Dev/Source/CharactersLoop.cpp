#include "serverLoop.h"

#include "legoPackets.h"
#include "User.h"
#include "CharPackets.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"

using namespace std;

void CharactersLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
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
	// Otherwise, quit the server (as the char server is REQUIRED for the
	// server to function properly)
	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		stringstream s;
		s << "Characters started! Listening on: " << cfg->listenPort << "\n";
		OutputQueue->Insert(s.str());
	} else exit(2);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\char");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below...
	int i = 0;

	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything
		PrintPacketInfo(packet, msgFileHandler); // Save packet info
		
		// This will save all packets recieved from the client if running from DEBUG
		#ifdef DEBUG
			stringstream packetName;
			packetName << ".//Saves//Packet" << i << ".bin";

			SavePacket(packetName.str(), (char *)packet->data, packet->length);
			i++;
		#endif

		switch (packet->data[0]) {
			case ID_LEGO_PACKET:

				switch (packet->data[1]) {
					case GENERAL:
						if (packet->data[3] == VERSION_CONFIRM) {	// thats just a formality so far since no other numbers occured for this case
							SendInitPacket(rakServer, packet->systemAddress, false);
						}
						break;

					case SERVER:

						switch (packet->data[3]) {
							case CLIENT_VALIDATION:
							{
										cout << "Recieved client validation..." << endl;
										break;
							}

							case CLIENT_CHARACTER_LIST_REQUEST:
							{
									  auto usr = OnlineUsers->Find(packet->systemAddress);
									  if (usr->nameInUse == 0) {
										  cout << "Sending char packet...";
										  SendCharPacket(rakServer, packet->systemAddress, usr);
									  }
									  break;
							}

							case CLIENT_CHARACTER_CREATE_REQUEST:
							{
								// Find online user by systemAddress
								auto usr = OnlineUsers->Find(packet->systemAddress);

								// Make SURE user is not null!!!!!!
								if (usr != NULL) {
									AddCharToDatabase(rakServer, packet->systemAddress, packet->data, packet->length, usr);
								}
								else {
									cout << "ERROR SAVING USER: User is null." << endl;
								}
								
								// If the username is in use, do NOT send the char packet. Otherwise, send it
								if (usr->nameInUse == 0) {
									SendCharPacket(rakServer, packet->systemAddress, usr);
								}	
							}
								break;

							case CLIENT_CHARACTER_DELETE_REQUEST:
							{
								// Find online user
								auto usr = OnlineUsers->Find(packet->systemAddress);

								// Send the delete packet
								SendDeletePacket(rakServer, packet->systemAddress, usr, packet->data, packet->length);
								break;
							}

							case CLIENT_LOGIN_REQUEST:
							{
								// Find online user using systemAddress
								auto usr = OnlineUsers->Find(packet->systemAddress);

								// If user exists, continue
								if (usr != NULL) {
									usr->numredir++; // Add to the number of redirects that the character has
									// Get Character ID
									vector< uchar > t;
									for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
									usr->nextcid = *(ulong*)t.data();

									// If DEBUG is on, and user had ID of one of these (It probably won't)
									// Print who the char is logging in with
								#ifdef DEBUG
									stringstream s;
									s << "\nCharacter logging in world with id: " << usr->nextcid;
									if ( usr->nextcid == 2397732190 ) s << " CheekyMonkey!\n";
									else if ( usr->nextcid == 2444680020 ) s << " monkeybrown!\n";
									else if ( usr->nextcid == 1534792735 ) s << " GruntMonkey!\n";
									else if ( usr->nextcid == 1457240027 ) s << " Shafantastic!\n";
									else s << "Unknown Character ID: " << usr->nextcid << endl;
									OutputQueue->Insert(s.str());
								#endif
								}

								// Open the packet to redirect the character to the world server
								auto v = OpenPacket(".\\char\\char_aw_redirect.bin");
								if (v.size() > 0) {
									// IP Address starts at byte 0x08, Port number starts at byte 0x29
									// Copy the redirectId to the packet
									if (cfg->redirectIp[0] != 0)
										memcpy(v.data() + 0x8, cfg->redirectIp, sizeof(cfg->redirectIp) - 3);	// NOTE: the IP can currently only be 13 characters long here since there are some non-zero bytes directly after the IP and I don't want to delete them (is it safe to overwrite them?)
									// Copy the redirectPort to the packet
									if (cfg->redirectPort > 0)
										memcpy(v.data() + 0x29, &cfg->redirectPort, sizeof(cfg->redirectPort));
									ServerSendPacket(rakServer, v, packet->systemAddress);
								}

								// Close connections to the char server for now
								rakServer->CloseConnection(packet->systemAddress, true);
							}
								break;
						}

						break;

					// If packet ID (3rd byte) is unidentified, print the packet data
					default:
						stringstream s;

						// If packet is unidentified, print data to console
						s << "\nCharacters received unknown packet: " << RawDataToString(packet->data, packet->length) << endl;
						OutputQueue->Insert(s.str());
				}

				break;

			// Recieving a new connection to the char server
			case ID_NEW_INCOMING_CONNECTION:
			#ifdef DEBUG
				OutputQueue->Insert("\n Characters is receiving a new connection...\n");
			#endif
				break;

			// User disconnected from the char server
			case ID_DISCONNECTION_NOTIFICATION:
			{
				auto usr = OnlineUsers->Find(packet->systemAddress);
				if (OnlineUsers->Remove(packet->systemAddress))
					OutputQueue->Insert("Disconnected " + usr->GetUsername() + "\n");
			}
				break;

			// Default msg (if packet is unidentified)
			default:
				stringstream s;
				s << "\nCharacters received unknown packet: " << RawDataToString(packet->data, packet->length) << endl;
				OutputQueue->Insert(s.str());
		}
	}
}
