#include "serverLoop.h"

#include "legoPackets.h"
#include "User.h"
#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"
using namespace std;

void CharactersLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();

	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		rakServer->AttachPlugin(msgFileHandler);
	}

	InitSecurity(rakServer, cfg->useEncryption);
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);

	if (rakServer->Startup(8, 30, &socketDescriptor, 1)) {
		stringstream s;
		s << "characters started! Listening on: " << cfg->listenPort << "\n";
		OutputQueue->Insert(s.str());
	} else exit(2);
	rakServer->SetMaximumIncomingConnections(8);

	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\char");

	Packet* packet;

	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive();
		if (packet == NULL) continue;
		PrintPacketInfo(packet, msgFileHandler);
		// create and send packets back here according to the one we got

		switch (packet->data[0]) {
			case ID_LEGO_PACKET:

				switch (packet->data[1]) {
					case LUNI_INITAW:
						if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
							auto v = OpenPacket(".\\char\\init_aw.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);
						}
						break;

					case LUNI_CHARACTER:

						switch (packet->data[3]) {
							case LUNI_CHARACTER_CHARSDATA: // char response: 05, 06
								// char traffic only - byte 8 defines number of characters, if there are no characters the user automatically gets to the character creation screen
							{
								//giving client characters list
								auto v = OpenPacket(".\\char\\char_aw2.bin");
								ServerSendPacket(rakServer, v, packet->systemAddress);

							#ifdef DEBUG
								/*StringCompressor* sc = StringCompressor::Instance();
								Ref<char> output = new char[256];
								RakNet::BitStream* bs = new RakNet::BitStream(v.data(), v.size(), false);
								if (sc->DecodeString(output.Get(), 256, bs))
									cout << ("\n the TEST: " + RawDataToString((uchar*)output.Get(), 256) + "\n");
								else cout << ("\n the Test failure :(\n");*/
								RakNet::BitStream bs(v.data(), v.size(), false);
								vector< uchar > out(v.size() *2);
								bs.ReadCompressed(out.data(), out.size(), true);
								//cout << "\n the Test: " << RawDataToString( s.Get(), v.size() ) << endl;
								//SavePacket("theTest.bin", out);
							#endif
							}
								break;

							case LUNI_CHARACTER_CREATEDNEW:
							{
								CharacterCreation cc(CleanPacket(packet->data, packet->length));

								stringstream s;
								s << "\nSomebody wants to create a character with name: " << cc.GetName() << endl;
								s << "re-Serialization: " << RawDataToString(cc.Serialize(), cc.GetGeneratedPacketSize()) << endl;
								OutputQueue->Insert(s.str());
								// response: 05, 06
								// since we can't build our own packets yet we can't send a response here (because it is dependent on the user input)
								vector< uchar > reply(2);
								reply.push_back(5);
								reply.push_back(6);
								ServerSendPacket(rakServer, reply, packet->systemAddress);
							}
								break;

							case 4: // is this sent from the client once the user wants to enter world?
							{
								auto usr = OnlineUsers->Find(packet->systemAddress);
								if (usr != NULL) {
									usr->numredir++;
									//character id is received
									vector< uchar > t;
									for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
									usr->nextcid = *(ulong*)t.data();

								#ifdef DEBUG
									stringstream s;
									s << "\nCharacter logging in world with id: " << usr->nextcid;
									if ( usr->nextcid == 2397732190 ) s << " CheekyMonkey!\n";
									else if ( usr->nextcid == 2444680020 ) s << " monkeybrown!\n";
									else if ( usr->nextcid == 1534792735 ) s << " GruntMonkey!\n";
									else if ( usr->nextcid == 1457240027 ) s << " Shafantastic!\n";
									else s << " Unknow cid: " << usr->nextcid << endl;
									OutputQueue->Insert(s.str());
								#endif
								}

								auto v = OpenPacket(".\\char\\char_aw_redirect.bin");
								if (v.size() > 0) {
									// ip starts at byte 8, port starts at byte 0x29
									if (cfg->redirectIp[0] != 0)
										memcpy(v.data() + 0x8, cfg->redirectIp, sizeof(cfg->redirectIp) - 3);	// NOTE: the IP can currently only be 13 characters long here since there are some non-zero bytes directly after the IP and I don't want to delete them (is it safe to overwrite them?)
									if (cfg->redirectPort > 0)
										memcpy(v.data() + 0x29, &cfg->redirectPort, sizeof(cfg->redirectPort));
									ServerSendPacket(rakServer, v, packet->systemAddress);
								}
								rakServer->CloseConnection(packet->systemAddress, true);
							}
								break;

							case LUNI_CHARACTER_DELETED:
								stringstream s;
								s << "\n Case LUNI_CHARACTER_DELETED: " << RawDataToString(packet->data, packet->length) << endl;
								OutputQueue->Insert(s.str());
								break;
						}

						break;

					default:
						stringstream s;
						s << "\ncharacters received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
						OutputQueue->Insert(s.str());
				}

				break;

			case ID_NEW_INCOMING_CONNECTION:
			#ifdef DEBUG
				OutputQueue->Insert("\n Characters is receiving a new connection...\n");
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
				s << "\ncharacters received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
				OutputQueue->Insert(s.str());
		}
	}
}