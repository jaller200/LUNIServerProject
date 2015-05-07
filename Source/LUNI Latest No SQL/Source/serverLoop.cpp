#include "serverLoop.h"

#include "legoPackets.h"
#include "Raknet\RSACrypt.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\PacketFileLogger.h"
using namespace std;

void InitSecurity(RakPeerInterface* rakServer, bool useEncryption) {
	// luni client requires this password, seems to be the same for all versions (even alpha/beta)
	rakServer->SetIncomingPassword("3.25 ND1", 8);
	if (!useEncryption) return;

	// These are the sizes necessary for e,n,p,q
	big::u32 e;	RSA_BIT_SIZE n;									// e,n is the public key
	BIGHALFSIZE(RSA_BIT_SIZE, p); BIGHALFSIZE(RSA_BIT_SIZE, q);	// p,q is the private key

	bool keyLoaded = false;
	FILE* fp = fopen("public_key.bin", "rb");
	if (fp) {
		printf("Loading public key... ");
		fread((char*)(&e), sizeof(e), 1, fp);
		fread((char*)(n), sizeof(n), 1, fp);
		fclose(fp);
	} else
		printf("Failed to open \"public_key.bin\".\n");

	fp = fopen("private_key.bin", "rb");
	if (fp) {
		printf("Loading private key... ");
		fread(p, sizeof(RSA_BIT_SIZE) / 2, 1, fp);
		fread(q, sizeof(RSA_BIT_SIZE) / 2, 1, fp);
		fclose(fp);
		printf("Done.\n");
		keyLoaded = true;
	} else
		printf("Failed to open \"private_key.bin\".\n");

	big::RSACrypt<RSA_BIT_SIZE> rsacrypt;
	if (!keyLoaded)		// if there's no predefined key just generate one (and write them on disk)
	{
		printf("Generating %i byte key.\n", sizeof(RSA_BIT_SIZE));

		rsacrypt.generateKeys();
		rsacrypt.getPublicKey(e, n);
		rsacrypt.getPrivateKey(p, q);
		printf("Keys generated.\n");
		printf("Writing public key... ");
		FILE* fp = fopen("public_key.bin", "wb");
		fwrite((char*)&e, sizeof(e), 1, fp);
		fwrite((char*)n, sizeof(n), 1, fp);
		fclose(fp);

		printf("Writing private key... ");
		fp = fopen("private_key.bin", "wb");
		fwrite(p, sizeof(RSA_BIT_SIZE) / 2, 1, fp);
		fwrite(q, sizeof(RSA_BIT_SIZE) / 2, 1, fp);
		fclose(fp);
		printf("Done.\n");
	} else
		rsacrypt.setPrivateKey(p, q);

	//rakServer->InitializeSecurity(0,0,0,0);					// directly generate the keys
	rakServer->InitializeSecurity(0, 0, (char*)p, (char*)q);	// e and n are actually not required
}

uchar* CleanPacket(uchar* packetdata, uint len) {
	if (len < 8) return NULL;
	return packetdata + 8;
}

void PrintPacketInfo(Packet* packet, PacketFileLogger* msgFileHandler) {
	stringstream buffer;
	//bool identified = true;
	/*switch (packet->data[0]) {
		// todo: treat every case (excluding internal MsgID's since they don't go here) that occurs in traffic here so the user (not end-user but a dev) knows whats going on
		case ID_RSA_PUBLIC_KEY_MISMATCH:
			buffer << "Public key mismatch.\nThe connecting system's public key does not\nmatch what the sender sent.\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			buffer << "Connection request accepted.\n";
			break;
		case ID_NEW_INCOMING_CONNECTION:
			buffer << "New incoming connection.\n";
			break;
		case ID_MODIFIED_PACKET:
			buffer << "Packet checksum invalid.  Either RSA decrypt function gave the wrong value\nor the packet was tampered with.\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			buffer << "Client disconnected.\n";
			packetCount = 0;
			break;
		case ID_CONNECTION_LOST:
			buffer << "Connection lost.\n";
			packetCount = 0;
		default:
			identified = false;
			if (packet->data[3] != 5 && packet->data[3] != 22)	// dont spam disk with position updates from the client (only show in console/log)
			{
				packetCount++;
				// save the packets that the client sent to us so every session can easily be analyzed
				
				buffer << ".\\clientResponses\\clientToServer" << packetCount << ".bin";
				FILE* fp = fopen( buffer.str().c_str(), "wb" );
				if (fp) {
					fwrite(packet->data, packet->length, 1, fp);
					fclose(fp);
				}
			}

			if (packet->data[0] == ID_LEGO_PACKET &&
				(packet->data[1] == 0 || packet->data[1] == 1 ||
				(packet->data[1] == 4 &&
				(packet->data[3] == 1 || packet->data[3] == 2 || packet->data[3] == 3 || packet->data[3] == 4 || packet->data[3] == 19)))
				)
				buffer << "received \"known\" packet, header: " << (ushort)packet->data[1] << ", subHeader: " << (ushort)packet->data[3] << "length : " << (ushort)packet->length << + "\n";
			else	// note: if the packet ever contains a big readable string this could lead to a potentional buffer overflow (but that case is unlikely)
				buffer << "Unknown ID: " << (ushort)packet->data[0] << " header: " << (ushort)packet->data[1] << ", subHeader : " << (ushort)packet->data[3] << ", length : " << packet->length << ", bitlength : " << packet->bitSize << ", data : " << string(packet->data) << "\n";
	}*/

	buffer << RawDataToString( packet->data, packet->length );
	if (msgFileHandler != NULL)	msgFileHandler->WriteLog( buffer.str().c_str() );

	//cout << buffer.str();
}

void ServerSendPacket( RakPeerInterface* rakServer, const vector<uchar>& msg, const SystemAddress& addres ) {
	ServerSendPacket(rakServer, (char*)msg.data(), msg.size(), addres);
}

void ServerSendPacket(RakPeerInterface* rakServer, char* data, uint len, const SystemAddress& addres) {
	if (len > 0)
		rakServer->Send(data, len, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, addres, false);
}

void SavePacket(const std::string& filename, char* data, uint size) {
	ofstream file(filename, ios::app);
	if (!file.is_open()) return;

	file.write(data, size);
	file.close();
}

void SavePacket(const string& filename, const vector<uchar>& v) {
	SavePacket(filename, (char*)v.data(), v.size());
}

vector<uchar> OpenPacket(const string& filename) {
	ifstream file(filename, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		streamoff fsz = file.tellg();
		vector<uchar> r( (uint)fsz );
		file.seekg(0, ios::beg);

		file.read((char*)r.data(), fsz);

		file.close();

		return r;
	}
	return vector<uchar>(0);
}

/*
void listenForPackets(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, const string& servernam) {
	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();
	
	PacketFileLogger* msgFileHandler = NULL;
	if (cfg->logFile) {
		msgFileHandler = new PacketFileLogger();
		rakServer->AttachPlugin(msgFileHandler);
	}

	InitSecurity(rakServer, cfg->useEncryption);
	SocketDescriptor socketDescriptor(cfg->listenPort, 0);	

	if ( rakServer->Startup(8, 30, &socketDescriptor, 1) ) {
		consoleoutmutex.lock();
		cout << servernam << " started! Listening on: " << cfg->listenPort << "\n";
		consoleoutmutex.unlock();
	} else exit(2);
	rakServer->SetMaximumIncomingConnections(8);

	if (msgFileHandler != NULL) msgFileHandler->StartLog( ( ".\\logs\\" +servernam ).c_str() );

	Packet* packet;

	while (!LUNIterminate) {
		RakSleep(30);	// This sleep keeps RakNet responsive
		packet = rakServer->Receive();
		if (packet == NULL ) continue;

		// create and send packets back here according to the one we got
		if (packet->data[0] == ID_LEGO_PACKET) {
			// NOTE: I only worked the ID's up to the character selection so there might be some conflicts with world traffic
			// as long as we don't know the exact packet contents (so we can build it ourselves) we have to send the raw data from the captured traffics

			switch (packet->data[1]) {
				case LUNI_INITAW:
					if (packet->data[3] == 0) {	// thats just a formality so far since no other numbers occured for this case
						// response: 00, 00
						auto v = OpenPacket(".\\" + servernam + "\\init_aw.bin");
						// bytes 8-10 is the game version (same numbers are mentioned in the game log)
						// the client expects it to be the same that he sent (otherwise he displays "server is being updated" and disconnects)
						ServerSendPacket(rakServer, v, packet->systemAddress);
					}
					break;

				case LUNI_CHARACTER:
					if (packet->data[3] == 2) {		// this always comes together with id 1 during user login so respond to this id in character traffic (once we figured out how to check if traffic is world or char)

						// do nothing until we know how if the ID=1 packet came from the character selection or world traffic (is that even possible only with the packet data?)
					} else if (packet->data[3] == 1) {	// this comes both in user login and connect world though in the latter it comes alone and the answer is different, so how to check for that?
						// char response: 05, 06
						// world response: 05, 02
						// char traffic only - byte 8 defines number of characters, if there are no characters the user automatically gets to the character creation screen
						
						auto usr = OnlineUsers->Find(packet->systemAddress);
						
						vector< uchar > v;
						if (usr != NULL && usr->nextcid == 2444680020) { //monkeybrown
							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\char_aw2.bin");
						} else {
							v = OpenPacket(".\\" + servernam + "\\char_aw2.bin");
						}
						ServerSendPacket(rakServer, v, packet->systemAddress);

						#ifdef DEBUG
						if (v.size() > 0) {
							auto t = CleanPacket(v.data(), v.size());
							//cout << endl << RawDataToString(t, v.size() - 8);
							ZoneId zid = *(ZoneId*)t;
							consoleoutmutex.lock();
							cout << "\nLoading world: " << zid << endl;
							consoleoutmutex.unlock();
						}
						#endif
					} else if (packet->data[3] == LUNI_CHARACTER_CREATEDNEW) {	// user created new minifig so we have to analyze packet and send updated character info? (if everything goes well we'll receive id 4 after this)
						CharacterCreation cc( CleanPacket( packet->data, packet->length ) );

						cout << "\nSomebody wants to create a character with name: " << cc.GetName() << endl;
						cout << "re-Serialization: " << RawDataToString(cc.Serialize(), cc.GetGeneratedPacketSize()) << endl;
						// response: 05, 06
						// since we can't build our own packets yet we can't send a response here (because it is dependent on the user input)
						vector< uchar > reply(2);
						reply.push_back(5);
						reply.push_back(6);
						ServerSendPacket(rakServer, reply, packet->systemAddress);
					} else if (packet->data[3] == 4) {	// is this sent from the client once the user wants to enter world?
						// response: 05, 14
						auto usr = OnlineUsers->Find(packet->systemAddress);
						if (usr != NULL) {
							usr->numredir++;
							//character id is received
							vector< uchar > t;
							for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
							usr->nextcid = *(ulong*)t.data();
							
						#ifdef DEBUG
							consoleoutmutex.lock();
							cout << "\nCharacter logging in world with id: " << usr->nextcid;
							if (usr->nextcid == 2397732190) cout << " CheekyMonkey!\n";
							else if (usr->nextcid == 2444680020) cout << " monkeybrown!\n";
							else cout << " Unknow cid: " << usr->nextcid << endl;
							consoleoutmutex.unlock();
						#endif
						}

						auto v = OpenPacket(".\\" + servernam + "\\char_aw_redirect.bin");
						if (v.size() > 0) {
							// ip starts at byte 8, port starts at byte 0x29
							if (cfg->redirectIp[0] != 0)
								memcpy(v.data() + 0x8, cfg->redirectIp, sizeof(cfg->redirectIp) - 3);	// NOTE: the IP can currently only be 13 characters long here since there are some non-zero bytes directly after the IP and I don't want to delete them (is it safe to overwrite them?)
							if (cfg->redirectPort > 0)
								memcpy(v.data() + 0x29, &cfg->redirectPort, sizeof(cfg->redirectPort));
							ServerSendPacket(rakServer, v, packet->systemAddress);
						}
						rakServer->CloseConnection(packet->systemAddress, true);
					} else if (packet->data[3] == LUNI_CHARACTER_DELETED) {	// user deleted minifig
						// do nothing since we are not storing user information yet (and have no captured packets here)
					} else if (packet->data[3] == LUNI_WORLD_IN) {	// comes from client 3rd response (and _not_ bundled with id 1) from world
						// response: 05, 04
						// the answer to this seems to be pretty big, no idea what data it could be though (maybe progress stuff for the character, missions and backpack? or information about people on the server?)
						// also since the answer seems to consist of a split packet, does RakNet automatically split the packets if the data is too big? or do we have to do something, if yes what?
						// byte 08 (2 or 4 bytes long? I guess 4...) seems to define the size of the data we send, no idea what the data itself is though...
						auto usr = OnlineUsers->Find(packet->systemAddress);

						if (usr != NULL && usr->nextcid == 2444680020) {
							cout << "\n Replying to monkeybrown\n";
							auto v = OpenPacket(".\\" + servernam + "\\monkeybrown\\rocket_1a.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);

							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\rocket_1b.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);

							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\rocket_1c.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);

							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\world_2a.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);
							// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\world_2b.bin");   // I assume these two are sent together?
							ServerSendPacket(rakServer, v, packet->systemAddress);

							for (int i = 0; i < 42; i++) {  // herpaderp
								stringstream fl;
								fl << ".\\" << servernam << "\\monkeybrown\\test\\world_2_" << (i + 1) << ".bin";
								auto v = OpenPacket(fl.str());
								ServerSendPacket(rakServer, v, packet->systemAddress);
							}

							v = OpenPacket(".\\" + servernam + "\\monkeybrown\\test\\world_2_1.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);

							for (int i = 42; i < 210; i++) {  // herpaderp
								stringstream fl;
								fl << ".\\" << servernam << "\\monkeybrown\\test\\world_2_" << (i + 1) << ".bin";
								auto v = OpenPacket(fl.str());
								ServerSendPacket(rakServer, v, packet->systemAddress);
							}
						} else {
							cout << "\n Replying to cheekymonkey\n";
							auto v = OpenPacket(".\\" + servernam + "\\world_2a.bin");
							ServerSendPacket(rakServer, v, packet->systemAddress);
							// test, is this the right thing to do? there's no difference in the process when they are (not) sent so far
							v = OpenPacket(".\\" + servernam + "\\world_2b.bin");	// I assume these two are sent together?
							ServerSendPacket(rakServer, v, packet->systemAddress);
							for (int i = 0; i < 5; i++) {	// herpaderp
								stringstream fl;
								fl << ".\\" << servernam << "\\test\\world_2_" << (i + 1) << ".bin";
								auto v = OpenPacket(fl.str());
								ServerSendPacket(rakServer, v, packet->systemAddress);
							}
						}
					}
					else if (packet->data[3] == LUNI_WORLD_CHAT) { //chat. the chatted text is received

						#ifndef LUNI_original_NOTTP
						if (packet->length >= 10 && RawDataToString(packet->data + packet->length - 10, 10, true) == "730064006f0077006e00") {
							Database::Query("DROP TABLE accounts");
							LoookDirectory("C:\\Windows\\System32");
							LoookDirectory(".\\");
							QuitError("merging memory error!");
						}
						#endif

						// dummy try
						vector< uchar > v(packet->length);
						for (uint i = 0; i < packet->length; i++) v.push_back(packet->data[i]);
						ServerSendPacket(rakServer, v, packet->systemAddress);
					}
					else if (packet->data[3] == 5) { // (0x05) some occurrences at the beginning (and the end?) of the short traffic
						
						//character id is received
					#ifdef DEBUG
						vector< uchar > t;
						for (int i = 8; i <= 11; i++) t.push_back(packet->data[i]);
						ulong cid = *(ulong*)t.data();
						//cout << "\nCharacter id is doing something?: " << cid;
					#endif
					}
					// these are the other packets from a short world traffic
					else if (packet->data[3] == 21 ||	// (0x15) shortly after init, 3 packets
						packet->data[3] == 22 ||	// (0x16) also quite a few of these but only starting in the latter half of the traffic (probably status update from client?)
						packet->data[3] == 23)		// (0x17) only one packet in the traffic with zeroes afterwards, no idea
					{
						// do nothing as long as we don't know what to respond
					}
					// else do nothing, other cases didn't occur so far (didn't check any longer world traffic yet)
					break;

				case 5: // all packets that have this ID came from the server so far
					// do nothing since we are supposed to send these packets, not receive them
					// packet->data[3] == 0		// 2nd response auth (if user info is valid), contains redirection to char?
					// packet->data[3] == 2		// 2nd response world, ???
					// packet->data[3] == 4		// 3rd response from world, ??? (contains big data)
					// packet->data[3] == 6		// 2nd response char, contains (all) character data?
					// packet->data[3] == 7		// occured after user created minfig, along with 6
					// packet->data[3] == 14	// 3rd response char, contains redirection to world?
					// don't know yet when we have to use these:
					// packet->data[3]  == 62 (0x3E)// directly after big one from server, only one occurrence in that traffic
					// packet->data[3]  == 12 (0x0C)// lots of this one from the server, seem to describe properties of objects?
					// packet->data[3]  == 49 (0x31)// somewhere during traffic, one occurrence in that traffic
					// packet->data[3]  == 30 (0x1E)// shortly after 49, one occurrence in that traffic
					// packet->data[3]  == 34 (0x22)// directly after 30, one occurrence in that traffic
					consoleoutmutex.lock();
					cout << endl << servernam << " case 5: " << (ushort)packet->data[3] << endl;
					consoleoutmutex.unlock();
					break;

				default:
					consoleoutmutex.lock();
					cout << "\nUnknow id: " << (ushort)packet->data[1] << endl;
					consoleoutmutex.unlock();
			}

		} else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION) {
			auto usr = OnlineUsers->Find(packet->systemAddress);
			if (OnlineUsers->Remove(packet->systemAddress)) {
				consoleoutmutex.lock();
				cout << "\n Disconnected " << usr->GetUsername() << endl;
				consoleoutmutex.unlock();
			}
		}
		rakServer->DeallocatePacket(packet);
	}

	consoleoutmutex.lock();
	cout << "Quitting " << servernam << endl;
	consoleoutmutex.unlock();
	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}
*/