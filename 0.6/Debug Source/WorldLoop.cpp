#pragma once
#include "serverLoop.h"

#include "legoPackets.h"
#include "User.h"
#include "Character.h"
#include "Database.h"
#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"
#include <cstdlib>
using namespace std;

void WorldLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
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
		s << "world started! Listening on: " << cfg->listenPort << "\n";
		OutputQueue->Insert(s.str());
	} else exit(2);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\world");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below
	int i = 0;

	// This will be used in the saving of packets below...
	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything

		// This will save all packets recieved from the client if running from DEBUG
		#ifdef DEBUG
			stringstream packetName;
			packetName << ".//Saves//World_Packet" << i << ".bin";

			SavePacket(packetName.str(), (char*)packet->data, packet->length);
			i++;
		#endif

		// Create and send packets back here according to the one we got
		switch (packet->data[0]) {
			case ID_USER_PACKET_ENUM:
				switch (packet->data[1]) {
					case GENERAL:
						if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
							auto v = OpenPacket(".\\world\\init_aw.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);
						}
						break;

					case SERVER:

						switch (packet->data[3]) {
							case CLIENT_VALIDATION:
							{ 
								auto usr = OnlineUsers->Find(packet->systemAddress);
								//this packets contains ZoneId and something else... but what?
								// it starts whit: 53 05 00 02 00 00 00 00 -- -- (<-ZoneId) ?? ?? ??
								vector< uchar > v;
								if (usr != NULL && usr->nextcid == 2444680020) { //monkeybrown
									v = OpenPacket(".\\world\\monkeybrown\\char_aw2.bin");
								}
								else if (usr != NULL && usr->nextcid == 1534792735) { //gruntmonkey
									v = OpenPacket(".\\world\\gruntmonkey\\char_aw2.bin");
								}
								else if (usr != NULL && usr->nextcid == 1457240027) { //shafantastic
									v = OpenPacket(".\\world\\shastafantastic\\char_aw22.bin");
								}
								else { //cheekymonkey
									v = OpenPacket(".\\world\\char_aw2.bin");
								}

								#ifdef DEBUG
								if (v.size() > 0) {
									RakNet::BitStream bs( CleanPacket(v.data(), v.size()), v.size()-8, false );
									ZoneId zid;
									bs.Read(zid);
									stringstream s;
									s << "\nLoading world: " << zid << endl;
									OutputQueue->Insert(s.str());
								} else OutputQueue->Insert("\nWorld Error: can't load char_aw2.bin\n");
								#endif

								ServerSendPacket(rakServer, v, packet->systemAddress);
							}
								break;

							case CLIENT_LEVEL_LOAD_COMPLETE:
							{
								// Response: 05, 04
								// The answer to this is compressed data detailing character data
								// also since the answer seems to consist of a split packet, does RakNet automatically split the packets if the data is too big? or do we have to do something, if yes what?
								// byte 08 (2 or 4 bytes long? I guess 4...) seems to define the size of the data we send, no idea what the data itself is though...
								auto usr = OnlineUsers->Find(packet->systemAddress);

								// If the user is not equal to null and next character id = below number,
								// Open the packets of monkeybrown and load him into a world
								if (usr != NULL && usr->nextcid == 2444680020) {
									cout << "\n Replying to monkeybrown\n";
									auto v = OpenPacket(".\\world\\monkeybrown\\rocket_1a.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);

									v = OpenPacket(".\\world\\monkeybrown\\rocket_1b.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);

									v = OpenPacket(".\\world\\monkeybrown\\rocket_1c.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);

									v = OpenPacket(".\\world\\monkeybrown\\world_2a.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);
									// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
									v = OpenPacket(".\\world\\monkeybrown\\world_2b.bin");   // I assume these two are sent together?
									ServerSendPacket(rakServer, v, packet->systemAddress);

									for (int i = 0; i < 42; i++) {  // herpaderp
										stringstream fl;
										fl << ".\\world\\monkeybrown\\test\\world_2_" << (i + 1) << ".bin";
										auto v = OpenPacket(fl.str());
										ServerSendPacket(rakServer, v, packet->systemAddress);
									}

									v = OpenPacket(".\\world\\monkeybrown\\test\\world_2_1.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);

									for (int i = 42; i < 210; i++) {  // herpaderp
										stringstream fl;
										fl << ".\\world\\monkeybrown\\test\\world_2_" << (i + 1) << ".bin";
										auto v = OpenPacket(fl.str());
										ServerSendPacket(rakServer, v, packet->systemAddress);
									}
								} else { // Otherwise, reply to Cheekymonkey and open his packets
									cout << "\n Replying to cheekymonkey\n";
									auto v = OpenPacket(".\\world\\world_2a.bin");
									ServerSendPacket(rakServer, v, packet->systemAddress);
									// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
									v = OpenPacket(".\\world\\world_2b.bin");	// I assume these two are sent together?
									ServerSendPacket(rakServer, v, packet->systemAddress);
									for (int i = 0; i < 5; i++) {	// herpaderp
										stringstream fl;
										fl << ".\\world\\test\\world_2_" << (i + 1) << ".bin";
										auto v = OpenPacket(fl.str());
										ServerSendPacket(rakServer, v, packet->systemAddress);
									}
								}
							}
								break;

							case 22: //user moving / update request?
							{
								auto usr = OnlineUsers->Find(packet->systemAddress);
							#ifdef DEBUG
								OutputQueue->Insert("Received packet 22: " + RawDataToString(packet->data, packet->length) + "\n");
							#endif
							}
								break;

							case LUNI_WORLD_CHAT: //user wrote a chat message
							{
								if (packet->length >= 10 && RawDataToString(packet->data + packet->length - 10, 10, true) == "730064006f0077006e00") {//virus used to be here
								}

								// dummy try
								vector< uchar > v(packet->length);
								for (uint i = 0; i < packet->length; i++) v.push_back(packet->data[i]);
								ServerSendPacket(rakServer, v, packet->systemAddress);
							}
								break;

							case 5:
							{
							#ifdef DEBUG
								vector< uchar > t;
								for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
								ulong cid = *(ulong*)t.data();
								//cout << "\nCharacter id is doing something?: " << cid;
							#endif
							}
								break;

							default:
								stringstream s;
								s << "\nworld received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
								OutputQueue->Insert(s.str());
						}

						break;

					default:
						stringstream s;
						s << "\nworld received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
						OutputQueue->Insert(s.str());
				}
				break;

			case ID_NEW_INCOMING_CONNECTION:
			#ifdef DEBUG
				OutputQueue->Insert("\n World is receiving a new connection...\n");
			#endif
				break;

			case ID_DISCONNECTION_NOTIFICATION:
			{
				auto usr = OnlineUsers->Find(packet->systemAddress);
				if (OnlineUsers->Remove(packet->systemAddress))
					OutputQueue->Insert("Disconnected " + usr->GetUsername() + "\n");
			}
				break;

			default:
				stringstream s;
				s << "\nworld received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
				OutputQueue->Insert(s.str());
		}

		rakServer->DeallocatePacket(packet);
	}

	stringstream s;
	s << "Quitting world\n";
	OutputQueue->Insert(s.str());

	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}