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
std::wstring StringToWString(std::string &s);
std::wstring StringToWString(std::string& s, wchar_t size);

// Create a packet header that is used for almost all server packets
void CreatePacketHeader(MessageID messageId, ushort connectionType, ulong internalPacketId, RakNet::BitStream *output);

// Create the init packet known as init_aw2.bin
void SendInitPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, bool isAuth);

// This will convert ushort, ulong, and ulonglong to little endian (or back to big endian... does byte swap)
ushort SwapUShort(ushort number);
ulong SwapULong(ulong number);
ulonglong SwapULongLong(ulonglong number);

// For writing LDF data
// void WriteLDFData(std::wstring key, uchar dataId, T data, RakNet::BitStream *dataStream);

template <typename T>
void WriteLDFData(std::wstring key, uchar dataId, T data, RakNet::BitStream *dataStream)
{
	uchar keyLength = key.length() * 2 + 2;

	// LDF data is usually compressed... Keeping these
	// for instance where it is not
	dataStream->Write(keyLength);
	dataStream->Write((char*)key.data(), sizeof(wchar_t) * key.size() / 2);
	dataStream->Write(dataId);

	if (dataId == 0) {
		long stringLength = sizeof(data) * 2 + 2;
		dataStream->Write(stringLength);
		dataStream->Write(data);
	}
	else {
		dataStream->Write(data);
	}
}

void ReadCompressedLDFData();

// NOTE: LDF Key IDs as follows:
// 0 - std::string
// 1 - signed long (s32)
// 2 - ???
// 3 - float (32-bit, signed)
// 4 - ???
// 5 - unsigned long (u32)
// 6 - ???
// 7 - boolean
// 8 - signed long long (s64)
// 9 - NetworkID (8 bytes)(?)
// 10 - ???
// 11 - ???
// 12 - ???
// 13 - XML data