#pragma once

#include "Common.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\MessageIdentifiers.h"
#include "RakNet\BitStream.h"

#include <intrin.h>

struct InitPacket {
	ulong versionId; // The game version
	ulong unknown1; // Dunno what this is...
	ulong remoteConnectionType; // This the remote connection type (1 = auth, 4 = other)
	ulong processId; // The process ID of the server
	ushort unknown2; // Dunno what this is either... it is "FF FF" in hex
	std::string ipString; // The IP string of the server (will be changed programmatically)

	InitPacket() {}

	InitPacket(bool isAuth) {
		// Set the variables
		versionId = 171022;
		unknown1 = 147;
		remoteConnectionType = isAuth ? 1 : 4; // Make sure to set this!!!! Determines whether remoteConnectionType should be 1 or 4
		processId = 5136;
		unknown2 = 65535;
		ipString = "172.24.8.139.un i = 15.„–<.„–<.Àˆ";
	}
};

// This is just a function that writes strings to a RakNet::BitStream
void WriteStringToBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *output);
void DecompressStringFromBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *input);

// Convert a std::string to std::wstring
std::wstring StringtoWString(std::string &s);
std::wstring StringToWString(std::string& s, wchar_t size);

// Create a packet header that is used for almost all server packets
void CreatePacketHeader(MessageID messageId, ushort connectionType, ulong internalPacketId, RakNet::BitStream *output);

// Create the init packet known as init_aw2.bin
void SendInitPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, bool isAuth);

// This will convert ushort, ulong, and ulonglong to little endian (or back to big endian... does byte swap)
ushort SwapUShort(ushort number);
ulong SwapULong(ulong number);
ulonglong SwapULongLong(ulonglong number);