#include "Packet.h"
#include "serverLoop.h"

#include <comdef.h>
#include <climits>

using namespace RakNet;

void CreatePacketHeader(MessageID messageId, ushort connectionType, ulong internalPacketId, BitStream *output) {
	uchar unknown1 = 0; // This is an unknown uchar
	
	// Write data to provided BitStream
	output->Write(messageId);
	output->Write(connectionType);
	output->Write(internalPacketId);
	output->Write(unknown1);
}

void SendInitPacket(RakPeerInterface *rakServer, const SystemAddress& systemAddress, bool isAuth) {
	// Initialize the BitStream
	RakNet::BitStream bitStream;

	// Always create the packet header
	CreatePacketHeader(ID_USER_PACKET_ENUM, 0, 0, &bitStream);

	// Create the InitPacket from the InitPacket struct
	InitPacket charInitPacket = InitPacket(isAuth);

	// Write the char init packet to the bitstream
	bitStream.Write((char*)&charInitPacket, sizeof(charInitPacket));

	// Send the BitStream to the client
	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

void WriteStringToBitStream(const char *myString, int stringSize, int maxChars, RakNet::BitStream *output) {
	// Write the string to provided BitStream along with the size
	output->Write(myString, stringSize);

	// Check to see if there are any bytes remaining according to user
	// specification
	int remaining = maxChars - stringSize;

	// If so, fill with 0x00
	for (int i = 0; i < remaining; i++) {
		uchar zero = 0;

		output->Write(zero);
	}
}

void DecompressStringFromBitStream(const char *myString, int maxChars, RakNet::BitStream *input) {
	// Does nothing write now... To be added later
}

std::wstring StringToWString(std::string& s) {
	// Convert the std::string &s into a wstring provided in the arguments
	std::wstringstream tempStream;
	tempStream << s.c_str();

	return tempStream.str();
}

std::wstring StringToWString(std::string& s, wchar_t size) {
	// Convert the std::string &s into a wstring provided in the arguments
	std::wstringstream tempStream;
	tempStream << s.c_str();

	std::wstring tempWString = tempStream.str();

	// Now resize the string to the specified size
	tempWString.resize(size);

	return tempWString;
}

// ----- NOTE: THESE SHOULD NOT BE USED RIGHT NOW AS WINDOWS ALREADY WRITES IN LITTLE ENDIAN ----- //

// Swap a ushort between little and big endian
ushort SwapUShort(ushort number) {
	ushort swappedNum = _byteswap_ushort(number);
	return swappedNum;
}

ulong SwapULong(ulong number) {
	ulong swappedNum = _byteswap_ulong(number);
	return swappedNum;
}

ulonglong SwapULongLong(ulonglong number) {
	unsigned __int64 numberToSwap = (unsigned __int64)number;
	unsigned __int64 swappedNum = _byteswap_uint64(numberToSwap);

	return (ulonglong)swappedNum;
}