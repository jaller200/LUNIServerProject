#include "WorldPackets.h"
#include "serverLoop.h"

// Fetch the mail data from the database
void FetchMailData(RakPeerInterface *rakServer, SystemAddress &systemAddr, Ref<User> user) {
	/*auto v = OpenPacket(".\\worldTest\\mail\\mailData.bin");
	ServerSendPacket(rakServer, v, systemAddr);

	return;*/
	
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 49, &bitStream);
	bitStream.Write((ulonglong)4);

	auto qr = Database::Query("SELECT `mailCount` FROM `characters` WHERE `name` = 'Bob';");

	if (mysql_num_rows(qr) == 0) {
		cout << "ERROR!! UNDEFINED NUMBER OF MAILS!!!" << endl;
	}
	else {
		auto ftc = mysql_fetch_row(qr);
	
		ushort mailCount = (ushort)atoi(ftc[0]);

		cout << "Mail count is: " << mailCount << endl;
	
		bitStream.Write(mailCount);
		bitStream.Write((ushort)0);

		if (mailCount > 0) {
			ulonglong mailID = 1;
			wstring mailSubject(L"Welcome to LUNIServer!");
			mailSubject.resize(100);
			wstring mailBody(L"Welcome to the LUNIServer Project\n This project is dedicated to recreating LEGO Universe.");
			mailBody.resize(800);
			wstring mailSender(L"Joe");
			mailSender.resize(84);

			uchar attachment = 0;

			ulonglong sendTime = 10000;

			bool isRead = false;

			bitStream.Write(mailID);
			bitStream.Write((char*)mailSubject.data(), sizeof(wchar_t)* mailSubject.length() / 2);
			bitStream.Write((char*)mailBody.data(), sizeof(wchar_t)* mailBody.length() / 2);
			bitStream.Write((char*)mailSender.data(), sizeof(wchar_t)* mailSender.length() / 2);
				
			if (attachment == 0) {
				bitStream.Write(0xffffffff);
			}
			else {
				bitStream.Write(attachment);
			}

			uchar i = 0;

			for (uchar j = 0; j < 28; j++) {
				bitStream.Write(i);
			}

			bitStream.Write(sendTime);
			bitStream.Write(isRead);

			for (uchar k = 0; k < 7; k++) {
				bitStream.Write(i);
			}
		}
	}


	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddr, false);
	SavePacket("mailResponse.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}