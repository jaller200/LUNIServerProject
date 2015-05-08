#pragma once
#define LUNI_NO_AUTH //no user authetication with mysql

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
	char redirectIp[16];// an ipv4 address can only occupy up to 15 bytes, do we assume a 16th byte as end char?
	short redirectPort;	// ports occupy 2 bytes
	short listenPort;

	bool useEncryption;
	bool logFile;

	CONNECT_INFO() { for (uint i = 0; i < 16; i++) redirectIp[i] = 0; }
};

std::string RawDataToString(uchar* data, uint size, bool onlyraw = false);
void QuitError(const std::string& msg = "");