/*
	Lego Universe Authentification Server WIP
	Code done by humanoid24 (aka Triver) as of 01.12.2013
	additional credits to raffa505 30.01.2013 - 14.02.2013 (Database mysql, Character, User, World, Color, threads, loading both nimbus 
	isle and nimbus station)

	This source code requires RakNet version 3.25 as
	an external dependency to work with the luni client.
	the source is open and free and all trademarks are of their respective owners
	you are not autorized to open a private server. this is only for testing / development
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
	// For mkdir
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif

using namespace std;

void LoadConfig(CONNECT_INFO& auth, CONNECT_INFO& character, CONNECT_INFO& world) {
	CIniReader iniReader(".\\config.ini");
	strcpy(auth.redirectIp, iniReader.ReadString("Settings", "redirect_ip", "127.0.0.1"));
	strcpy(character.redirectIp, auth.redirectIp);
	strcpy(world.redirectIp, auth.redirectIp);

	auth.useEncryption = character.useEncryption = world.useEncryption = iniReader.ReadBoolean("Settings", "use_encryption", false);
	auth.logFile = character.logFile = world.logFile = iniReader.ReadBoolean("Settings", "log_file", true);

	auth.redirectPort = iniReader.ReadInteger("Auth", "redirect_port", 2002);
	auth.listenPort = iniReader.ReadInteger("Auth", "listen_port", 1001);

	character.redirectPort = iniReader.ReadInteger("Char", "redirect_port", 2003);
	character.listenPort = iniReader.ReadInteger("Char", "listen_port", 2002);

	world.redirectPort = iniReader.ReadInteger("World", "redirect_port", 2004);
	world.listenPort = iniReader.ReadInteger("World", "listen_port", 2003);

#ifndef LUNI_NO_AUTH
	try {
		Database::Connect(iniReader.ReadString("MYSQL", "host", "localhost"), iniReader.ReadString("MYSQL", "database", "luni"), iniReader.ReadString("MYSQL", "username", "root"), iniReader.ReadString("MYSQL", "password", ""));
	}
	catch (MySqlException& ex) {
		QuitError(ex.what());
	}

	cout << "Loaded config! \nConnected to mysql database!\n\n";
#else
	cout << "MySql disabled\n\n";
#endif
}

int main() {
	cout << "LL        EE    EE   GG     GG   OO\n";
	cout << "LL        EE    EE   GGG    GG   OO\n";
	cout << "LL        EE    EE   GGG    GG   OO\n";
	cout << "LL        EE    EE   GG G   GG   OO\n";
	cout << "LL        EE    EE   GG  G  GG   OO\n";
	cout << "LLLLLL    EE    EE   GG   G GG   OO\n";
	cout << "LLLLLLLL   EEEEEE    GG    GGG   OO   Server\n";
	cout << "\tOriginal project: luniserver.sf.net\n\n\n";


	cout << "Initializing LUNI test server...\n \nPress enter type \"help\" and enter again for commands\n\n";
	CONNECT_INFO auth, character, world;
	LoadConfig(auth, character, world);
	_mkdir("logs");

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

	Ref< UsersPool > OnlineUsers = new UsersPool(); //online userpool shared by threads
	Ref< CrossThreadQueue< string > > OutputQueue = new CrossThreadQueue< string >(); //used by threads for writing output

	thread thauth(AuthLoop, &auth, OnlineUsers, OutputQueue);
	thread thchar(CharactersLoop, &character, OnlineUsers, OutputQueue);
	thread thworld(WorldLoop, &world, OnlineUsers, OutputQueue);

	bool quit = false;
	while (!quit) {
		if (OutputQueue->Count() > 0) { //pop the thread messages
			cout << OutputQueue->Pop();
		}

		if (_kbhit()) { //parsing server commands. press enter to start writing a command (may need to lock consoleoutmutex...)
			string command;
			cout << "> ";
			cin >> command;
			cout << endl;
			if (command == "help") cout << "\nAviable commands: \nquit = quit the server \nregister = register new user \nuser_online = show online users";
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
				cin >> username;
				if (User::FindUserID( username ) == 0) {
					cout << "Password: ";
					cin >> password;
					Database::Query("INSERT INTO `accounts` (`id`, `name`, `password`, `email`, `ip`, `rank`, `lastLog`, `activeSub`, `subTime`, `legoClub`) VALUES (NULL, '" + username + "', '" + password + "', '', '127.0.0.1', '0', CURRENT_TIMESTAMP, '0', '', '');");
				} else cout << "Username already exist!\n";

			} else cout << "invalid command: " << command << endl;
		}
	}

	/*thauth.join();
	thchar.join();
	thworld.join();*/

	exit(0);
}
