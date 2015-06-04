#include "AuthPackets.h"
#include "serverLoop.h"
#include "Packet.h"

#include "RakNet\BitStream.h"

using namespace std;

void SendStatusPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, UserSuccess loginStatus, string redirectIpAddress, ushort redirectPort, ushort extraDataLength) {
	RakNet::BitStream bitStream; // Create the bitStream

	// Always create the packet header
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 0, &bitStream);

	LoginStatusPacket loginStatusPacket;

	// Set the loginStatus
	loginStatusPacket.loginStatus = loginStatus;

	// Set Talk_Like_A_Pirate_String
	loginStatusPacket.talkLikeAPirate = "Talk_Like_A_Pirate";
	loginStatusPacket.unknownString = "";

	// Set client version
	loginStatusPacket.clientVersion1 = 1;
	loginStatusPacket.clientVersion2 = 10;
	loginStatusPacket.clientVersion3 = 64;

	// This is unknown data...
	loginStatusPacket.unknown = "_";

	// Get the user key
	loginStatusPacket.userKey = "0 9 4 e 7 0 1 a c 3 b 5 5 2 0 b 4 7 8 9 5 b 3 1 8 5 7 b f 1 c 3   ";

	// Set chat IPs/Port and the other IP
	loginStatusPacket.chatIp = "localhost";
	loginStatusPacket.chatPort = 2004;
	loginStatusPacket.anotherIp = "localhost";

	loginStatusPacket.possibleGuid = "00000000-0000-0000-0000-000000000000";

	loginStatusPacket.zeroShort = 0;

	// Set localization
	loginStatusPacket.localizationChar[0] = 0x55;
	loginStatusPacket.localizationChar[1] = 0x53;
	loginStatusPacket.localizationChar[2] = 0x00;

	// Subscribed?
	loginStatusPacket.firstLoginSubscription = 0;
	loginStatusPacket.subscribed = 0;

	loginStatusPacket.zeroLong = 0;

	loginStatusPacket.redirectIp = redirectIpAddress;
	loginStatusPacket.redirectPort = redirectPort;

	// Sett the error msg and the error msg length
	// This message only shows
	loginStatusPacket.errorMsg = "T";
	loginStatusPacket.errorMsgLength = 0;

	// Set the extraBytesLength to the one the user defined
	loginStatusPacket.extraBytesLength = extraDataLength;

	// ---- CREATE BITSTREAM ---- //
	// Write the connectionId to bitStream
	bitStream.Write(loginStatusPacket.loginStatus);

	// Write the "Talk_Like_A_Pirate" string to bitStream
	WriteStringToBitStream(loginStatusPacket.talkLikeAPirate.c_str(), 18, 33, &bitStream);

	// Write 7 blocks of data (length: 33) to bitStream
	// In the original login_status_ok.bin, these sometimes ended in
	// bytes other than 0x00, but losing that doesn't seem to have much of an effect... yet.
	for (int i = 0; i < 7; i++) {
		WriteStringToBitStream(loginStatusPacket.unknownString.c_str(), 0, 33, &bitStream);
	}

	// Write the clientVersion to bitStream (all 3 parts)
	bitStream.Write(loginStatusPacket.clientVersion1);
	bitStream.Write(loginStatusPacket.clientVersion2);
	bitStream.Write(loginStatusPacket.clientVersion3);

	// Write the userKey, redirectIp, and chatIp to bitStream
	WriteStringToBitStream(loginStatusPacket.userKey.c_str(), 66, 66, &bitStream);
	WriteStringToBitStream(loginStatusPacket.redirectIp.c_str(), sizeof(loginStatusPacket.redirectIp), 33, &bitStream);
	WriteStringToBitStream(loginStatusPacket.chatIp.c_str(), sizeof(loginStatusPacket.chatIp), 33, &bitStream);
	
	// Write the redirectPort and the chatPort to bitStream
	bitStream.Write(loginStatusPacket.redirectPort);
	bitStream.Write(loginStatusPacket.chatPort);
	
	// Write seperate ip(?) and possibleGuid to bitStream
	WriteStringToBitStream(loginStatusPacket.anotherIp.c_str(), sizeof(loginStatusPacket.anotherIp), 33, &bitStream);
	WriteStringToBitStream(loginStatusPacket.possibleGuid.c_str(), 37, 37, &bitStream);
	
	// Write the zeroShort to the errorMsgLength to the bitStream
	bitStream.Write(loginStatusPacket.zeroShort);
	bitStream.Write(loginStatusPacket.localizationChar);
	bitStream.Write(loginStatusPacket.firstLoginSubscription);
	bitStream.Write(loginStatusPacket.subscribed);
	bitStream.Write(loginStatusPacket.zeroLong);
	bitStream.Write(loginStatusPacket.errorMsgLength);
	
	// Write the error msg string to the bitStream
	WriteStringToBitStream(loginStatusPacket.errorMsg.c_str(), loginStatusPacket.errorMsgLength, 1, &bitStream);
	
	// Add extra bytes length
	bitStream.Write(loginStatusPacket.extraBytesLength);

	// Create extra packet data (even if not success, doesn't appear to
	// Do anything at the moment...)
	CreateExtraPacketDataSuccess(&bitStream);

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
	//SavePacket("test_login.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}

// Function to easily create extra packet data and write it to bitStream
void CreateExtraPacketData(ulong stampId, signed long bracketNum, ulong afterNum, RakNet::BitStream *bitStream) {
	ulong zeroPacket = 0;

	bitStream->Write(stampId);
	bitStream->Write(bracketNum);
	bitStream->Write(afterNum);
	bitStream->Write(zeroPacket);
}

// Add extra packet data to the server
void CreateExtraPacketDataSuccess(RakNet::BitStream *bitStream) {
	CreateExtraPacketData(0, 0, 2803442767, bitStream);
	CreateExtraPacketData(7, 37381, 2803442767, bitStream);
	CreateExtraPacketData(8, 6, 2803442767, bitStream);
	CreateExtraPacketData(9, 0, 2803442767, bitStream);
	CreateExtraPacketData(10, 0, 2803442767, bitStream);
	CreateExtraPacketData(11, 1, 2803442767, bitStream);
	CreateExtraPacketData(14, 1, 2803442767, bitStream);
	CreateExtraPacketData(15, 0, 2803442767, bitStream);
	CreateExtraPacketData(17, 1, 2803442767, bitStream);
	CreateExtraPacketData(5, 0, 2803442767, bitStream);
	CreateExtraPacketData(6, 1, 2803442767, bitStream);
	CreateExtraPacketData(20, 1, 2803442767, bitStream);
	CreateExtraPacketData(19, 30854, 2803442767, bitStream);
	CreateExtraPacketData(21, 0, 2803442767, bitStream);
	CreateExtraPacketData(22, 0, 2803442767, bitStream);
	CreateExtraPacketData(23, 4114, 2803442767, bitStream);
	CreateExtraPacketData(27, 4114, 2803442767, bitStream);
	CreateExtraPacketData(28, 1, 2803442767, bitStream);
	CreateExtraPacketData(29, 0, 2803442767, bitStream);
	CreateExtraPacketData(30, 30854, 2803442767, bitStream);
}

