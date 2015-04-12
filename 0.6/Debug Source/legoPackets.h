#pragma once

#include "RakNet\MessageIdentifiers.h"

// ----- DO NOT USE BELOW IDs - LOOK BELOW FOR NEWER IDs TO USE ----- //

// Put all packet specific information here like structs, enums, etc..
// We don't know the real names but that doesn't matter as long as they serve their purpose

// "main" ID, first byte of the packet data

// This is the enum that corresponds the LEGO Universe packet identifiers
// With their values, giving the values names
enum{
	ID_LEGO_PACKET = ID_USER_PACKET_ENUM,	// all lego packets that don't do RakNet stuff have this main ID
	LUNI_INITAW = 0, // LU Initializtion
	LUNI_LOGIN = 1, // LU Login
	LUNI_CHARACTER_CHARSDATA = 1, // Character Data

	LUNI_CHARACTER = 4, // Character
	LUNI_CHARACTER_CREATEDNEW = 3, // Create New Character
	LUNI_CHARACTER_DELETED = 6, // Delete Character

	LUNI_WORLD_IN = 19, // Load World (I believe)
	LUNI_WORLD_CHAT = 25 // Chat message
	// other unknown ID's didn't appear (yet?)
};

// second and fourth byte seem to serve as ID's as well, see inside server loop for more comments

// ----- THESE IDS BELOW SHOULD BE USED INSTEAD OF THE ABOVE ----- //
// ----- NOTE: IF YOU ADD TO THIS, USE NAMES FROM packet_structs BY HUMANOID AND LCDR ----- //

// RemoteConnection IDs
enum RemoteConnection : uchar {
	GENERAL = 0,
	AUTH,
	CHAT,
	UNKNOWNCONN,
	SERVER
};
// IDs for all servers
enum ServerPacketID : uchar {
	VERSION_CONFIRM = 0,
	DISCONNECT_NOTIFY,
	GENERAL_NOTIFY,
};

// IDs for Auth Server
enum AuthPacketID : uchar {
	LOGIN_REQUEST = 0,
	LOGOUT_REQUEST,
	CREATE_NEW_ACCOUNT_REQUEST,
	LEGOINTERFACE_AUTH_RESPONSE,
	SESSIONKEY_RECIEVED_CONFIRM,
	RUNTIME_CONFIG
};

// IDs for Chat Server
enum ChatPacketID : uchar {
	// IDs for Chat
	LOGIN_SESSION_NOTIFY = 0,
	GENERAL_CHAT_MESSAGE,
	PRIVATE_CHAT_MESSAGE
};

// IDs for World Server
enum WorldPacketID : uchar {
	CLIENT_VALIDATION = 1,
	CLIENT_CHARACTER_LIST_REQUEST,
	CLIENT_CHARACTER_CREATE_REQUEST,
	CLIENT_LOGIN_REQUEST,
	CLIENT_GAME_MSG,
	CLIENT_CHARACTER_DELETE_REQUEST,
	CLIENT_CHARACTER_RENAME_REQUEST,
	CLIENT_HAPPY_FLOWER_MODE_NOTIFY,
	CLIENT_SLASH_RELOAD_MAP,
	CLIENT_SLASH_PUSH_MAP_REQUEST,
	CLIENT_SLASH_PUSH_MAP,
	CLIENT_SLASH_PULL_MAP,
	CLIENT_LOCK_MAP_REQUEST,
	CLIENT_GENERAL_CHAT_MESSAGE,
	CLIENT_HTTP_MONITOR_INFO_REQUEST,
	CLIENT_SLASH_DEBUG_SCRIPTS,
	CLIENT_MODELS_CLEAR,
	CLIENT_EXIBIT_INSERT_MODEL,
	CLIENT_LEVEL_LOAD_COMPLETE
};