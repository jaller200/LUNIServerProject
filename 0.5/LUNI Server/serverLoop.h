#pragma once

#include "Common.h"
#include "User.h"
#include "UsersPool.h"
#include "RakNet\RakNetTypes.h"
#include "RakNet\RakPeerInterface.h"
#include "RakNet\PacketFileLogger.h"
#include "RakNet\StringCompressor.h"
#include "RakNet\BitStream.h"

static bool LUNIterminate = false; //use for terminating threads

void InitSecurity(RakPeerInterface* rakServer, bool useEncryption);

//remove first 8 bytes form packet
uchar* CleanPacket(uchar* packetdata, uint len);
void PrintPacketInfo(Packet* packet, PacketFileLogger* msgFileHandler);
void ServerSendPacket(RakPeerInterface* rakServer, char* data, uint len, const SystemAddress& addres);
void ServerSendPacket(RakPeerInterface* rakServer, const std::vector<uchar>& msg, const SystemAddress& addres);
std::vector<uchar> OpenPacket(const std::string& filename);
void SavePacket(const std::string& filename, const std::vector<uchar>& v);
void SavePacket(const std::string& filename, char* data, uint size);

void AuthLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< std::string > > OutputQueue);
void CharactersLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< std::string > > OutputQueue);
void WorldLoop(CONNECT_INFO* cfg, Ref< UsersPool > OnlineUsers, Ref< CrossThreadQueue< std::string > > OutputQueue);