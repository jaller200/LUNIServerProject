#pragma once

//#include "serverLoop.h"
#include "Packet.h"
#include "Database.h"

#include "User.h"

using namespace std;

void FetchMailData(RakPeerInterface *rakServer, SystemAddress &systemAddr, Ref< User > user);