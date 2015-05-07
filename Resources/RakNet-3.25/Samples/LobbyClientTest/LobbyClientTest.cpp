#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include "RakSleep.h"
#include <stdlib.h> // For atoi
#include <cstring> // For strlen
#include <stdio.h>
#include <string.h>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "LobbyClientInterface.h"
// Database specifications of input data, passed to some functions in the Lobby Client
#include "LobbyDBSpec.h"
#include "RankingServerDBSpec.h"
#include "TitleValidationDBSpec.h"
#include "EpochTimeToString.h"

#ifdef _CONSOLE_1
#elif defined (_CONSOLE_2)
#else
#include "LobbyClientPC.h"
#endif

bool quit;

using namespace RakNet;

void PrintDownloadClansResult( LobbyDBSpec::GetClans_Data *callResult );

class MyLobbyCallbacks : public LobbyClientInterfaceCB
{
	/// --- FUNCTION CALL RESULTS ---
	virtual void Login_Result(LobbyClientCBResult result){printf("Login_Result: "); result.DebugPrintf();};
	virtual void SetTitleLoginId_Result(LobbyClientCBResult result){printf("SetTitleLoginId_Result: "); result.DebugPrintf();};
	virtual void Logoff_Result(LobbyClientCBResult result){printf("Logoff_Result: "); result.DebugPrintf();};
	virtual void RegisterAccount_Result(LobbyClientCBResult result){printf("RegisterAccount_Result: "); result.DebugPrintf();};
	virtual void UpdateAccount_Result(LobbyClientCBResult result){printf("UpdateAccount_Result: "); result.DebugPrintf();};
	virtual void ChangeHandle_Result(LobbyClientCBResult result){printf("ChangeHandle_Result: "); result.DebugPrintf();};
	virtual void RetrievePasswordRecoveryQuestion_Result(LobbyClientCBResult result){printf("RetrievePasswordRecoveryQuestion_Result: "); result.DebugPrintf();};
	virtual void RetrievePassword_Result(LobbyClientCBResult result){printf("RetrievePassword_Result: "); result.DebugPrintf();};
	virtual void DownloadFriends_Result(LobbyClientCBResult result){printf("DownloadFriends_Result: "); result.DebugPrintf();};
	virtual void SendAddFriendInvite_Result(LobbyClientCBResult result, const char *friendHandle, LobbyClientUserId *friendId){printf("SendAddFriendInvite_Result: "); result.DebugPrintf();};
	virtual void AcceptAddFriendInvite_Result(LobbyClientCBResult result, const char *friendHandle, LobbyClientUserId *friendId){printf("AcceptAddFriendInvite_Result: "); result.DebugPrintf();};
	virtual void DeclineAddFriendInvite_Result(LobbyClientCBResult result, const char *friendHandle, LobbyClientUserId *friendId){printf("DeclineAddFriendInvite_Result: "); result.DebugPrintf();};
	virtual void DownloadIgnoreList_Result(LobbyClientCBResult result){printf("DownloadIgnoreList_Result: "); result.DebugPrintf();};
	virtual void AddToOrUpdateIgnoreList_Result(LobbyClientCBResult result){printf("AddToOrUpdateIgnoreList_Result: "); result.DebugPrintf();};
	virtual void RemoveFromIgnoreList_Result(LobbyClientCBResult result){printf("RemoveFromIgnoreList_Result: "); result.DebugPrintf();};
	virtual void DownloadEmails_Result(LobbyClientCBResult result){printf("DownloadEmails_Result: "); result.DebugPrintf();};
	virtual void SendEmail_Result(LobbyClientCBResult result){printf("SendEmail_Result: "); result.DebugPrintf(); /* User should redownload the list after this, if they want the list to update */};
	virtual void DeleteEmail_Result(LobbyClientCBResult result){printf("DeleteEmail_Result: "); result.DebugPrintf();};
	virtual void UpdateEmailStatus_Result(LobbyClientCBResult result){printf("UpdateEmailStatus_Result: "); result.DebugPrintf();};
	virtual void SendIM_Result(LobbyClientCBResult result, const char *userHandle, LobbyClientUserId *userId){printf("SendIM_Result: "); result.DebugPrintf();};
	virtual void CreateRoom_Result(LobbyClientCBResult result){printf("CreateRoom_Result: "); result.DebugPrintf();};
	virtual void DownloadRooms_Result(LobbyClientCBResult result){printf("DownloadRooms_Result: "); result.DebugPrintf();};
	virtual void LeaveRoom_Result(LobbyClientCBResult result){printf("LeaveRoom_Result: "); result.DebugPrintf();};
	virtual void JoinRoom_Result(LobbyClientCBResult result){printf("JoinRoom_Result: "); result.DebugPrintf();};
	virtual void JoinRoomByFilter_Result(LobbyClientCBResult result){printf("JoinRoomByFilter_Result: "); result.DebugPrintf();};
	virtual void RoomChat_Result(LobbyClientCBResult result){printf("RoomChat_Result: "); result.DebugPrintf();};
	virtual void InviteToRoom_Result(LobbyClientCBResult result){printf("InviteToRoom_Result: "); result.DebugPrintf();};
	virtual void SetReadyToPlayStatus_Result(LobbyClientCBResult result){printf("SetReadyToPlayStatus_Result: "); result.DebugPrintf();};
	virtual void KickRoomMember_Result(LobbyClientCBResult result){printf("KickRoomMember_Result: "); result.DebugPrintf();};
	virtual void SetRoomIsHidden_Result(LobbyClientCBResult result){printf("SetRoomIsHidden_Result: "); result.DebugPrintf();};
	virtual void SetRoomAllowSpectators_Result(LobbyClientCBResult result){printf("SetRoomAllowSpectators_Result: "); result.DebugPrintf();};
	virtual void ChangeNumSlots_Result(LobbyClientCBResult result){printf("ChangeNumSlots_Result: "); result.DebugPrintf();};
	virtual void GrantModerator_Result(LobbyClientCBResult result){printf("GrantModerator_Result: "); result.DebugPrintf();};
	virtual void StartGame_Result(LobbyClientCBResult result){printf("StartGame_Result: "); result.DebugPrintf();};
	virtual void SubmitMatch_Result(LobbyClientCBResult result){printf("SubmitMatch_Result: "); result.DebugPrintf();};
	virtual void DownloadRating_Result(LobbyClientCBResult result){printf("DownloadRating_Result: "); result.DebugPrintf();};
	virtual void QuickMatch_Result(LobbyClientCBResult result){printf("QuickMatch_Result: "); result.DebugPrintf();};
	virtual void CancelQuickMatch_Result(LobbyClientCBResult result){printf("CancelQuickMatch_Result: "); result.DebugPrintf();};
	virtual void DownloadActionHistory_Result(LobbyClientCBResult result){printf("DownloadActionHistory_Result: "); result.DebugPrintf();};
	virtual void AddToActionHistory_Result(LobbyClientCBResult result){printf("AddToActionHistory_Result: "); result.DebugPrintf();};
	virtual void CreateClan_Result(LobbyClientCBResult result, LobbyDBSpec::CreateClan_Data *callResult){printf("CreateClan_Result: "); result.DebugPrintf();};
	virtual void ChangeClanHandle_Result(LobbyClientCBResult result, LobbyDBSpec::ChangeClanHandle_Data *callResult){printf("ChangeClanHandle_Result: "); result.DebugPrintf();};
	virtual void LeaveClan_Result(LobbyClientCBResult result, RakNet::RakString clanHandle, LobbyDBSpec::DatabaseKey clanId){printf("LeaveClan_Result: "); result.DebugPrintf();};
	virtual void DownloadClans_Result(LobbyClientCBResult result, LobbyDBSpec::GetClans_Data *callResult){printf("DownloadClans_Result: \n");};
	virtual void SendClanJoinInvitation_Result(LobbyClientCBResult result, LobbyDBSpec::UpdateClanMember_Data *callResult){printf("SendClanJoinInvitation_Result: ");};
	virtual void WithdrawClanJoinInvitation_Result(LobbyClientCBResult result, RakNet::RakString clanHandle, ClanId *clanId, RakNet::RakString userHandle, LobbyClientUserId *userId){printf("WithdrawClanJoinInvitation_Result: ");};
	virtual void AcceptClanJoinInvitation_Result(LobbyClientCBResult result, LobbyDBSpec::UpdateClanMember_Data *callResult){printf("AcceptClanJoinInvitation_Result: ");};
	virtual void RejectClanJoinInvitation_Result(LobbyClientCBResult result, const char *clanHandle, ClanId *clanId){printf("RejectClanJoinInvitation_Result: ");};
	virtual void DownloadByClanMemberStatus_Result(LobbyClientCBResult result, LobbyDBSpec::ClanMemberStatus status, DataStructures::List<LobbyDBSpec::RowIdOrHandle> &clans){printf("DownloadByClanMemberStatus_Result: ");};
	virtual void SendClanMemberJoinRequest_Result(LobbyClientCBResult result, LobbyDBSpec::UpdateClanMember_Data *callResult){printf("SendClanMemberJoinRequest_Result: ");};
	virtual void WithdrawClanMemberJoinRequest_Result(LobbyClientCBResult result, const char *clanHandle, ClanId *clanId){printf("WithdrawClanMemberJoinRequest_Result: ");};
	virtual void AcceptClanMemberJoinRequest_Result(LobbyClientCBResult result, LobbyDBSpec::UpdateClanMember_Data *callResult){printf("AcceptClanMemberJoinRequest_Result: ");};
	virtual void RejectClanMemberJoinRequest_Result(LobbyClientCBResult result, RakNet::RakString clanHandle, ClanId *clanId, RakNet::RakString userHandle, LobbyClientUserId *userId){printf("RejectClanMemberJoinRequest_Result: ");};
	virtual void SetClanMemberRank_Result(LobbyClientCBResult result){printf("SetClanMemberRank_Result: "); result.DebugPrintf();};
	virtual void ClanKickAndBlacklistUser_Result(LobbyClientCBResult result){printf("ClanKickAndBlacklistUser_Result: "); result.DebugPrintf();};
	virtual void ClanUnblacklistUser_Result(LobbyClientCBResult result){printf("ClanUnblacklistUser_Result: "); result.DebugPrintf();};
	virtual void AddPostToClanBoard_Result(LobbyClientCBResult result){printf("AddPostToClanBoard_Result: "); result.DebugPrintf();};
	virtual void RemovePostFromClanBoard_Result(LobbyClientCBResult result){printf("RemovePostFromClanBoard_Result: "); result.DebugPrintf();};
	virtual void DownloadMyClanBoards_Result(LobbyClientCBResult result){printf("DownloadMyClanBoards_Result: "); result.DebugPrintf();};
	virtual void DownloadClanBoard_Result(LobbyClientCBResult result){printf("DownloadClanBoard_Result: "); result.DebugPrintf();};
	virtual void DownloadClanMember_Result(LobbyClientCBResult result){printf("DownloadClanMember_Result: "); result.DebugPrintf();};
	virtual void ValidateUserKey_Result(LobbyClientCBResult result){printf("ValidateUserKey_Result: "); result.DebugPrintf();};

	/// --- EVENT_NOTIFICATIONS ---
	virtual void ReceiveIM_Notify(const char *senderHandle, const char *body, LobbyClientUserId* senderId)	{
		printf("ReceiveIM (%i, %s): %s\n", *senderId, senderHandle, body);
	};
	virtual void FriendStatus_Notify(const char *friendName, LobbyClientUserId *friendId, FriendStatus status)	{
		printf("FriendStatus_Notify: %s status = ", friendName);
//		if (status==FRIEND_STATUS_NEW_FRIEND) printf("now friends.\n");
//		else 
			if (status==FRIEND_STATUS_NO_LONGER_FRIENDS) printf("no longer friends.\n");
		else if (status==FRIEND_STATUS_FRIEND_ONLINE) printf("online.\n");
		else if (status==FRIEND_STATUS_FRIEND_OFFLINE) printf("offline.\n");
	}
	virtual void IncomingEmail_Notify(LobbyClientUserId senderId, const char *senderHandle, const char *subject){printf("Incoming email from %s (%i), subject=%s\n", senderHandle, senderId, subject);};
	virtual void FriendInvitation_Notify(FriendInvitation_Notification *notification){
		if (notification->request)
			printf("%s has asked to be our friend.\n", notification->invitorHandle);
		else if (notification->accepted)
			printf("%s has accepted our friendship.\n", notification->inviteeHandle);
		else
			printf("%s has rejected our friendship.\n", notification->inviteeHandle);
	};
	virtual void RoomMemberDrop_Notify(RoomMemberDrop_Notification *notification){printf("Room member dropped.\n");};
	virtual void RoomMemberJoin_Notify(RoomMemberJoin_Notification *notification){printf("Room member joined.\n");};
	virtual void RoomMemberReadyStateChange_Notify(RoomMemberReadyStateChange_Notification *notification){
		printf("User %i readyState=%i\n", *notification->member, notification->isReady);};
	virtual void RoomNewModerator_Notify(RoomNewModerator_Notification *notification){printf("New moderator.\n");};
	virtual void RoomSetIsHidden_Notify(RoomSetroomIsHidden_Notification *notification){printf("RoomSetIsHidden_Notify");};
	virtual void RoomSetAllowSpectators_Notify(RoomSetAllowSpectators_Notification *notification){printf("RoomSetAllowSpectators_Notify");};
	virtual void RoomChangeNumSlots_Notify(RoomChangeNumSlots_Notification *notification){printf("RoomChangeNumSlots_Notify");};
	virtual void ChatMessage_Notify(LobbyClientUserId *senderId, const char *senderHandle, const char *message,
		void *binaryData,unsigned int binaryDataLength){printf("%s: %s\n", senderHandle, message);}
	virtual void RoomInvite_Notify(LobbyClientUserId *senderId, const char *senderHandle, const char *message, void *binaryData,
		unsigned int binaryDataLength, LobbyClientRoomId *roomId, const char *roomName, bool privateSlotInvitation){
			printf("%s: %s invited to room %s (%i)\n", senderHandle, message, roomName, *roomId);}
	virtual void RoomDestroyed_Notify(void){printf("Room destroyed. Going back to lobby.\n");};
	virtual void KickedOutOfRoom_Notify(KickedOutOfRoom_Notification *notification){
		printf("Kicked out of room by %s (%i)\n", notification->memberHandle, * notification->member);	};
	virtual void QuickMatchTimeout_Notify(QuickMatchTimeout_Notification *notification){printf("Quick match timed out. Back in lobby.\n");};
	virtual void StartGame_Notify(StartGame_Notification *notification){
		printf("Game should be started now.\n");}
	virtual void ClanDissolved_Notify(ClanDissolved_Notification *notification){printf("Clan %s dissolved\n", notification->clanHandle.C_String());};
	virtual void LeaveClan_Notify(LeaveClan_Notification *notification){printf("LeaveClan_Notify");};
	virtual void ClanJoinInvitationWithdrawn_Notify(ClanJoinInvitationWithdrawn_Notification *notification){printf("ClanJoinInvitationWithdrawn_Notify\n");};
	virtual void ClanJoinInvitationRejected_Notify(ClanJoinInvitationRejected_Notification *notification){printf("ClanJoinInvitationRejected_Notify\n");};
	virtual void ClanJoinInvitationAccepted_Notify(ClanJoinInvitationAccepted_Notification *notification){printf("ClanJoinInvitationAccepted_Notify\n");};
	virtual void ClanJoinInvitation_Notify(ClanJoinInvitation_Notification *notification){printf("ClanJoinInvitation_Notify\n");};
	virtual void ClanMemberJoinRequestWithdrawn_Notify(ClanMemberJoinRequestWithdrawn_Notification *notification){printf("ClanMemberJoinRequestWithdrawn_Notify\n");};
	virtual void ClanMemberJoinRequestRejected_Notify(ClanMemberJoinRequestRejected_Notification *notification){printf("ClanMemberJoinRequestRejected_Notify\n");};
	virtual void ClanMemberJoinRequestAccepted_Notify(ClanMemberJoinRequestAccepted_Notification *notification){printf("ClanMemberJoinRequestAccepted_Notify\n");};
	virtual void ClanMemberJoinRequest_Notify(ClanMemberJoinRequest_Notification *notification){printf("ClanMemberJoinRequest_Notify\n");};
	virtual void ClanMemberRank_Notify(ClanMemberRank_Notification *notification){printf("ClanMemberRank_Notify\n");};
	virtual void ClanMemberKicked_Notify(ClanMemberKicked_Notification *notification){printf("ClanMemberKicked_Notify\n");};
};

int main(void)
{
	RakPeerInterface *rakPeer=RakNetworkFactory::GetRakPeerInterface();
	char inputStr[512];
	SocketDescriptor socketDescriptor(0,0);
	if (rakPeer->Startup(1,30,&socketDescriptor, 1)==false)
	{
		printf("Start call failed!\n");
		return 0;
	}
	
	printf("Console based lobby client test (PC)\n");
	printf("Difficulty: Intermediate\n\n");

	// PC version of the lobby client (all you need unless you are doing consoles)
	LobbyClientPC lobbyClient;
	// LobbyClientPC is a plugin so it updates and checks messages automatically when you call RakPeer::Receive()
	rakPeer->AttachPlugin(&lobbyClient);

	// Setup that class we defined above to get the notification callbacks.
	// This is necessary as most operations are asynchronous
	MyLobbyCallbacks callbacks;
	lobbyClient.SetCallbackInterface(&callbacks);

	// In this sample we only connect to the lobby server.
	// This is set by the first system we connect to
	SystemAddress serverLobbyAddress = UNASSIGNED_SYSTEM_ADDRESS;

	unsigned short port;
	char str[512];
	printf("Enter port of lobby server: ");
	gets(str);
	if (str[0]!=0)
		port = atoi(str);
	else
		port=60000; // Arbitrary, Hardcoded into LobbyServerTest.cpp
	printf("Enter IP address of lobby server: ");
	gets(str);
	if (str[0]==0)
		strcpy(str, "127.0.0.1"); // Loopback
	SocketDescriptor sd(0,0); // Autochoose local port
	// Start RakNet
	rakPeer->Startup(1,100,&sd, 1);
	// Connect to the server. If and when we get back ID_CONNECTION_REQUEST_ACCEPTED we will login
	rakPeer->Connect(str,port,0,0,0);	


	// TODO - Clans database
	char ch;
	printf("Commands:\n"
		"(~) Quit\n"
		"(a) Set TitleId\n"
		"(b) Register an account\n"
		"(c) Login\n"
		"(d) Logoff\n"
		"(e) Retrieve password recovery question from handle\n"
		"(f) Retrieve password from password recovery question\n"
		"(g) Update account\n"
		"(h) Change handle\n"
		"(i) Download friends\n"
		"(j) Send add friend invite\n"
		"(k) Accept add friend invite\n"
		"(l) Decline add friend invite\n"
		"(m) Get friends list (print them)\n"
		"(n) Oops skipped this letter :)\n"
		"(o) Download ignore list\n"
		"(p) Add to or update ignore list\n"
		"(q) Remove from ignore list\n"
		"(r) Get ignore list (print it)\n"
		"(s) Download emails\n"
		"(t) Send email\n"
		"(u) Delete email\n"
		"(v) Update email status\n"
		"(w) Read emails (print it)\n"
		"(x) Send IM\n"
		"(y) Create room\n"
		"(z) Download rooms\n"
		"(0) Get All rooms (print them)\n"
		"(1) Leave room\n"
		"(2) Join room\n"
		"(3) Room chat\n"
		"(4) Room invite\n"
		"(5) Set ready to play status\n"
		"(6) Kick room member\n"
		"(7) Set room invite only\n"
		"(8) Set room allow spectators\n"
		"(9) Change room slots\n"
		"(!) Grant moderator\n"
		"(@) Start game\n"
		"(#) Quick match\n"
		"($) Cancel Quick match\n"
		"(%%) Join room by filter\n"
		"(^) Download action history\n"
		"(&) Add to action history\n"
		"(*) Get action history (print it)\n"
		"(() Print recently met users (client-side only)\n"
		"()) Submit match\n"
		"(-) Get ranking\n"
		"(=) Print rating\n"
		"DO NOT USE CLANS: STILL IN PROGRESS\n"
		"(A) Create clan\n"
		"(B) Change clan handle\n"
		"(C) Leave clan\n"
		"(D) Download clans\n"
		"(E) Send clan join invitation\n"
		"(F) Withdraw clan join invitation\n"
		"(G) Accept clan join invitation\n"
		"(H) Reject clan join invitation\n"
		"(I) Download by clan member status\n"
		"(J) Send clan join request\n"
		"(K) Withdraw clan join request\n"
		"(L) Accept clan join request\n"
		"(M) Reject clan join request\n"
		"(N) Set clan member rank\n"
		"(O) Kick and blacklist user\n"
		"(P) Unblacklist user\n"
		"(Q) Add post to clan board\n"
		"(R) Remove post from clan board\n"
		"(S) Download my clan boards\n"
		"(T) Download clan board\n"
		"(U) Download clan member\n"
);

	bool quit=false;
	Packet *packet;
	while (quit==false)
	{
		if (kbhit())
		{
			ch=getch();
			if (ch!='\n' && ch!='\r')
				printf("Command: (%c) ", ch);
			switch (ch)
			{
				case '~':
				{
					printf("Quitting...\n");
					quit=true;
					break;
				}

				case 'a':
					{
						// Set title ID
						char titleIdStr[512];
						char titlePw[512];
						printf("Set title ID.\n");
						printf("Enter title name (string): ");
						gets(titleIdStr);
						if (titleIdStr[0]==0) strcpy(titleIdStr, "Hangman EXTREME!");

						// For games in which users can host their own servers, the password is not secret so is just an identifier for the game.
						// For games where users cannot host their own servers, the password should be kept secure so that ranking can be uploaded accurately.
						printf("Enter title identifier / password.\nThis should have been setup on the database beforehand.\nSee the LobbyServerTest sample.\n(binary, entered here as string): ");
						gets(titlePw);
						if (titlePw[0]==0) strcpy(titlePw, "SECRET_PER_GAME_LOGIN_PW_PREVIOUSLY_SETUP_ON_THE_DB");
						int titleLoginPwLength = (int) strlen(titlePw);
						lobbyClient.SetTitleLoginId(titleIdStr,titlePw,titleLoginPwLength);

						break;
					}

				case 'b':
					{
						// Register
						printf("Registering a new account.\n");
						LobbyDBSpec::CreateUser_Data accountData;

						// These two fields are mandatory
						printf("Enter what handle you want to use: ");
						gets(str);
						if (str[0]==0) strcpy(str, "Rak'kar the great");
						accountData.handle=str;
						printf("Enter your password: ");
						gets(str);
						if (str[0]==0) strcpy(str, "fakePW");
						accountData.password=str;
						// These two are optional, as are all the rest
						printf("Enter your password recovery question: ");
						gets(str);
						if (str[0]==0) strcpy(str, "2+2=?");
						accountData.passwordRecoveryQuestion=str;
						printf("Enter your password recovery answer: ");
						gets(str);
						if (str[0]==0) strcpy(str, "4");
						accountData.passwordRecoveryAnswer=str;
						printf("Enter your email address: ");
						gets(str);
						if (str[0]==0) strcpy(str, "johndoe@fakeEmailAddress.com");
						accountData.emailAddress=str;

						// There are a lot of other fields, we will leave them blank for now to keep the sample clear
						lobbyClient.RegisterAccount(&accountData, serverLobbyAddress);
						break;
					}

				case 'c':
				{
					// Login
					printf("Logging into lobby server.\n");
					char userHandle[512];
					char userPw[512];

					printf("Enter handle (string): ");
					gets(userHandle);
					if (userHandle[0]==0) strcpy(userHandle, "Rak'kar the great");

					printf("Enter user password (string): ");
					gets(userPw);
					if (userPw[0]==0) strcpy(userPw, "fakePW");

					lobbyClient.Login(userHandle,userPw,serverLobbyAddress);

					break;
				}


				case 'd':
				{
					lobbyClient.Logoff();
					break;
				}

				case 'e':
				{
					// Retrieve password recovery question
					printf("Retrieving password recovery question.\n");
					char userHandle[512];

					printf("Enter handle (string): ");
					gets(userHandle);
					if (userHandle[0]==0) strcpy(userHandle, "Rak'kar the great");

					lobbyClient.RetrievePasswordRecoveryQuestion(userHandle, serverLobbyAddress);
					break;
				}

				case 'f':
				{
					// Retrieve password
					printf("Retrieving password.\n");

					char userHandle[512];
					printf("Enter handle (string): ");
					gets(userHandle);
					if (userHandle[0]==0) strcpy(userHandle, "Rak'kar the great");

					char pwRecoveryAnswer[512];
					printf("Enter password recovery answer (string): ");
					gets(pwRecoveryAnswer);
					if (pwRecoveryAnswer[0]==0) strcpy(pwRecoveryAnswer, "4");

					lobbyClient.RetrievePassword(userHandle, pwRecoveryAnswer, serverLobbyAddress);
					break;
				}

				case 'g':
				{
					printf("Updating an account.\n");
					LobbyDBSpec::UpdateUser_Data userData;
					
					userData.updateCCInfo=true;
					userData.updateBinaryData=false;
					userData.updatePersonalInfo=false;
					userData.updateEmailAddr=false;
					userData.updatePassword=false;
					userData.updateCaptions=false;
					userData.updateOtherInfo=false;
					userData.updatePermissions=false;
					userData.updateAccountNumber=true;
					userData.updateAdminLevel=false;
					userData.updateAccountBalance=false;

					char str[512];
					printf("Enter your CCV: ");
					gets(str);
					if (str[0]==0) strcpy(str, "12345");
					userData.input.creditCardCVV = str; // Should fail if defaultAllowUpdateCCInfo==false, or if the title wans't yet set

					printf("Enter your account number: ");
					gets(str);
					if (str[0]==0) strcpy(str, "98765");
					userData.input.accountNumber = atoi(str); // Should fail if defaultAllowUpdateAccountNumber==false, or if the title wans't yet set

					lobbyClient.UpdateAccount(&userData);
					break;
				}

				case 'h':
				{
					printf("Changing handle.\n");
					char str[512];
					printf("Enter handle (string): ");
					gets(str);
					if (str[0]==0) strcpy(str, "My new handle");

					lobbyClient.ChangeHandle(str);
					break;
				}

				case 'i':
				{
					printf("Download friends.\n");
					lobbyClient.DownloadFriends(true);
					break;
				}

				case 'j':
				{
					printf("Send add friend invite.\n");
					char str[512];
					char friendHandle[512];
					printf("Enter invitation text: ");
					gets(str); // Can be blank
					printf("Enter friend handle: ");
					gets(friendHandle);
					if (friendHandle[0]==0) strcpy(friendHandle, "Rak'kar the great");
					lobbyClient.SendAddFriendInvite(friendHandle, 0, str, 0);
					break;
				}

				case 'k':
				{
					printf("Accept add friend invite.\n");
					char str[512];
					char friendIdStr[512];
					printf("Enter acceptance text: ");
					gets(str); // Can be blank
					printf("Enter friend id (integer): ");
					gets(friendIdStr);
					if (friendIdStr[0]==0) strcpy(friendIdStr, "1");
					LobbyClientUserId id;
					id = atoi(friendIdStr);
					lobbyClient.AcceptAddFriendInvite("", &id, str, 0);

					break;
				}

				case 'l':
				{
					printf("Decline add friend invite.\n");

					char str[512];
					char friendIdStr[512];
					printf("Enter declination text: ");
					gets(str); // Can be blank
					printf("Enter friend id (integer): ");
					gets(friendIdStr);
					if (friendIdStr[0]==0) strcpy(friendIdStr, "1");
					LobbyClientUserId id;
					id = atoi(friendIdStr);
					lobbyClient.DeclineAddFriendInvite("", &id, str, 0);

					break;
				}

				case 'm':
				{
					printf("Get friends list (currently downloaded).\n");
					const DataStructures::List<LC_Friend *>& friendsList = lobbyClient.GetFriendsList();
					unsigned i;
					if (friendsList.Size()>0)
					{
						for (i=0; i < friendsList.Size(); i++)
						{
							printf("ID=%i isOnline=%i, Name=%s\n", *(friendsList[i]->id), friendsList[i]->isOnline, friendsList[i]->handle);
						}
					}
					else
					{
						printf("No friends downloaded.\n");
					}
					
					break;
				}

				case 'o':
				{
					printf("Downloading ignore list.\n");
					lobbyClient.DownloadIgnoreList(true);
					break;
				}

				case 'p':
				{
					printf("Add to or update ignore list.\n");

					char actionsStr[512];
					char userHandle[512];
					unsigned int userId;
					printf("Enter ignored user id (integer): ");
					gets(userHandle);
					userId = atoi(userHandle);
					printf("Enter actions string (arbitrary): ");
					gets(actionsStr);
					lobbyClient.AddToOrUpdateIgnoreList(0, &userId, actionsStr);
					break;
				}

				case 'q':
				{
					printf("Remove from ignore list.\n");
					char userHandle[512];
					unsigned int userId;
					printf("Enter ignored user id (integer): ");
					gets(userHandle);
					userId = atoi(userHandle);						
					lobbyClient.RemoveFromIgnoreList(&userId);
					break;

				}

				case 'r':
				{
					printf("Print ignore list (currently downloaded).\n");
					const DataStructures::List<LC_Ignored *>& ignoreList = lobbyClient.GetIgnoreList();
					unsigned i;
					if (ignoreList.Size()>0)
					{
						for (i=0; i < ignoreList.Size(); i++)
						{
							printf("ID=%i ActionString=%s\n", *(ignoreList[i]->id), ignoreList[i]->actionString);
						}
					}
					else
					{
						printf("Either nobody is ignored, or we didn't download the ignored users list yet.\n");
					}
					break;
				}

				case 's':
				{
					char str[256];
					printf("Downloading emails.\n");
					printf("Download from (I)nbox or (S)ent emails?: ");
					gets(str);
					bool inbox=(str[0]=='i');
					printf("Sort by date (A)scending or (D)escending?: ");
					gets(str);
					bool ascendingSort=(str[0]=='a');

					lobbyClient.DownloadEmails(ascendingSort, inbox, 0);
					
					break;
				}

				case 't':
				{
					printf("Sending an email.\n");

					LobbyDBSpec::SendEmail_Data data;
					char inputStr[4096];

					while (1)
					{
						LobbyDBSpec::RowIdOrHandle uioh;

						printf("Enter TO: recipient ID? (Enter to stop) (y/n): ");
						gets(inputStr);
						if (inputStr[0]=='y')
						{
							printf("Enter TO: user ID (integer): ");
							gets(inputStr);
							uioh.hasDatabaseRowId=true;
							uioh.databaseRowId=atoi(inputStr);
						}
						else if (inputStr[0]==0)
							break;
						else
						{
							printf("Enter TO: user handle: ");
							gets(inputStr);
							uioh.hasDatabaseRowId=false;
							uioh.handle=inputStr;
						}

						data.to.Insert(uioh);
					}

					printf("Enter subject: ");
					gets(inputStr);
					data.subject=inputStr;

					printf("Enter body: ");
					gets(inputStr);
					data.body=inputStr;

					// Test attachments
					data.attachment=new char [2];
					data.attachmentLength=2;
					data.attachment[0]='Z';
					data.attachment[1]=0;

					// Emails can be tagged with a status number. Can use for read, priority, etc.
					data.initialSenderStatus=63;
					data.initialRecipientStatus=64;

					// Mark sent email as initially opened.
					// Note: Received mail always marked as not opened until updated with UpdateEmailStatus
					data.wasOpened=true;

					/// Puts this functor on the processing queue. It will process sometime later in a thread.
					lobbyClient.SendEmail(&data, 0);

					// data.attachment automatically deallocated in the destructor of LobbyDBSpec::SendEmail_Data

					break;
				}

				case 'u':
				{
					printf("Deleting an email.\n");
					
					char str[512];
					EmailId emailId;
					printf("Enter email id (integer): ");
					gets(str);
					emailId = atoi(str);						
					lobbyClient.DeleteEmail(&emailId);

					break;
				}

				case 'v':
				{
					printf("Updating email status.\n");
					char str[512];
					EmailId emailId;
					printf("Enter email id (integer): ");
					gets(str);
					emailId = atoi(str);
					int newStatus;
					printf("Enter status (integer): ");
					gets(str);
					newStatus = atoi(str);
					bool wasOpened;
					printf("Mark the email as opened? (y/n): ");
					gets(str);
					if (str[0]=='y')
						wasOpened=true;
					else
						wasOpened=false;
					lobbyClient.UpdateEmailStatus(&emailId, newStatus, wasOpened);

					break;
				}

				case 'w':
				{
					printf("Read emails (print them)\n");

					const DataStructures::List<LobbyDBSpec::SendEmail_Data *> &inbox = lobbyClient.GetInboxEmailList();
					const DataStructures::List<LobbyDBSpec::SendEmail_Data *> &sentEmails = lobbyClient.GetSentEmailList();

					unsigned i;
					printf("INBOX:\n");
					if (inbox.Size()==0)
						printf("<EMPTY> (or none downloaded yet)\n");
					for (i=0; i < inbox.Size(); i++)
					{
						printf("%i. id.userId=%i\n", i+1, inbox[i]->id.databaseRowId);
						printf("%i. id.userHandle=%s\n", i+1, inbox[i]->id.handle.C_String());
						printf("%i. emailMessageID=%i\n", i+1, inbox[i]->emailMessageID);
						printf("%i. subject=%s\n", i+1, inbox[i]->subject.C_String());
						printf("%i. body=%s\n", i+1, inbox[i]->body.C_String());
						printf("%i. creationTime=%s\n", i+1, EpochTimeToString(inbox[i]->creationTime)); // Use EpochTimeToString.h
						printf("%i. status=%i opened=%i\n", i+1, inbox[i]->status, inbox[i]->wasOpened);
						printf("%i. attachmentLength=%i\n", i+1, inbox[i]->attachmentLength);
						printf("\n");
					}

					printf("SENT:\n");
					if (sentEmails.Size()==0)
						printf("<EMPTY> (or none downloaded yet)\n");
					for (i=0; i < sentEmails.Size(); i++)
					{
						printf("%i. id.userId=%i\n", i+1, sentEmails[i]->id.databaseRowId);
						printf("%i. id.userHandle=%s\n", i+1, sentEmails[i]->id.handle.C_String());
						printf("%i. emailMessageID=%i\n", i+1, sentEmails[i]->emailMessageID);
						printf("%i. subject=%s\n", i+1, sentEmails[i]->subject.C_String());
						printf("%i. body=%s\n", i+1, sentEmails[i]->body.C_String());
						printf("%i. creationTime=%s\n", i+1, EpochTimeToString(sentEmails[i]->creationTime)); // Use EpochTimeToString.h
						printf("%i. status=%i opened=%i\n", i+1, sentEmails[i]->status, sentEmails[i]->wasOpened);
						printf("%i. attachmentLength=%i\n", i+1, sentEmails[i]->attachmentLength);
						printf("\n");
					}

					break;
				}

				case 'x':
				{
					printf("Sending IM\n");

					printf("Send IM to user.\n");
					char userHandle[512];
					printf("Enter user handle: ");
					gets(userHandle);
					char message[4096];
					printf("Enter message: ");
					gets(message);
					lobbyClient.SendIM(userHandle, 0, message, 0, 0, 0);

					break;
				}

				case 'y':
				{
					bool roomIsHidden;
					int publicSlots;
					int privateSlots;
					char columnValue[256];
					bool allowSpectators;
					DataStructures::Table customPropertiesTable;
					printf("Create a room\n");
					printf("Enter room name: ");
					char roomName[512];
					gets(roomName);
					if (roomName[0]==0) strcpy(roomName, "Default room name");
					char roomPassword[512];
					printf("Enter room password: ");
					gets(roomPassword);
					char str[256];
					printf("Invite only? (y/n): ");
					gets(str);
					roomIsHidden=(str[0]=='y');
					printf("Number of public slots: ");
					gets(str); if (str[0]==0) strcpy(str, "10");
					publicSlots=atoi(str);
					printf("Number of private slots: ");
					gets(str); if (str[0]==0) strcpy(str, "5");
					privateSlots=atoi(str);
					printf("Allow spectators? (y/n): ");
					gets(str);
					allowSpectators=(str[0]!='n');

					// Just entering one column as a sample, but it can have any number of columns of string, binary, or numerical
					printf("Enter name of custom property column: ");
					gets(str); if (str[0]==0) strcpy(str, "My custom column");
					printf("Enter property value: ");
					gets(columnValue); if (columnValue[0]==0) strcpy(columnValue, "My custom value");
					customPropertiesTable.AddColumn(str, DataStructures::Table::STRING); // Note there is binary and numeric as well.
					DataStructures::Table::Row* row = customPropertiesTable.AddRow(0);
					row->cells[0]->Set(columnValue);

					lobbyClient.CreateRoom(roomName, roomPassword, roomIsHidden, publicSlots, privateSlots, allowSpectators, &customPropertiesTable);

					break;
				}

				case 'z':
				{
					printf("Download rooms\n");

					// Filters are AND
					// Filters are optional
					// Use filter indices if possible, for faster lookup
					DataStructures::Table::FilterQuery inclusionFilters[2];
					DataStructures::Table::Cell filterCells[2];

					// First filter sets only return rooms that allow spectators
					filterCells[0].Set(1); // 1 = integer value true
					inclusionFilters[0].cellValue=&filterCells[0];
					// Default columns are always at specific indices
					inclusionFilters[0].columnIndex=DefaultTableColumns::TC_ALLOW_SPECTATORS;
					inclusionFilters[0].operation=DataStructures::Table::QF_EQUAL;

					// Second filter searches our own custom column, string must match "My custom value"
					filterCells[1].Set("My custom value");
					inclusionFilters[1].cellValue=&filterCells[1];
					// Search by the name of the column
					strncpy(inclusionFilters[1].columnName,"My custom column",_TABLE_MAX_COLUMN_NAME_LENGTH);
					inclusionFilters[1].operation=DataStructures::Table::QF_EQUAL;

					// Download rooms using 2 search filters
					lobbyClient.DownloadRooms(inclusionFilters, 2);

					break;
				}

				case '0':
				{
					DataStructures::Table *table = lobbyClient.GetAllRooms();
					DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = table->GetListHead();
					unsigned i;

					printf("Columns:\n");
					for (i=0; i < table->GetColumns().Size(); i++)
					{
						printf("%i. %s : ", i+1, table->GetColumns()[i].columnName);
						if (table->GetColumns()[i].columnType==DataStructures::Table::BINARY)
							printf("BINARY");
						else if (table->GetColumns()[i].columnType==DataStructures::Table::NUMERIC)
							printf("NUMERIC");
						else if (table->GetColumns()[i].columnType==DataStructures::Table::POINTER)
							printf("POINTER");
						else
							printf("STRING");
						printf("\n");
					}
					if (cur)
						printf("Rows:\n");
					else
						printf("Table has no rows.\n");
					while (cur)
					{
						for (i=0; i < (unsigned)cur->size; i++)
						{
							table->PrintRow(str, 256, ',', true, cur->data[i]);
							printf("RowID %i: %s\n", cur->keys[i], str );
						}
						cur=cur->next;
					}

					break;
				}

				case '1':
				{
					printf("Leave room\n");

					lobbyClient.LeaveRoom();
					break;
				}

				case '2':
				{
					printf("Join room\n");
					unsigned int roomId;
					bool joinAsSpectator;
					char str[256];
					printf("Enter room id (integer): ");
					gets(str); if (str[0]==0) strcpy(str,"0");
					roomId=atoi(str);
					printf("Enter as spectator? (y/n): ");
					gets(str);
					joinAsSpectator=(str[0]=='y');

					lobbyClient.JoinRoom(&roomId, "", 0, joinAsSpectator);
					break;
				}

				case '3':
				{
					printf("Enter chat message: \n");
					char str[4096];
					gets(str);
					lobbyClient.RoomChat(str,0,0,0);
					break;
				}

				case '4':
				{
					char userName[512];
					printf("Enter user to invite: ");
					gets(userName); if (userName[0]==0) strcpy(str,"Rak'kar the great");
					printf("Enter room invite message\n");
					char str[4096];
					gets(str);
					lobbyClient.InviteToRoom(userName,0, str,0,0,0,true);
					break;
				}

				case '5':
				{
					printf("Set ready to play status");
					bool isReady;
					char str[512];
					printf("Ready to play? (y/n)\n");
					gets(str);
					isReady = (str[0]=='y');
					lobbyClient.SetReadyToPlayStatus(isReady);
					break;
				}

				case '6':
				{
					printf("Kick room member\n");

					printf("Enter memberId to kick: ");
					gets(str); if (str[0]==0) strcpy(str,"1");
					LobbyClientUserId id;
					id = atoi(str);
					lobbyClient.KickRoomMember(&id);

					break;
				}

				case '7':
				{
					printf("Set room invite only\n");
					char str[512];
					printf("Invite only? (y/n)\n");
					gets(str);
					bool b = (str[0]=='y');
					lobbyClient.SetRoomIsHidden(b);

					break;
				}

				case '8':
				{
					printf("Set room allow spectators\n");
					char str[512];
					printf("Allow (new) spectators? (y/n)\n");
					gets(str);
					bool b = (str[0]=='y');
					lobbyClient.SetRoomAllowSpectators(b);

					break;
				}

				case '9':
				{
					printf("Change room slot counts\n");

					printf("Enter number of public slots: ");
					gets(str); if (str[0]==0) strcpy(str,"1");
					int publicSlots = atoi(str);

					printf("Enter number of reserved slots: ");
					gets(str); if (str[0]==0) strcpy(str,"1");
					int reservedSlots = atoi(str);

					lobbyClient.ChangeNumSlots(publicSlots, reservedSlots);

					break;
				}

				case '!':
				{
					printf("Grant moderator\n");
					printf("Enter moderator id: ");
					gets(str); if (str[0]==0) strcpy(str,"1");
					LobbyClientUserId id;
					id = atoi(str);
					lobbyClient.GrantModerator(&id);

					break;
				}

				case '@':
				{
					printf("Start game\n");
					lobbyClient.StartGame();

					break;
				}

				case '#':
				{
					printf("Quick match\n");
					printf("Enter required players: \n");
					gets(str); if (str[0]==0) strcpy(str,"2");
					int requiredPlayers = atoi(str);
					printf("Enter timeout in seconds: \n");
					gets(str); if (str[0]==0) strcpy(str,"30");
					int timeout = atoi(str);
					lobbyClient.QuickMatch(requiredPlayers,timeout);

					break;
				}

				case '$':
				{
					printf("Cancel Quick match\n");
					lobbyClient.CancelQuickMatch();
					break;
				}

				case '%':
				{
					printf("Join room by filter\n");

					// Filters are AND
					// Filters are optional
					// Use filter indices if possible, for faster lookup
					DataStructures::Table::FilterQuery inclusionFilters[2];
					DataStructures::Table::Cell filterCells[2];

					// First filter sets only return rooms that allow spectators
					filterCells[0].Set(1); // 1 = integer value true
					inclusionFilters[0].cellValue=&filterCells[0];
					// Default columns are always at specific indices
					inclusionFilters[0].columnIndex=DefaultTableColumns::TC_ALLOW_SPECTATORS;
					inclusionFilters[0].operation=DataStructures::Table::QF_EQUAL;

					// Second filter searches our own custom column, string must match "My custom value"
					filterCells[1].Set("My custom value");
					inclusionFilters[1].cellValue=&filterCells[1];
					// Search by the name of the column
					strncpy(inclusionFilters[1].columnName,"My custom column",_TABLE_MAX_COLUMN_NAME_LENGTH);
					inclusionFilters[1].operation=DataStructures::Table::QF_EQUAL;

					lobbyClient.JoinRoomByFilter(inclusionFilters, 2, false);
					break;
				}

				case '^':
				{
					printf("Download action history\n");
					printf("Sort by date (A)scending or (D)escending?: ");
					gets(str);
					bool ascendingSort=(str[0]=='a');
					lobbyClient.DownloadActionHistory(ascendingSort);
					break;
				}

				case '&':
				{
					printf("Add to action history\n");
					printf("defaultAllowClientsUploadActionHistory must be true in the titles table OR\n");
					printf("Your IP must be in the trustedIpList table\n");

					// history.id is not used from the lobbyClient
					// If you want to update the history for a particular user other than yourself, use AddToActionHistory_PostgreSQLImpl directly on the server.
					LobbyDBSpec::AddToActionHistory_Data history;

					char inputStr[512];
					printf("Enter actionTaken: ");
					gets(inputStr);
					history.actionTaken=inputStr;

					printf("Enter description: ");
					gets(inputStr);
					history.description=inputStr;

					history.actionTime; // Leave blank
					history.sourceIpAddress="127.0.0.1";

					/// Puts this functor on the processing queue. It will process sometime later in a thread.
					lobbyClient.AddToActionHistory(&history);

					break;
				}

				case '*':
				{
					printf("Print action history\n");

					const DataStructures::List<LobbyDBSpec::AddToActionHistory_Data *> &actionHistoryList = lobbyClient.GetActionHistoryList();
					unsigned i;
					if (actionHistoryList.Size()==0)
						printf("None.\n");
					for (i=0; i < actionHistoryList.Size(); i++)
					{
						printf("%i. id.userId=%i\n", i+1, actionHistoryList[i]->id.databaseRowId);
						printf("%i. id.userHandle=%s\n", i+1, actionHistoryList[i]->id.handle.C_String());
						printf("%i. actionTime=%s\n", i+1,actionHistoryList[i]->actionTime.C_String());
						printf("%i. actionTaken=%s\n", i+1, actionHistoryList[i]->actionTaken.C_String());
						printf("%i. description=%s\n", i+1, actionHistoryList[i]->description.C_String());
						printf("%i. sourceIpAddress=%s\n", i+1, actionHistoryList[i]->sourceIpAddress.C_String());
						printf("%i. creationTime=%s\n", i+1, EpochTimeToString(actionHistoryList[i]->creationTime));
					}

					break;
				}

				case '(':
				{
					printf("Print recently met users\n");
					const DataStructures::List<LobbyUserClientDetails *>& recentlyMetUsers = lobbyClient.GetRecentlyMetUsersList();
					unsigned i;
					if (recentlyMetUsers.Size()==0)
						printf("None.\n");
					for (i=0; i < recentlyMetUsers.Size(); i++)
					{
						printf("%i. %s\n", i+1, recentlyMetUsers[i]->userData->handle.C_String());
					}
					break;
				}

				case ')':
				{
					printf("Submit match\n");
					printf("Note: Your IP must be in the trustedIpList table\n");

					RankingServerDBSpec::SubmitMatch_Data *dat = new RankingServerDBSpec::SubmitMatch_Data;

					/// Every participant is represented by a pair of numbers.
					/// The primary ID can be used to reference a database primary key referring to another table holding all participant
					/// The secondary ID can be used to indicate type
					/// such as indicating the type of participant (player, squad, guild, clan, etc).

					printf("Enter the primary ID of participant A (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "0");
					dat->participantADbId.primaryKey = atoi(inputStr);

					printf("Enter the secondary ID of participant A (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "1");
					dat->participantADbId.secondaryKey = atoi(inputStr);

					printf("Enter the primary ID of participant B (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "2");
					dat->participantBDbId.primaryKey = atoi(inputStr);

					printf("Enter the secondary ID of participant B (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "3");
					dat->participantBDbId.secondaryKey = atoi(inputStr);

					// Score is tracked in case you want to check the database for cheaters, or to modify ratings, etc.
					// If you don't care about it, just use any number.
					printf("Enter the score of participant A (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "10.0");
					dat->participantAScore = (float) atof(inputStr);

					printf("Enter the score of participant B (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "21.0");
					dat->participantBScore = (float) atof(inputStr);

					// The rating of the player before the match started. Elo.h/.cpp is provided as one way to calculate ratings
					printf("Enter the pre-match rating of participant A (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "12.0");
					dat->participantAOldRating = (float) atof(inputStr);

					printf("Enter the post-match rating of participant A (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "5.0");
					dat->participantANewRating = (float) atof(inputStr);

					printf("Enter the pre-match rating of participant B (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "20.0");
					dat->participantBOldRating = (float) atof(inputStr);

					printf("Enter the post-match rating of participant B (float): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "30.0");
					dat->participantBNewRating = (float) atof(inputStr);

					/// Games are identified by a pair of integers the same way participants are
					/// The primary key could be the game title (such as Asteroids) while the secondary key could
					/// Indicate a submode (such as cooperative or deathmatch)
					printf("Enter the primary ID of the game (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "12");
					dat->gameDbId.primaryKey = atoi(inputStr);

					printf("Enter the secondary ID of the game (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "13");
					dat->gameDbId.secondaryKey = atoi(inputStr);

					/// A string is stored with each match.
					printf("Enter the match notes: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Default Match Notes");
					dat->matchNotes=inputStr;

					lobbyClient.SubmitMatch(dat);

					break;
				}

				case '-':
				{
					printf("Get rating\n");
					char inputStr[512];
					RankingServerDBSpec::GetRatingForParticipant_Data input;

					printf("Enter the primary ID of participant A (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "0");
					input.participantDbId.primaryKey = atoi(inputStr);

					printf("Enter the secondary ID of participant A (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "1");
					input.participantDbId.secondaryKey = atoi(inputStr);

					printf("Enter the primary ID of the game (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "12");
					input.gameDbId.primaryKey = atoi(inputStr);

					printf("Enter the secondary ID of the game (int): ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "13");
					input.gameDbId.secondaryKey = atoi(inputStr);

					lobbyClient.DownloadRating(&input);
					break;
				}

				case '=':
				{
					printf("Print rating\n");
					RankingServerDBSpec::GetRatingForParticipant_Data *rating = lobbyClient.GetRating();

					printf("[in] participantDbId.primaryKey=%i\n", rating->participantDbId.primaryKey);
					printf("[in] participantDbId.secondaryKey=%i\n", rating->participantDbId.secondaryKey);
					printf("[in] gameDbId.primaryKey=%i\n", rating->gameDbId.primaryKey);
					printf("[in] gameDbId.secondaryKey=%i\n", rating->gameDbId.secondaryKey);
					printf("[out] participantFound=%i\n", rating->participantFound); // False means no such participant
					if (rating->participantFound)
						printf("[out] rating=%f\n", rating->rating);
					break;
				}
/// ---------------------------- CLAN FEATURES STILL INCOMPLETE, DO NOT USE ----------------------------------
				case 'A':
				{
					printf("Create clan\n");
					LobbyDBSpec::CreateClan_Data clanData;
					char inputStr[512];
					
					printf("Enter clan name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Test clan handle");
					clanData.initialClanData.handle = inputStr;
					lobbyClient.CreateClan(&clanData, false);

					break;
				}

				case 'B':
				{
					printf("Change clan handle\n");
					lobbyClient.ChangeClanHandle(0,"New clan handle");
					break;
				}
				case 'C':
				{
					printf("Leave clan\n");
					lobbyClient.LeaveClan(0,true,true);
					break;
				}
				case 'D':
				{
					printf("Download clans\n");
					// Just download my own clans
					lobbyClient.DownloadClans(0,0,true);
					break;
				}
				case 'E':
				{
					printf("Send clan join invitation\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.SendClanJoinInvitation(0,inputStr,0);
					break;
				}
				case 'F':
				{
					printf("Withdraw clan join invitation\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.WithdrawClanJoinInvitation(0,inputStr,0);
					break;
				}
				case 'G':
				{
					printf("Accept clan join invitation\n");

					LobbyDBSpec::UpdateClanMember_Data clanMemberData;
					clanMemberData.clanId.hasDatabaseRowId=false;
					printf("Enter clan name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Test clan handle");
					clanMemberData.clanId.handle=inputStr;

					lobbyClient.AcceptClanJoinInvitation(&clanMemberData, true);
					break;
				}
				case 'H':
				{
					printf("Reject clan join invitation\n");
					printf("Enter clan name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Test clan handle");
					lobbyClient.RejectClanJoinInvitation(inputStr, 0);
					break;
				}
				case 'I':
				{
					printf("Download by clan member status\n");
					// As an example, get all clans we are a member of
					lobbyClient.DownloadByClanMemberStatus(LobbyDBSpec::CLAN_MEMBER_STATUS_MEMBER);
					break;
				}
				case 'J':
				{
					printf("Send clan join request\n");
					LobbyDBSpec::UpdateClanMember_Data clanMemberData;
					clanMemberData.clanId.hasDatabaseRowId=false;
					printf("Enter clan name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Test clan handle");
					clanMemberData.clanId.handle=inputStr;
					lobbyClient.SendClanMemberJoinRequest(&clanMemberData);
					break;
				}
				case 'K':
				{

					printf("Withdraw clan join request\n");
					printf("Enter clan name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Test clan handle");
					lobbyClient.WithdrawClanMemberJoinRequest(inputStr,0);
					break;
				}
				case 'L':
				{
					printf("Accept clan join request\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.AcceptClanMemberJoinRequest(0,inputStr,0,true);
					break;
				}
				case 'M':
				{
					printf("Reject clan join request\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.RejectClanMemberJoinRequest(0,inputStr,0);
					break;
				}
				case 'N':
				{
					printf("Set clan member rank\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.SetClanMemberRank(0,inputStr,0,LobbyDBSpec::CLAN_MEMBER_STATUS_LEADER);
					break;
				}
				case 'O':
				{

					printf("Kick and blacklist user\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.ClanKickAndBlacklistUser(0,inputStr,0,true,true);
					break;
				}
				case 'P':
				{
					printf("Unblacklist user\n");
					printf("Enter user name: ");
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.ClanUnblacklistUser(0,inputStr,0);
					break;
				}
				case 'Q':
				{
					printf("Add post to clan board\n");
					LobbyDBSpec::AddToClanBoard_Data newPost;
					newPost.clanId.databaseRowId=1;
					newPost.subject="Hello world clan post (subject)";
					newPost.body="Hello world clan post (body)";
					lobbyClient.AddPostToClanBoard(&newPost,true,true,true);
					break;
				}
				case 'R':
				{
					printf("Remove post from clan board\n");
					ClanBoardPostId cbpi=1;
					lobbyClient.RemovePostFromClanBoard(0,&cbpi,true);
					break;
				}
				case 'S':
				{
					printf("Download my clan boards\n");
					lobbyClient.DownloadMyClanBoards(true);
					break;
				}
				case 'T':
				{
					printf("Download clan board\n");
					ClanId clanId=1;
					lobbyClient.DownloadClanBoard(0,&clanId,true);
					break;
				}
				case 'U':
				{
					printf("Download clan member\n");
					ClanId clanId=1;
					gets(inputStr);
					if (inputStr[0]==0) strcpy(inputStr, "Dogbert");
					lobbyClient.DownloadClanMember(0,&clanId,inputStr,0);
					break;
				}

			}
		}

		// Read all packets every tick. This also updates all plugins.
		for (packet=rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet=rakPeer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// Connected
				printf("Connected to lobby server.\n");
				serverLobbyAddress = packet->systemAddress;				
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;

			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString());
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Failed to connect!\nCheck that the server is running, and the IP and port is correct.\nThen restart this sample.\n");
				break;
			}

		}

		RakSleep(100);

	}



	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 0;
}

void PrintDownloadClansResult( LobbyDBSpec::GetClans_Data *callResult)
{
	if (callResult->clans.Size()>0)
	{
		printf("%i CLANS FOUND for user with ID %i\n", callResult->clans.Size(), callResult->userId.databaseRowId);
		for (unsigned clanIndex=0; clanIndex < callResult->clans.Size(); clanIndex++)
		{
			LobbyDBSpec::UpdateClan_Data *clan = callResult->clans[clanIndex];
			printf("(%i.) CLAN %s (ID=%i):\n", clanIndex+1, clan->handle.C_String(), clan->clanId.databaseRowId);
			printf("%i %i %i %i:\n", clan->integers[0], clan->integers[1],clan->integers[2],clan->integers[3]);
			printf("%.1f %.1f %.1f %.1f:\n", clan->floats[0], clan->floats[1],clan->floats[2],clan->floats[3]);
			printf("DESC1: %s\n", clan->descriptions[0].C_String());
			printf("DESC2: %s\n", clan->descriptions[1].C_String());
			printf("Requires invitations to join = ");
			if (clan->requiresInvitationsToJoin)
				printf("true\n");
			else
				printf("false\n");

			printf("Member list for clan %s:\n", clan->handle.C_String());
			for (unsigned memberIndex=0; memberIndex < callResult->clans[clanIndex]->members.Size(); memberIndex++)
			{
				LobbyDBSpec::UpdateClanMember_Data *member = clan->members[memberIndex];
				printf("(%i.) ", memberIndex+1);
				if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_LEADER)
					printf("<Leader>");
				else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_SUBLEADER)
					printf("<Subleader>");
				else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_MEMBER)
					printf("<Member>");
				else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_REQUESTED_TO_JOIN)
					printf("<Requested>");
				else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_INVITED_TO_JOIN)
					printf("<Invited>");
				else if (member->mEStatus1==LobbyDBSpec::CLAN_MEMBER_STATUS_BLACKLISTED)
					printf("<Blacklisted>");
				else
					printf("<Unknown>");
				printf(" %s (ID=%i)\n", member->userId.handle.C_String(), member->userId.databaseRowId);

				printf("%i %i %i %i:\n", member->integers[0], member->integers[1],member->integers[2],member->integers[3]);
				printf("%.1f %.1f %.1f %.1f:\n", member->floats[0], member->floats[1],member->floats[2],member->floats[3]);
				printf("DESC1: %s\n", member->descriptions[0].C_String());
				printf("DESC2: %s\n", member->descriptions[1].C_String());
			}
		}

	}
	else
		printf("No clans found\n");
}