/*
	Lego Universe Server WIP
	Code done by humanoid24 (aka Triver) as of 01.12.2013

	Additional credits to raffa505 30.01.2013 - 14.02.2013 (Database MySQL, Character, User, World, Color, Threads, Loading both Nimbus 
	Isle and Nimbus Station)

	Added code and additional documentation done by Jon002 20.02.2015 (Character Creation, Custom Logins and success states, Dynamic Packets
	for Auth and Char servers)

	This source code requires RakNet version 3.25 as an external dependency to work with the LUNI Client.

	The source is open and free under the GPL License, Version 3 for use on a non-commercial basis
*/

#include "Common.h"
#include "serverLoop.h"
#include "Database.h"

#ifdef DEBUG
	#include "User.h"
	#include "Character.h"
#endif

#include "SUtil\IniReader.h"
#include "SUtil\Kbhit.h"
#include <thread>

#ifdef _WIN32
	// For mkdir command
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

using namespace std;

// Load the config file (config.ini)
void LoadConfig(CONNECT_INFO& auth, CONNECT_INFO& character, CONNECT_INFO& world) {
	CIniReader iniReader(".\\config.ini"); // Load config.ini
	
	// Default IP resorts to the auth.redirectIp BTW
	strcpy(auth.redirectIp, iniReader.ReadString("Settings", "redirect_ip", "127.0.0.1")); // Copy the auth redirect IP from the file
	strcpy(character.redirectIp, auth.redirectIp); // Copy the char redirect IP from the file
	strcpy(world.redirectIp, auth.redirectIp); // Copy the world redirect IP

	// Get whether to use encryption
	auth.useEncryption = character.useEncryption = world.useEncryption = iniReader.ReadBoolean("Settings", "use_encryption", false);

	// Get whether to use log file
	auth.logFile = character.logFile = world.logFile = iniReader.ReadBoolean("Settings", "log_file", true);

	// NOTE: The default ports are the ports LU listens on / redirects on
	// Changing these may result in the game not running properly!
	auth.redirectPort = iniReader.ReadInteger("Auth", "redirect_port", 2002); // Get auth redirect port (default: 2002)
	auth.listenPort = iniReader.ReadInteger("Auth", "listen_port", 1001); // Get auth listen port (default: 1001)

	character.redirectPort = iniReader.ReadInteger("Char", "redirect_port", 2003); // Get char redirect port (default: 2003)
	character.listenPort = iniReader.ReadInteger("Char", "listen_port", 2002); // Get char listen port (default: 2002)

	world.redirectPort = iniReader.ReadInteger("World", "redirect_port", 2004); // Get world redirect port (default: 2004)
	world.listenPort = iniReader.ReadInteger("World", "listen_port", 2003); // Get world listen port (default: 2003)

	// Try to connect to the database...
	try {
		Database::Connect(iniReader.ReadString("MYSQL", "host", "localhost"), iniReader.ReadString("MYSQL", "database", "luni"), iniReader.ReadString("MYSQL", "username", "root"), iniReader.ReadString("MYSQL", "password", ""));
	}
	// Otherwise, exit the server
	catch (MySqlException& ex) {
		QuitError(ex.what());
	}

	// Print that the server loaded the config file!
	cout << "Loaded config! \nConnected to mysql database!\n\n";
}

// This is the entry point into the server (the main() function)
int main() {
	// Print starting info to the console
	cout << "LL        EE    EE   GG     GG   OO\n";
	cout << "LL        EE    EE   GGG    GG   OO\n";
	cout << "LL        EE    EE   GGG    GG   OO\n";
	cout << "LL        EE    EE   GG G   GG   OO\n";
	cout << "LL        EE    EE   GG  G  GG   OO\n";
	cout << "LLLLLL    EE    EE   GG   G GG   OO\n";
	cout << "LLLLLLLL   EEEEEE    GG    GGG   OO   Server\n";
	cout << "\tOriginal project: luniserver.sf.net\n\n";

	cout << "This version is based on Jon002s code.\n";
	cout << "This version is still very unstable! Don't be surprised if the server crashes!s\n\n\n";

	cout << "Initializing LUNI test server...\n \nPress enter type \"help\" and enter again for commands\n\n";

	// Initialize the auth, character, and world CONNECT_INFO structs
	CONNECT_INFO auth, character, world;

	// Load the values for them and store them into their structs
	LoadConfig(auth, character, world);

	// Create the directory .//logs// if it does not exist
	_mkdir("logs");

	// If debug is on, print additional data to the console
	#ifdef DEBUG
	{
		cout << "\nDEBUG is ON!\n";
		cout << "\n Minifigure size: " << sizeof(MinifigureData) << endl;

		auto testbin = OpenPacket(".\\char\\char_aw2.bin");
		//PacketCharacter * pk = (PacketCharacter*) testbin;

		/*vector< Ref<Character> > characters(4);
		auto aux = (uchar*)testbin.data() +22;
		characters[0] = Ref<Character>(new Character(aux));
		cout << "\nTest (reading characters form char\\char_aw2.bin) character1 : " << characters[0]->GetName() << endl;
		//cout << characters[0]->Data.travelling << endl;
		
		aux += characters[0]->GetGeneratedPacketSize();
		characters[1] = Ref<Character>(new Character(aux));
		cout << "\nTest character2 : " << characters[1]->GetName() << endl;*/

		string s = "ciao";
		RakNet::BitStream bs;
		bs.WriteCompressed(s);

		cout << endl << RawDataToString(bs.GetData(), bs.GetNumberOfBytesUsed()) << endl;
		SavePacket("the Test.bin", (char*)bs.GetData(), bs.GetNumberOfBytesUsed());

		cout << endl;
	}
	#endif

	// Create the UserPool to store all users by their systemAddress (shared by threads)
	Ref< UsersPool > OnlineUsers = new UsersPool();

	// Create a new CrossThreadQueue for writing output to the console from a thread
	Ref< CrossThreadQueue< string > > OutputQueue = new CrossThreadQueue< string >();

	// Start the three new threads (Auth, Char, and World servers)
	thread thauth(AuthLoop, &auth, OnlineUsers, OutputQueue);
	thread thchar(CharactersLoop, &character, OnlineUsers, OutputQueue);
	thread thworld(WorldLoop, &world, OnlineUsers, OutputQueue);

	// If quit is ever equal to true, quit the server
	bool quit = false;

	// Keep the server from quitting by using a infinite loop
	while (!quit) {
		if (OutputQueue->Count() > 0) { // Pop the thread messages
			cout << OutputQueue->Pop();
		}

		if (_kbhit()) { // Parsing server commands. Press enter to start writing a command (may need to lock consoleoutmutex...)
			string command; // Initialize command string...
			cout << "> "; // Print "> " to show user where to type
			cin >> command; // Get the command
			cout << endl; // End the line

			// Match command to a pre-specified command here...
			if (command == "help") cout << "\nAvailable commands: \nquit = Quit the Server \nregister = Register New User \nuser_online = Show Number of Online Users";
			else if (command == "quit") quit = LUNIterminate = true;
			else if (command == "character_log_enable") character.logFile = true;
			else if (command == "character_log_disable") character.logFile = false;
			else if (command == "world_log_enable") world.logFile = true;
			else if (command == "world_log_disable") world.logFile = false;
			else if (command == "user_online") {
				cout << "\n Online user: " << OnlineUsers->Count() << endl;
			}
			else if (command == "register") {
				string username, password;
				cout << "Username: ";
				cin >> username; // Get the username
				if (User::FindUserID( username ) == 0) { // Check to see if username already exists... If not, continue
					cout << "Password: ";
					cin >> password; // Get the password

					// Create the new user into the database
					Database::Query("INSERT INTO `accounts` (`id`, `name`, `password`, `email`, `ip`, `rank`, `numChars`, `frontChar`, `lastLog`, `activeSub`, `subTime`, `legoClub`, `locked`, `banned`, `loginTries`) VALUES (NULL, '" + username + "', '" + password + "', '', '127.0.0.1', '0', '0', '0', CURRENT_TIMESTAMP, '0', '', '', '0', '0', '0');");
				} else cout << "Username already exist!\n";

			} else cout << "Invalid Command: " << command << "!" << endl;
		}
	}

	// No longer in use...
	/*thauth.join();
	thchar.join();
	thworld.join();*/

	// If the loop was terminated, quit the server
	exit(0);
}
