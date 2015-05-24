#pragma once

#include "SUtil\Ref.h"

#include "serverLoop.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\BitStream.h"

using namespace std;

// This is an enum that will store game message IDs
enum ClientGameMessageID : ushort {
	CLIENT_CHAT_PARSE = 850, // Request to parse a chat message
	CLIENT_TOGGLE_MAIL = 364 // Command to toggle mail
};

enum ServerGameMessageID : ushort {
	SERVER_CHAT_PARSE = 858 // Handle a chat message
};

// Game Message Functions
void HandleGameMessage(RakPeerInterface *rakServer, SystemAddress &systemAddr, ushort gameMsg, uchar *packetData, uint length, Ref<User> user);

// Parse a chat message
void ParseChatCommand(RakPeerInterface *rakServer, SystemAddress &systemAddr, uchar *packetData, uint length, Ref<User> user);
void ToggleMail(RakPeerInterface *rakServer, SystemAddress &systemAddr, uchar *packetData);
