#pragma once

#include "Common.h"
#include "User.h"
#include "UsersPool.h"
#include "CharactersPool.h"

#include "RakNet\RakNetTypes.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\StringCompressor.h"
#include "RakNet\BitStream.h"

static bool LUNIterminate = false; // Use for terminating threads

// Initialize RakNet security
void InitSecurity(RakPeerInterface* rakServer, bool useEncryption);

// Remove first 8 bytes form packet
uchar* CleanPacket(uchar* packetdata, uint len);

// Print packet information
void PrintPacketInfo(Packet* packet, PacketFileLogger* msgFileHandler);

// These two methods are used to send a packet to the client
void ServerSendPacket(RakPeerInterface* rakServer, char* data, uint len, const SystemAddress& addres);
void ServerSendPacket(RakPeerInterface* rakServer, const std::vector<uchar>& msg, const SystemAddress& addres);

// Open a specific packet
std::vector<uchar> OpenPacket(const std::string& filename);

// Save a packet
void SavePacket(const std::string& filename, const std::vector<uchar>& v);
void SavePacket(const std::string& filename, char* data, uint size);

// Thse are the threads of the Authentication, Character, and World servers
void AuthLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< std::string > > OutputQueue);
void CharactersLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CharactersPool > OnlineCharacters, Ref< CrossThreadQueue< std::string > > OutputQueue);
void WorldLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CharactersPool > OnlineCharacters, Ref< CrossThreadQueue< std::string > > OutputQueue);