#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

#include "SUtil\Common.h"
#include "SUtil\Ref.h"
#include "SUtil\CrossThreadQueue.h"

struct CONNECT_INFO {
	char redirectIp[16];// AnIPv4 address can only occupy up to 15 bytes, do we assume a 16th byte as end char?
	short redirectPort;	// Ports occupy 2 bytes - This is the redirect port
	short listenPort; // Port to listen on

	bool useEncryption; // Should the server use encryption or not
	bool logFile; // Should the server log a file with its information

	// CONNECT_INFO initialization - sets the redirectIp to all 0s.
	CONNECT_INFO() { for (uint i = 0; i < 16; i++) redirectIp[i] = 0; }
};

// Convert raw hex data to string
std::string RawDataToString(uchar* data, uint size, bool onlyraw = false);

// Error message
void QuitError(const std::string& msg = "");