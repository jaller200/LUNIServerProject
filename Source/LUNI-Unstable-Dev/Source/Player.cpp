#include "Player.h"
#include "serverLoop.h"

#include "RakNet\RakPeerInterface.h"
#include "RakNet\RakNetworkFactory.h"
#include "RakNet\BitStream.h"
#include "RakNet\MessageIdentifiers.h"
#include "RakNet\NetworkIDManager.h"
#include "RakNet\RakSleep.h"
#include "RakNet\RakAssert.h"

DataStructures::List<Player*> Player::players;
Player *Player::myPlayer;

extern NetworkIDManager networkIdManager;
extern RakPeerInterface *rakServer;
extern ReplicaManager2 replicaManager;

Player::Player()
{
	players.Insert(this);
	systemAddress = UNASSIGNED_SYSTEM_ADDRESS;
}

Player::~Player()
{
	players.RemoveAtIndex(players.GetIndexOf(this));

	printf("User with address %s deleted.\n", systemAddress.ToString());
}

void Player::DeletePlayerByAddress(SystemAddress systemAddress)
{
	for (unsigned i = 0; i < players.Size(); i++) {
		if (players[i]->systemAddress == systemAddress) {
			players[i]->BroadcastDestruction();

			delete players[i];
			return;
		}
	}
}

Player* Player::GetPlayerByAddress(SystemAddress systeamAddress)
{
	for (unsigned i = 0; i < players.Size(); i++) {
		if (players[i]->systemAddress == systeamAddress) {
			return players[i];
		}
	}

	return 0;
}

bool Player::SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	SavePacket("TestReplicaPlayer.bin", (char*)bitStream->GetData(), bitStream->GetNumberOfBytesUsed());

	return true;
}

bool Player::Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
{
	//bitStream->Write(systemAddress);
	bitStream->Write(UNASSIGNED_NETWORK_ID);
	bitStream->Write(LOT);

	return true;
}

void Player::Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
{
	NetworkID playerNetworkId;
	unsigned int playerLOT;
	//bitStream->Read(systemAddress);
	bitStream->Read(playerNetworkId);
	bitStream->Read(playerLOT);

	//if (systemAddress == rakServer->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS))
		Player::myPlayer = this;

}

Replica2* LUNIServerConnection::Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
{
	if (networkIDCollision) return 0;

	NetworkID playerNetworkID;
	unsigned int playerLOT;
	SystemAddress playerSystemAddr;

	//replicaData->Read(playerSystemAddr);
	replicaData->Read(playerNetworkID);
	replicaData->Read(playerLOT);

	SavePacket("TestReplicaPlayerConstruct.bin", (char*)replicaData->GetData(), replicaData->GetNumberOfBytesUsed());

	if (playerLOT == 1) {
		return new Player;
	}
}