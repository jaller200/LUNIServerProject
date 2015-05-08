#include "serverLoop.h"

#include "Database.h"
#include "User.h"
#include "legoPackets.h"
#include "RakNet\RakSleep.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\PacketFileLogger.h"

using namespace std;

Ref< User > HandleUserLogin(RakPeerInterface* rakServer, Packet* packet, CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers) {
	if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
		// response: 05, 00
		string username, password;
		// get the username just for fun
		if (packet->length > 0x8) {	// this is where username always starts (how many bytes are reserved for it in the packet?)
			int i = 0;
			while (packet->data[0x8 + i] != '\0' && packet->length > (uint)(0x8 + i)) {
				username.push_back(packet->data[8 + i]);
				i += 2;
			}
		}
		// get the password just for fun
		if (packet->length > 0x4a) {	// this is where password starts (how many bytes are reserved for it in the packet?)

			int i = 0;
			while (packet->data[0x4a + i] != '\0' && packet->length > (uint)(0x4a + i)) {
				password.push_back(packet->data[0x4a + i]);
				i += 2;
			}
		}

		auto usr = User::Login(username, password, packet->systemAddress);
		// we could do a login check here and then send the according response but as long as we use predefined data to send away its not worth the trouble
		if (usr == NULL) {	// very simple check
			auto v = OpenPacket(".\\auth\\auth_aw_login_bad.bin");
			ServerSendPacket(rakServer, v, packet->systemAddress);
			return NULL;
		} else {
			OnlineUsers->Insert(usr, packet->systemAddress);
			// 0x159 is where IP to redirect to starts (don't know how much bytes are reserved for it in the packet, I would imagine 16
			// but since there was always a great zero gap until the port in all these captured packets its nothing to worry about (yet)
			auto v = OpenPacket(".\\auth\\auth_aw_login_ok.bin");
			if (v.size() > 0) {
				if (v.size() > 0x19B) {	// 0x19B is port (2 bytes reserved or more?)
					if (cfg->redirectIp[0] != 0)
						memcpy(v.data() + 0x159, cfg->redirectIp, sizeof(cfg->redirectIp));
					if (cfg->redirectPort > 0)
						memcpy(v.data() + 0x19B, &cfg->redirectPort, sizeof(cfg->redirectPort));
				}
				ServerSendPacket(rakServer, v, packet->systemAddress);
			}
			return usr;
		}
	}
	return NULL;
}

void AuthLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< string > > OutputQueue) {
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
		s << "auth started! Listening on: " << cfg->listenPort << "\n";
		OutputQueue->Insert(s.str());
	} else QuitError("auth server init error!");
	rakServer->SetMaximumIncomingConnections(8);

	if (msgFileHandler != NULL) msgFileHandler->StartLog(".\\logs\\auth");

	Packet* packet;

	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive();
		if (packet == NULL) continue;
		PrintPacketInfo(packet, msgFileHandler);

		switch (packet->data[0]) {
			case ID_LEGO_PACKET:

				switch (packet->data[1]) {
					case LUNI_INITAW:
						if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
							// response: 00, 00
							auto v = OpenPacket(".\\auth\\init_aw.bin");
							// bytes 8-10 is the game version (same numbers are mentioned in the game log)
							// the client expects it to be the same that he sent (otherwise he displays "server is being updated" and disconnects)
							ServerSendPacket(rakServer, v, packet->systemAddress);
						}
						break;

					case LUNI_LOGIN: //user logging into server
						{
							auto usr = HandleUserLogin(rakServer, packet, cfg, OnlineUsers);
							if( usr != NULL ) OutputQueue->Insert("\n" + usr->GetUsername() + " Logged-in\n");
						#ifdef DEBUG
							else OutputQueue->Insert("\n Login failed!\n");
						#endif
						}
						break;

					default:
						stringstream s;
						s << "\nauth received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
						OutputQueue->Insert(s.str());
				}

				break;

			case ID_NEW_INCOMING_CONNECTION:
#			ifdef DEBUG
				OutputQueue->Insert("\n Auth is receiving a new connection...\n");
			#endif
				break;

			case ID_DISCONNECTION_NOTIFICATION: // do nothing
				break;

			default:
				stringstream s;
				s << "\nauth received unknow pakcet: " << RawDataToString(packet->data, packet->length) << endl;
				OutputQueue->Insert(s.str());
		}

		rakServer->DeallocatePacket(packet);
	}

	stringstream s;
	s << "Quitting auth\n";
	OutputQueue->Insert(s.str());

	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}
