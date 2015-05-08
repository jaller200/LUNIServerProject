#pragma once

#include "Common.h"
#include "Packet.h"
#include "User.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\MessageIdentifiers.h"
#include "RakNet\BitStream.h"

using namespace std;

struct LoginStatusPacket {
	uchar loginStatus; // This is the Connection ID
	string talkLikeAPirate; // Always is "Talk_Like_A_Pirate", yet 33 bytes long
	string unknownString; // Have NO idea what this is... Is it relevant?

	ushort clientVersion1; // For some reason, the client version
	ushort clientVersion2; // is split up over 3 unsigned shorts
	ushort clientVersion3; // I believe...

	string unknown; // This is 237 bytes long...

	string userKey; // Generated User Key - Should be wstring
	string redirectIp; // Redirect IP Address
	string chatIp; // Chat IP Address
	ushort redirectPort; // Redirect Port
	ushort chatPort; // Chat Port
	string anotherIp; // Another IP Address? 33 bytes long
	string possibleGuid; // In the form of a UUID (maybe a GUID?)
	ushort zeroShort; // Always 0 for some reason...
	uchar localizationChar[3]; // Can be IT, US, etc... Always seems to end in 0
	uchar firstLoginSubscription; // Check - Is this the first login after subscribing? 
	uchar subscribed; // 0 if subscribed, 1 if not subscribed (for LUNI, always 0)
	ulonglong zeroLong; // This is all zeros...
	ushort errorMsgLength; // This is the length of the error msg
	string errorMsg; // This is the error message displayed if connection ID is 0x05. Can be X bytes long (I believe) - Should be wstring
	
	// Extra bytes
	ushort extraBytesLength; // This is the number of bytes left (number of each chunk of extra data = 16 bytes * x chunks + these 4 bytes

	// Initializer
	LoginStatusPacket() { }
};

// Sends a packet with the login status to the client (good, bad, etc..)
void SendStatusPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, UserSuccess loginStatus, string redirectIpAddress, ushort redirectPort, ushort extraDataLength);

// This extra packet data occurs in auth packets.
void CreateExtraPacketData(ulong stampId, signed long bracketNum, ulong afterNum, RakNet::BitStream *bitStream);

// Create the Extra Packet data based on success packet (login_ok.bin)
void CreateExtraPacketDataSuccess(RakNet::BitStream *bitStream);
