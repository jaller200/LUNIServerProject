#include "CharPackets.h"
#include "serverLoop.h"
#include "Packet.h"
#include "Database.h"
#include "User.h"

#include "SUtil\Ref.h"

#include "RakNet\BitStream.h"

using namespace std;

void SendCharPacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> usr) {
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 6, &bitStream);
	
	if (usr == NULL) {
		cout << "ERROR: User is null!" << endl;
	}

	uchar charactersLength = (uchar)usr->userChars;
	uchar frontChar = (uchar)usr->userFrontChar;

	bitStream.Write(charactersLength);
	bitStream.Write(frontChar);

	ostringstream userId;
	userId << usr->GetID();

	cout << userId.str() << endl;

	string userIdStr = userId.str();

	// This loop perhaps is very inefficient, but it makes sure
	// 4 chars are loaded (or, if they don't exist, fill with 0x00)
	for (uint i = 0; i <= charactersLength; i++) {
		CharactersPacket charData = CharactersPacket();

		ostringstream loopSS;
		loopSS << i;
		string loopStr = loopSS.str(); //loopSS.str();

		// Connect to the database and retrieve character info
		auto qr = Database::Query("SELECT * FROM `characters` WHERE `id` = '" + loopStr + "' AND `accountID` = '" + userIdStr + "';");

		// If the number of rows equals 0 from database (character doesn't exist), print it
		if (mysql_num_rows(qr) == 0) {
			cout << "Char " << loopStr <<  " doesn't exist yet.";
		} else {
			// Otherwise, fetch the data in the row
			auto ftc = mysql_fetch_row(qr);

			charData.objectID = (ulonglong)atoi(ftc[2]); // This is the character ID... ftc[1] is the user id
			charData.unknown1 = 0; // Dunno what this is...

			string sCharName = (string)ftc[3]; // This is the character name
			string sUnapprovedName = (string)ftc[4]; // This is the pre-defined (unapproved) name

			/*wstringstream swCharName; // Create a wstringstream for the char name
			swCharName << sCharName.c_str(); // Add the char name to it

			wstringstream swUnapprovedName; // Create a wstringstream for the unapproved name
			swUnapprovedName << sUnapprovedName.c_str(); // Add the unapproved name to it

			wstring wCharName = swCharName.str(); // Create a wstring from the wstringstream
			wstring wUnapprovedName = swUnapprovedName.str(); // Create a wstring from the wstringstream

			wCharName.resize(66);
			wUnapprovedName.resize(66);*/

			wstring wCharName = StringToWString(sCharName, 66);
			wstring wUnapprovedName = StringToWString(sUnapprovedName, 66);
			
			charData.charName = wCharName; // This is the name of the character
			charData.unapprovedName = wUnapprovedName; // This is the unapproved name of the character			
			
			charData.nameRejected = (uchar)atoi(ftc[5]); // This determines whether the user's name is approved
			charData.freeToPlay = (uchar)atoi(ftc[6]); // This determines whether the user if Free to Play
			charData.unknown2 = "";
			charData.shirtColor = (ulong)atoi(ftc[7]); // The shirt color
			charData.shirtStyle = (ulong)atoi(ftc[8]); // Possibly the shirt style... ?
			charData.pantsColor = (ulong)atoi(ftc[9]); // The pants color
			charData.hairStyle = (ulong)atoi(ftc[10]); // Hair style
			charData.hairColor = (ulong)atoi(ftc[11]); // Hair color
			charData.lh = 0; (ulong)atoi(ftc[12]); // This is the left hand item?
			charData.rh = 0; (ulong)atoi(ftc[13]); // This is the right hand item?
			charData.eyebrows = (ulong)atoi(ftc[14]); // Eyebrows
			charData.eyes = (ulong)atoi(ftc[15]); // Eyes
			charData.mouth = (ulong)atoi(ftc[16]); // Mouth
			charData.unknown4 = 0; // Dunno what this is...
			charData.lastZoneId = (ulong)atoi(ftc[25]); // The last world ID the user was in...
			charData.mapInstance = (ushort)atoi(ftc[26]); // The map instance
			charData.mapClone = (ulong)atoi(ftc[27]); // The map clone
			charData.lastLogout = 0; // Last logout in seconds (useless until we get world server working)
			//charData.equippedItemsLength = CharSpecialItemsCount(charData.objectID); // The items length

			charData.WriteDataToBitstream(&bitStream, i);

			GetCharSpecialItems(charData.objectID, &bitStream);
		}
	}	

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
	SavePacket("char_creation.bin", (char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}

void SendDeletePacket(RakPeerInterface *rakServer, SystemAddress& systemAddress, Ref<User> user, uchar *packetData, uint length) {
	if (length < 8) return; // If packet data is less than 8 bytes long (counting byte 0), then return as it only contains the packet header

	// User ID stringstream
	stringstream usrId;
	usrId << user->GetID();

	// Get the User ID into a string
	string usrIDString = usrId.str();

	// Get the objectID from the packets
	unsigned long long objectID;
	memcpy(&objectID, packetData + 0x08, 8);

	cout << "Deleting character with ID: " << objectID << " from account: " << usrIDString << endl;

	// Query to delete char from database
	stringstream query;
	query << "DELETE FROM `characters` WHERE `objectID` = '" << objectID << "';";

	// Get number of characters the user has
	uchar usrNumChars = user->userChars;

	// Query to subtract one character from user
	stringstream query2;
	query2 << "UPDATE `accounts` SET `numChars` = '" << usrNumChars - 1 << "' WHERE `id` = '" << usrIDString << "';";

	// Change front character to 0
	stringstream query3;
	query3 << "UPDATE `accounts` SET `frontChar` = '0' WHERE `id` = '" << usrIDString << "';";

	// Query the database with the above 3 queries...
	Database::Query(query.str()); // Delete char...
	Database::Query(query2.str()); // ... Update account numChars...
	Database::Query(query3.str()); // ... and finish by setting front char to 0

	// Set local user's numChars and frontChar
	user->userChars = usrNumChars - 1;
	user->userFrontChar = 0;

	// Create the packet that will be used to send that char deletion
	// packet to the client
	RakNet::BitStream bitStream;

	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 12, &bitStream);
	bitStream.Write((uchar)1); // Success?

	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

ulong FindCharShirtID(ulong shirtColor, ulong shirtStyle) {
	ulong shirtID = 0;

	// This is a switch case to determine the base shirt color (IDs from CDClient.xml)
	switch (shirtColor) {
	case 0:
	{
			  shirtID = shirtStyle >= 35 ? 5730 : SHIRT_BRIGHT_RED;
			  break;
	}

	case 1:
	{
			  shirtID = shirtStyle >= 35 ? 5736 : SHIRT_BRIGHT_BLUE;
			  break;
	}

	case 3:
	{
			  shirtID = shirtStyle >= 35 ? 5808 : SHIRT_DARK_GREEN;
			  break;
	}

	case 5:
	{
			  shirtID = shirtStyle >= 35 ? 5754 : SHIRT_BRIGHT_ORANGE;
			  break;
	}

	case 6:
	{
			  shirtID = shirtStyle >= 35 ? 5760 : SHIRT_BLACK;
			  break;
	}

	case 7:
	{
			  shirtID = shirtStyle >= 35 ? 5766 : SHIRT_DARK_STONE_GRAY;
			  break;
	}

	case 8:
	{
			  shirtID = shirtStyle >= 35 ? 5772 : SHIRT_MEDIUM_STONE_GRAY;
			  break;
	}

	case 9:
	{
			  shirtID = shirtStyle >= 35 ? 5778 : SHIRT_REDDISH_BROWN;
			  break;
	}

	case 10:
	{
			   shirtID = shirtStyle >= 35 ? 5784 : SHIRT_WHITE;
			   break;
	}

	case 11:
	{
			   shirtID = shirtStyle >= 35 ? 5802 : SHIRT_MEDIUM_BLUE;
			   break;
	}

	case 13:
	{
			   shirtID = shirtStyle >= 35 ? 5796 : SHIRT_DARK_RED;
			   break;
	}

	case 14:
	{
			   shirtID = shirtStyle >= 35 ? 5802 : SHIRT_EARTH_BLUE;
			   break;
	}

	case 15:
	{
			   shirtID = shirtStyle >= 35 ? 5808 : SHIRT_EARTH_GREEN;
			   break;
	}

	case 16:
	{
			   shirtID = shirtStyle >= 35 ? 5814 : SHIRT_BRICK_YELLOW;
			   break;
	}

	case 84:
	{
			   shirtID = shirtStyle >= 35 ? 5820 : SHIRT_SAND_BLUE;
			   break;
	}

	case 96:
	{
			   shirtID = shirtStyle >= 35 ? 5826 : SHIRT_SAND_GREEN;
			   shirtColor = 16;
			   break;
	}
	}

	// Initialize another variable for the shirt color
	ulong editedShirtColor = shirtID;
	
	// This will be the final shirt ID
	ulong shirtIDFinal;

	// For some reason, if the shirt color is 35 - 40,
	// The ID is different than the original... Was this because
	// these shirts were added later?
	if (shirtStyle >= 35) {
		shirtIDFinal = editedShirtColor += (shirtStyle - 35);
	}
	else {
		// Get the final ID of the shirt by adding the shirt
		// style to the editedShirtColor
		shirtIDFinal = editedShirtColor += (shirtStyle - 1);
	}

	cout << "Shirt ID is: " << shirtIDFinal << endl;

	return shirtIDFinal;
}

ulong FindCharPantsID(ulong pantsColor) {
	ulong pantsID = 2508;

	// This is a switch case to determine 
	// the pants color (IDs from CDClient.xml)
	switch (pantsColor) {
	case 0:
	{
			  pantsID = PANTS_BRIGHT_RED;
			  break;
	}

	case 1:
	{
			  pantsID = PANTS_BRIGHT_BLUE;
			  break;
	}

	case 3:
	{
			  pantsID = PANTS_DARK_GREEN;
			  break;
	}

	case 5:
	{
			  pantsID = PANTS_BRIGHT_ORANGE;
			  break;
	}

	case 6:
	{
			  pantsID = PANTS_BLACK;
			  break;
	}

	case 7:
	{
			  pantsID = PANTS_DARK_STONE_GRAY;
			  break;
	}

	case 8:
	{
			  pantsID = PANTS_MEDIUM_STONE_GRAY;
			  break;
	}

	case 9:
	{
			  pantsID = PANTS_REDDISH_BROWN;
			  break;
	}

	case 10:
	{
			   pantsID = PANTS_WHITE;
			   break;
	}

	case 11:
	{
			   pantsID = PANTS_MEDIUM_BLUE;
			   break;
	}

	case 13:
	{
			   pantsID = PANTS_DARK_RED;
			   break;
	}

	case 14:
	{
			   pantsID = PANTS_EARTH_BLUE;
			   break;
	}

	case 15:
	{
			   pantsID = PANTS_EARTH_GREEN;
			   break;
	}

	case 16:
	{
			   pantsID = PANTS_BRICK_YELLOW;
			   break;
	}

	case 84:
	{
			   pantsID = PANTS_SAND_BLUE;
			   break;
	}

	case 96:
	{
			   pantsID = PANTS_SAND_GREEN;
			   break;
	}
	}

	// Print the pants ID
	cout << "Pants ID is: " << pantsID << endl;

	return pantsID;
}

void GetCharSpecialItems(ulonglong objectID, RakNet::BitStream *bitStream) {
	ushort itemCount = 0;

	stringstream sObjectIDStr;
	sObjectIDStr << objectID;

	auto qr = Database::Query("SELECT `headID`, `neckID`, `shirtID`, `pantsID`, `leftHandID`, `rightHandID` FROM `characters` WHERE `objectID` = '" + sObjectIDStr.str() + "';");

	if (mysql_num_rows(qr) != 0) {
		auto ftc = mysql_fetch_row(qr);

		for (int i = 0; i < 6; i++) {
			if (atoi(ftc[i]) != 0) {
				itemCount += 1;
			}
		}

		bitStream->Write(itemCount);

		for (int j = 0; j < 6; j++) {
			if (atoi(ftc[j]) != 0) {
				bitStream->Write((ulong)atoi(ftc[j]));
			}
		}
	}

	cout << "User has " << itemCount << " number of special items";
}

void AddCharToDatabase(RakPeerInterface *rakServer, SystemAddress& systemAddress, uchar *packetData, uint length, Ref<User> usr) {
	if (length < 8) return;

	// Get the ID out the user logged in (from User.h)
	stringstream usrId;
	usrId << usr->GetID();

	// Convert it to a string to use with the queries.
	string usrIDString = usrId.str();

	// Create the variables to hold the char items
	ulong shirtColor;
	ulong shirtStyle;
	ulong pantsColor;
	ulong hairStyle;
	ulong hairColor;
	ulong lh;
	ulong rh;
	ulong eyebrows;
	ulong eyes;
	ulong mouth;

	// Copy the char items from the creation packet
	// to their variables
	memcpy(&shirtColor, packetData + 0x5F, 4);
	memcpy(&shirtStyle, packetData + 0x63, 4);
	memcpy(&pantsColor, packetData + 0x67, 4);
	memcpy(&hairStyle, packetData + 0x6B, 4);
	memcpy(&hairColor, packetData + 0x6F, 4);
	memcpy(&lh, packetData + 0x73, 4);
	memcpy(&rh, packetData + 0x77, 4);
	memcpy(&eyebrows, packetData + 0x7B, 4);
	memcpy(&eyes, packetData + 0x7F, 4);
	memcpy(&mouth, packetData + 0x83, 4);

	// Print to the console the char items... (these
	// will also be in the database to view later)
	cout << "Shirt color is: " << shirtColor << endl;
	cout << "Shirt style is: " << shirtStyle << endl;
	cout << "Pants color is: " << pantsColor << endl;
	cout << "Hair style is: " << hairStyle << endl;
	cout << "Hair color is: " << hairColor << endl;
	cout << "LH is: " << lh << endl;
	cout << "RH is: " << rh << endl;
	cout << "Eyebrows are: " << eyebrows << endl;
	cout << "Eyes are: " << eyes << endl;
	cout << "Mouth is: " << mouth << endl;

	// Figure out the pants and shirt IDs that the
	// char starts with...
	ulong shirtID = FindCharShirtID(shirtColor, shirtStyle);
	ulong pantsID = FindCharPantsID(pantsColor);
	
	// Initialize username string...
	string username;

	// Read the string from the packet
	int i = 8;
	while (true) {
		username.push_back(packetData[i++]);
		if (packetData[++i] == 0) break;
	}
	username.push_back(0);
	i++;

	// Print the username to the console
	cout << "Username is: " << username << endl;

	// Get the line numbers from the packet for the character
	// premade name (in minifigname_first.txt, _middle.txt, and
	// _last.txt)
	ulong firstLineNum;
	ulong middleLineNum;
	ulong lastLineNum;

	// Copy the data into the variables
	memcpy(&firstLineNum, packetData + 0x4A, 4);
	memcpy(&middleLineNum, packetData + 0x4E, 4);
	memcpy(&lastLineNum, packetData + 0x52, 4);

	// Now, figure out the unapproved name and store it as
	// as string
	string unapprovedName = GetUnapprovedUsername(firstLineNum, middleLineNum, lastLineNum);
	

	// Save character into the database
	uchar creationStatus = 0;

	// Get the strings of the username
	string usernameStr = string(username.c_str());
	string unapprovedUserStr = string(unapprovedName.c_str());

	// Prepare the first query to check to see if username
	// already exists
	stringstream query1;
	query1 << "SELECT `id` FROM `characters` WHERE `name` = '" << usernameStr << "' LIMIT 1;";

	cout << "Query is: " << query1.str() << endl;

	// Query the database...
	auto qr = Database::Query(query1.str());

	// If the username does not exist, mysql_num_rows(qr) should
	// equal 0. Otherwise, return the error to the client
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL) {
		// Query the database again to see if the unapproved
		// name already exists...
		auto qr2 = Database::Query("SELECT `unapprovedName` FROM `characters` WHERE `unapprovedName` = '" + unapprovedName + "';");
		
		// If the unapproved name does not exist, mysql_num_rows(qr)
		// should equal 0. Otherwise, return the error to the client
		if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == 0) {
			cout << "Username and predefined username not in use! Storing in database..." << endl;
			
			// Figure out which char ID (NOT objectID) to put the
			// Character in (0 - 3); 0 = first char, 3 = 4th char
			for (uchar i = 1; i <= 4; i++) {
				ostringstream charId;
				charId << i - 1;

				// Get the ID of the character (from 0 - 3)
				string charIDStr = charId.str();
				
				// Query the database to see if character with ID already exists
				auto whichChar = Database::Query("SELECT `id`, `accountID` FROM `characters` WHERE `id` = '" + charIDStr + "' AND `accountID` = '" + usrIDString + "';");
				
				// The ObjectID is basically the unique ID that
				// destingushes all user chars ingame.
				// Create the ObjectID ulonglong
				ulonglong objectID;

				// Figure out User ID (known as ObjectID in database)
				while (true) {
					// Initalize the tempObjectID
					ulonglong tempObjectID;

					// Seed the random number generator with the
					// current time so that it generates different
					// numbers each time
					srand(time(NULL));

					// Generate a random number
					tempObjectID = rand() % RAND_MAX;

					// Check to see if another user already is using
					// the ObjectID
					stringstream query3;
					query3 << "SELECT `id` FROM `characters` WHERE `objectID` = '" << tempObjectID << "';";
					auto objectIDExists = Database::Query(query3.str());

					// If not, use it for this char. If so, continue to
					// generate another one (we shouldn't ever run out...)
					if (mysql_num_rows(objectIDExists) == 0) {
						objectID = tempObjectID;
						cout << "User ID is: " << objectID << endl;
						break;
					}
					else {
						continue;
					}
				}

				// Prepare query to insert char into database
				stringstream query2;
				query2 << "INSERT INTO `characters` (`id`, `accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, `shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, `shirtID`, `pantsID`, `lastZoneId`) VALUES ('" << charIDStr << "', '" << usrId.str() << "', '" << objectID << "', '" << usernameStr << "', '" << unapprovedUserStr << "', '0', '0', '" << shirtColor << "', '" << shirtStyle << "', '" << pantsColor << "', '" << hairStyle << "', '" << hairColor << "', '" << lh << "', '" << rh << "', '" << eyebrows << "', '" << eyes << "', '" << mouth << "', '" << shirtID << "', '" << pantsID << "', '1000');";

				// Prepare the increaseChars value
				stringstream increaseChars;
				increaseChars << usr->userChars + 1;

				// If the user has less than 4 characters, and
				// i = a charID that does not exists, use that and
				// insert the char in the database.
				if (mysql_num_rows(whichChar) == 0) {
					cout << "Query to insert is: " << query2.str() << endl;

					Database::Query(query2.str());
					Database::Query("UPDATE `accounts` SET `numChars` = '" + increaseChars.str() + "' WHERE `id` = '" + usrIDString + "';");
					Database::Query("UPDATE `accounts` SET `frontChar` = '" + charIDStr + "' WHERE `id` = '" + usrIDString + "';");

					usr->userChars = atoi(increaseChars.str().c_str());
					usr->userFrontChar = atoi(charIDStr.c_str());
					usr->nameInUse = 0;
					break;
				}
				else {
					cout << "User with charID " << charId.str() << " already exists for user with id " << usrId.str() << endl;
					continue;
				}
			}
		}
		else {
			cout << "Pre-defined username already in use!" << endl;
			usr->nameInUse = 1;
			creationStatus = 3;
		}
	}
	else {
		cout << "Username already in use!" << endl;
		usr->nameInUse = 1;
		creationStatus = 4;
	}

	// Create the bitstream to send to the client with the
	// char create status
	RakNet::BitStream bitStream;
	CreatePacketHeader(ID_USER_PACKET_ENUM, 5, 7, &bitStream);

	bitStream.Write(creationStatus);

	// Send the packet
	rakServer->Send(&bitStream, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
}

string GetUnapprovedUsername(ulong firstLine, ulong middleLine, ulong lastLine) {
	// First name
	fstream firstNameFile(".//names//minifigname_first.txt");
	GoToLine(firstNameFile, firstLine + 1);

	string firstName;
	firstNameFile >> firstName;
	cout << endl << "First name is: " << firstName << endl;

	// Middle Name
	fstream middleNameFile(".//names//minifigname_middle.txt");
	GoToLine(middleNameFile, middleLine + 1);

	string middleName;
	middleNameFile >> middleName;
	cout << "Middle name is: " << middleName << endl;

	// Last Name
	fstream lastNameFile(".//names//minifigname_last.txt");
	GoToLine(lastNameFile, lastLine + 1);

	string lastName;
	lastNameFile >> lastName;
	cout << "Last name is: " << lastName << endl << endl;

	stringstream unapprovedName;
	unapprovedName << firstName << middleName << lastName;

	cout << "File unapproved char name is: " << unapprovedName.str() << endl;

	return unapprovedName.str();
}


// ----- FILE METHODS ----- //

#ifdef max
#pragma push_macro("max")
#undef max
#define _restore_max_
#endif

fstream& GoToLine(fstream& file, ulong num) {
	file.seekg(std::ios::beg);
	for (int i = 0; i < num - 1; ++i) {
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return file;
}

#ifdef _restore_max_
#pragma pop_macro("max")
#undef _restore_max_
#endif
