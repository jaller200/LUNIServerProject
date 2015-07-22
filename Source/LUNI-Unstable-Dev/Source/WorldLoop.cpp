#pragma once
#include "serverLoop.h"

#include "legoPackets.h"
#include "User.h"
#include "World.h"
#include "Database.h"
#include "GameMessages.h"
#include "WorldPackets.h"
#include "Player.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\NetworkIDManager.h"
#include "RakNet\ReplicaManager.h"

#include "Packet.h"

#include <cstdlib>
using namespace RakNet;

RakPeerInterface* rakServer; // This is the RakNet Server Instance
NetworkIDManager networkIdManager; // This handles the Network IDs
ReplicaManager replicaManager; // This is the ReplicaManager plugin

LUNIServerConnectionFactory connectionFactory; // This is the RakNet connection factory

Character *character;

void WorldLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CharactersPool> OnlineCharacters, Ref< CrossThreadQueue< string > > OutputQueue) {
	// Initialize the RakPeerInterface used throughout the entire server
	rakServer = RakNetworkFactory::GetRakPeerInterface();

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

	rakServer->SetNetworkIDManager(&networkIdManager);

	rakServer->AttachPlugin(&replicaManager);
	networkIdManager.SetIsNetworkIDAuthority(true);

	// Attach the ReplicaManager2
	replicaManager.SetAutoSerializeInScope(true);
	replicaManager.SetAutoParticipateNewConnections(true);
	replicaManager.SetAutoConstructToNewParticipants(true);

	// Set max incoming connections to 8
	rakServer->SetMaximumIncomingConnections(8);

	// If msgFileHandler is not NULL, save logs of char server
	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\world");

	// Initialize the Packet class for the packets
	Packet* packet;

	// This will be used in the saving of packets below
	int i = 0;

	ZoneId zone = NIMBUS_ISLE;

	// This will be used in the saving of packets below...
	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive(); // Recieve the packets from the server
		if (packet == NULL) continue; // If packet is NULL, just continue without processing anything

		// This will save all packets recieved from the client if running from DEBUG
		#ifdef DEBUG
			stringstream packetName;
			packetName << ".//Saves//World_Packet" << i << ".bin";

			if (packet->data[3] != 22) {
				SavePacket(packetName.str(), (char*)packet->data, packet->length);
				i++;
			}
		#endif

		// Create and send packets back here according to the one we got
			switch (packet->data[0]) {
			case ID_USER_PACKET_ENUM:
				switch (packet->data[1]) {
				case GENERAL:
					if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
						auto v = OpenPacket(".\\worldTest\\init_aw2.bin");
						ServerSendPacket(rakServer, v, packet->systemAddress);
					}
					break;

				case WORLD:

					switch (packet->data[3]) {
					case CLIENT_VALIDATION:
					{
											  auto usr = OnlineUsers->Find(packet->systemAddress);
											  //this packets contains ZoneId and something else... but what?
											  // it starts whit: 53 05 00 02 00 00 00 00 -- -- (<-ZoneId) ?? ?? ??
											  vector< uchar > v;
											  /*if (usr != NULL && usr->nextcid == 2444680020) { //monkeybrown
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
												  }*/

											  if (zone == NIMBUS_ISLE)
												  v = OpenPacket(".\\world\\char_aw2.bin");
											  else if (zone == NIMBUS_STATION) {
												  v = OpenPacket(".\\worldTest\\NS\\char_aw2.bin");
											  }
											  else if (zone == LEGO_CLUB) {
												  v = OpenPacket(".\\worldTest\\LC\\char_aw2.bin");
											  }

											  //ReadCompressedLDFData();
#ifdef DEBUG
											  if (v.size() > 0) {
												  RakNet::BitStream bs(CleanPacket(v.data(), v.size()), v.size() - 8, false);
												  ZoneId zid;
												  bs.Read(zid);
												  stringstream s;
												  s << "\nLoading world: " << zid << endl;
												  OutputQueue->Insert(s.str());
											  }
											  else OutputQueue->Insert("\nWorld Error: can't load char_aw2.bin\n");
#endif

											  ServerSendPacket(rakServer, v, packet->systemAddress);

											  if (zone == NIMBUS_ISLE) {
												  //auto character = Character::InitializeCharacter(12241);
												  //character->WriteInitPacket(rakServer, packet->systemAddress);

												  v = OpenPacket(".\\worldTest\\game_msg1.bin");
												  ServerSendPacket(rakServer, v, packet->systemAddress);

												  v = OpenPacket(".\\worldTest\\game_msg2.bin");
												  ServerSendPacket(rakServer, v, packet->systemAddress);
											  }
											  else if (zone == NIMBUS_STATION) {
												  v = OpenPacket(".\\worldTest\\NS\\msg1.bin");
												  ServerSendPacket(rakServer, v, packet->systemAddress);
											  }
											  else if (zone == LEGO_CLUB) {
												  v = OpenPacket(".\\worldTest\\LC\\msg1.bin");
												  ServerSendPacket(rakServer, v, packet->systemAddress);

												  v = OpenPacket(".\\worldTest\\LC\\msg2.bin");
												  ServerSendPacket(rakServer, v, packet->systemAddress);
											  }
					}
						break;

					case CLIENT_GAME_MSG:
					{

#ifdef DEBUG
											vector< uchar > t;
											for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
											ulong cid = *(ulong*)t.data();

											cout << "Character wants to do something using packet: " << RawDataToString(packet->data, packet->length) << endl;

											//cout << "\nCharacter id is doing something?: " << cid;
#endif

											if (packet->length >= 18) {
												ushort gameMsg;
												memcpy(&gameMsg, (uchar*)packet->data + 0x10, 2);

												cout << "Game Message ID is: " << gameMsg << endl;

												auto user = OnlineUsers->Find(packet->systemAddress);

												HandleGameMessage(rakServer, packet->systemAddress, gameMsg, (uchar*)packet->data, packet->length, user);

												break;
											}
					}

					case CLIENT_GENERAL_CHAT_MESSAGE:
					{
														auto usr = OnlineUsers->Find(packet->systemAddress);

														vector<uchar> chatMsg;

														/*for (uchar i = 0; i <= packet->length; i++) {
															chatMsg.push_back(packet->data[0x0f + i++]);

															if (packet->data[0x0f + i + 1] == 0) {
															i++;
															break;
															}
															}

															chatMsg.push_back(0);

															string msg(chatMsg.begin(), chatMsg.end());*/

														for (uchar i = 15; i < packet->length; i++) {
															chatMsg.push_back(packet->data[i]);
														}

														string wMsg(chatMsg.begin(), chatMsg.end());

														cout << "User entered: " << wMsg.c_str() << endl;

														RakNet::BitStream bitStream;

														CreatePacketHeader(ID_USER_PACKET_ENUM, 2, 1, &bitStream);

														ulonglong l1 = 0;
														uchar l2 = 0;
														ushort l3 = 0;

														ulonglong senderID = 63515;

														bitStream.Write(l1);
														bitStream.Write(l2);
														bitStream.Write((uchar)(wMsg.length() * 2 + 2));
														bitStream.Write(l2);
														bitStream.Write(l3);

														string x1 = "";

														wstring emptyUser = StringToWString(x1, 66);
														bitStream.Write((char*)emptyUser.data(), sizeof(wchar_t)* emptyUser.size() / 2);
														bitStream.Write(senderID);
														bitStream.Write(l3);
														bitStream.Write((uchar)1);
														bitStream.Write((char*)wMsg.data(), sizeof(char)* wMsg.length());

														rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
														SavePacket("msg_response.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());

														/*RakNet::BitStream bitStream2;

														CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 12, &bitStream2);

														ushort u2 = 858;
														ulong u3 = 0;
														ushort u4 = 0;

														bitStream2.Write(u1);
														bitStream2.Write(u2);

														bitStream2.Write(u3);
														bitStream2.Write(u4);
														bitStream2.Write(wMsg.length());
														bitStream2.Write(wMsg);

														rakServer->Send(&bitStream2, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

														SavePacket("hi_msg.bin", (char*)bitStream2.GetData(), bitStream2.GetNumberOfBytesUsed());*/
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
													   /*if (usr != NULL && usr->nextcid == 2444680020) {
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
														   } else { // Otherwise, reply to Cheekymonkey and open his packets*/
														cout << "\n Replying to Character\n";

														//auto v = OpenPacket("LEGO Software.bin");
														
														//Change "63450" to a chars ID in your MySQL database.
														cout << "Attempting to create new char..." << endl;
														if (character == 0) {
															cout << "Creating new char..." << endl;
															character = new Character(63450);
															replicaManager.Construct(character, false, UNASSIGNED_SYSTEM_ADDRESS, true);
															replicaManager.SignalSerializeNeeded(character, UNASSIGNED_SYSTEM_ADDRESS, true);
														}
													  
														auto v = OpenPacket(".\\world\\world_2a.bin");

														if (v.size() <= 0) {
															cout << "World 2A contains no data!!!!" << endl;
														}

														ServerSendPacket(rakServer, v, packet->systemAddress);
														// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
														v = OpenPacket(".\\world\\world_2b.bin");	// I assume these two are sent together?
														ServerSendPacket(rakServer, v, packet->systemAddress);

														auto currentChar = OnlineCharacters->Find(packet->systemAddress);
														replicaManager.Construct(currentChar.Get(), false, UNASSIGNED_SYSTEM_ADDRESS, true);

													//replicaManager.Construct(character, false, UNASSIGNED_SYSTEM_ADDRESS, true);

													   if (zone == NIMBUS_ISLE) {
														   for (int i = 1; i < 5; i++) {	// herpaderp
															   stringstream fl;
															   fl << ".\\world\\test\\world_2_" << (i + 1) << ".bin";
															   auto v = OpenPacket(fl.str());
															   ServerSendPacket(rakServer, v, packet->systemAddress);
														   }

														   //auto character = Character::InitializeCharacter(12241);
														   //character->WriteInitPacket(rakServer, packet->systemAddress);
													   }
													   else if (zone == NIMBUS_STATION) {
														   auto v = OpenPacket(".\\worldTest\\NS\\msg1.bin");
														   ServerSendPacket(rakServer, v, packet->systemAddress);

														   for (int i = 1; i < 10; i++) {
															   stringstream fl;
															   fl << ".\\worldTest\\NS\\replica\\ns_replica" << i << ".bin";

															   v = OpenPacket(fl.str());
															   ServerSendPacket(rakServer, v, packet->systemAddress);
														   }

														   for (int j = 1; j < 10; j++) {
															   stringstream fl2;
															   fl2 << ".\\worldTest\\NS\\messages\\ns_message" << j << endl;

															   v = OpenPacket(fl2.str());
															   ServerSendPacket(rakServer, v, packet->systemAddress);
														   }
													   }

													   
													   else if (zone == LEGO_CLUB) {
														   cout << "Loading LEGO CLUB" << endl;

														   for (int i = 47; i < 300; i) {
															   stringstream packetName;

															   packetName << ".\\..\\..\\..\\..\\Downloaded Packets\\GruntMonkey\\Lego Club - 42\\output_data\\" << i << "s.bin";

															   auto v = OpenPacket(packetName.str());

															   if (v.size() > 0) {
																   ServerSendPacket(rakServer, v, packet->systemAddress);
															   }

															   for (int j = 0; j < 20; j) {
																   stringstream packetName2;
																   packetName2 << ".\\..\\..\\..\\..\\Downloaded Packets\\GruntMonkey\\Lego Club - 42\\output_data\\" << i << "s_" << j << ".bin";

																   v = OpenPacket(packetName2.str());

																   if (v.size() > 0) {
																	   ServerSendPacket(rakServer, v, packet->systemAddress);
																   }

																   j++;
															   }

															   i++;
														   }

														   cout << "DONE Loading LEGO CLUB" << endl;
													   }
													   //}

													   /*cout << "Loading Venture Explorer for character..." << endl;

													   auto v = OpenPacket(".\\worldTest\\char\\char_load.bin");
													   ServerSendPacket(rakServer, v, packet->systemAddress);

													   v = OpenPacket(".\\worldTest\\char\\server_status.bin");
													   ServerSendPacket(rakServer, v, packet->systemAddress);

													   for (int i = 1; i <= 50; i++) {
													   stringstream fileName;
													   fileName << ".\\worldTest\\replica\\replica" << i << ".bin";
													   v = OpenPacket(fileName.str());
													   ServerSendPacket(rakServer, v, packet->systemAddress);
													   }*/

													   /*for (int i = 35; i <= 789; i++) {
														   stringstream fileName;

														   fileName << ".\\worldTest\\replicaTest\\" << i << "s.bin";
														   auto v = OpenPacket(fileName.str());

														   if (v.data() > 0) {
														   cout << "Sending file: " << fileName.str() << endl;
														   ServerSendPacket(rakServer, v, packet->systemAddress);
														   }

														   for (int j = 2; j < 12; j++) {
														   stringstream fileNameStr;
														   fileNameStr << ".\\worldTest\\replicaTest\\" << i << "s_" << j << ".bin";

														   v = OpenPacket(fileNameStr.str());

														   if (v.data() > 0) {
														   cout << "Sending file: " << fileNameStr.str() << endl;
														   ServerSendPacket(rakServer, v, packet->systemAddress);
														   }
														   }

														   }*/

					}
						break;

					case CLIENT_MAIL:
					{
										auto usr = OnlineUsers->Find(packet->systemAddress);

										uint mailID;
										memcpy(&mailID, packet->data + 0x08, 4);

										if (mailID == 3) {
											FetchMailData(rakServer, packet->systemAddress, usr);
										}
					}
						break;

					case CLIENT_ROUTE_PACKET:
					{
												uchar remoteConnection;
												uchar packetID;

												memcpy(&remoteConnection, packet->data + 0x0C, 1);
												memcpy(&packetID, packet->data + 0x0E, 1);

												/*if (remoteConnection == 2 && packetID == 10) {
													auto character = OnlineUsers->FindChar(packet->systemAddress);

													character->CreateFriendsList(rakServer, packet->systemAddress, character->charObjectID);
												}*/
					}
						break;

					case CLIENT_STRING_CHECK:
					{
												uchar requestID = packet->data[9];

												cout << "Getting char moderation request with id: " << requestID << endl;

												RakNet::BitStream bitStream;
												CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 59, &bitStream);

												bitStream.Write((uchar)1);
												bitStream.Write((ushort)0);
												bitStream.Write(requestID);

												rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					}
						break;

					case 22: //user moving / update request?
					{
								 auto usr = OnlineUsers->Find(packet->systemAddress);
#ifdef DEBUG
								 //OutputQueue->Insert("Received packet 22: " + RawDataToString(packet->data, packet->length) + "\n");
#endif
					}
						break;

						/*case LUNI_WORLD_CHAT: //user wrote a chat message
						{
						if (packet->length >= 10 && RawDataToString(packet->data + packet->length - 10, 10, true) == "730064006f0077006e00") {//virus used to be here
						}

						// dummy try
						vector< uchar > v(packet->length);
						for (uint i = 0; i < packet->length; i++) v.push_back(packet->data[i]);
						ServerSendPacket(rakServer, v, packet->systemAddress);
						}*/
						break;

						/*case 5:
						{
						#ifdef DEBUG
						vector< uchar > t;
						for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
						ulong cid = *(ulong*)t.data();

						cout << "Character wants to do something using packet: " << RawDataToString(packet->data, packet->length) << endl;

						//cout << "\nCharacter id is doing something?: " << cid;
						#endif

						if (packet->data[0x1A] == 123 && packet->data[0x1B] == 33 & packet->data[0x1C] == 22) {
						cout << "Sending char to new world..." << endl;

						zone = NIMBUS_STATION;

						auto v = OpenPacket(".\\worldTest\\init_aw2.bin");
						ServerSendPacket(rakServer, v, packet->systemAddress);
						}
						}
						break;*/

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
			{
			#ifdef DEBUG
				OutputQueue->Insert("\n World is receiving a new connection...\n");
			#endif

				//replicaManager.AddNewConnection(packet->systemAddress);
				/*Player *newPlayer = new Player;
				newPlayer->SetReplicaManager(&replicaManager);
				newPlayer->systemAddress = packet->systemAddress;
				newPlayer->LOT = 1;
				newPlayer->AddAutoSerializeTimer(100);
				newPlayer->BroadcastConstruction();*/

			}
				break;

			case ID_DISCONNECTION_NOTIFICATION:
			{
				auto usr = OnlineUsers->Find(packet->systemAddress);
				if (OnlineUsers->Remove(packet->systemAddress))
					OutputQueue->Insert("Disconnected " + usr->GetUsername() + "\n");
			}

				//Player::DeletePlayerByAddress(packet->systemAddress);
				delete character;

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
