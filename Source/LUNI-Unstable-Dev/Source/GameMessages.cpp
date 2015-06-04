#include "GameMessages.h"
#include "Packet.h"

// Handle a game message that is coming from the client
void HandleGameMessage(RakPeerInterface *rakServer, SystemAddress &systemAddr, ushort gameMsg, uchar *packetData, uint length, Ref<User> user) {
	switch (gameMsg) {
	case CLIENT_CHAT_PARSE:
		cout << "Parsing chat command game msg..." << endl;
		//ParseChatCommand(rakServer, systemAddr, packetData, length, user);
		break;

	case CLIENT_TOGGLE_MAIL:
		cout << "Sending command to toggle the mail...";
		ToggleMail(rakServer, systemAddr, packetData);
		break;

	default:
		cout << "Unknown game message" << endl;
	}
}

void ParseChatCommand(RakPeerInterface *rakServer, SystemAddress &systemAddr, uchar *packetData, uint length, Ref<User> user) {
	cout << "Packet data is: \n" << packetData << endl;

	ulonglong userId;
	vector<uchar> chatMsg;

	memcpy(&userId, packetData + 0x08, 8);

	// Create the RakNet bitstream/packet header
	RakNet::BitStream bitStream;
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 12, &bitStream);

	// Get the object ID and copy it from the packet data
	ulonglong objectId;
	memcpy(&objectId, packetData + 0x08, 8);

	// Write ObjectID
	bitStream.Write(objectId);

	// Write the 
	bitStream.Write(SERVER_CHAT_PARSE);

	bitStream.Write(0);
	bitStream.Write((ushort)0);
	
	/*for (uint i = 0x1A; i < length; i++) {
		chatMsg.push_back(packetData[i]);
		chatMsg.push_back(0);
		i++;

		if (packetData[i + 1] = 0x00 || i + 1 > length) break;
	}*/

	for (uint i = 0x1A; i <= length; i++) {
		chatMsg.push_back(packetData[i]);
	}

	int chatSize = chatMsg.size();

	wstring wMsg(chatMsg.begin(), chatMsg.end());

	bitStream.Write(wMsg);
	bitStream.Write(chatMsg.data());

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddr, false);	


}

void ToggleMail(RakPeerInterface *rakServer, SystemAddress &systemAddr, uchar *packetData) {
	ulonglong objectID;
	memcpy(&objectID, packetData + 0x08, 8);

	auto v = OpenPacket(".\\worldTest\\mail\\mail_01.bin");
	if (v.size() == 0) {
		cout << "Unable to open mail_01.bin!!!";
	}

	memcpy(&v + 0x08, &objectID, 8);

	ServerSendPacket(rakServer, v, systemAddr);

	v = OpenPacket(".\\worldTest\\mail\\mail_02.bin");
	if (v.size() == 0) {
		cout << "Unable to open mail_02.bin!!!";
	}

	memcpy(&v + 0x08, &objectID, 8);

	ServerSendPacket(rakServer, v, systemAddr);
}
