# LUNIServerProject
A repository trying to bring back the defunct game, LEGO Universe.

This is a GitHub-Project for the developers of LUNI (Lego Universe Newly Imagined), a project to create a server in C++, using the RakNet libraries, that will be able to provide all aspects of the LEGO Universe game as it was when it was live. Don't download any files from here if you aren't a developer, because the versions here aren't tested and may include many bugs. If you are searching for a tested version of the LUNI-Server, visit [TimTech Software](http://bit.ly/-lu-).

If you would like to get involved, but don't have enough knowledge, please see the [kickstart in the wiki](https://github.com/jaller200/LUNIServerProject/wiki/Getting-Started-in-Development).

Repository Owner: [jaller200 (Jon002)](https://github.com/jaller200)

##Requirements

* [Visual Studio 2013](https://www.visualstudio.com/downloads/download-visual-studio-vs) (community/express will work fine)
* [XAMPP](https://www.apachefriends.org/index.html) or similar WAMP stack (specifically a MySQL server and the ability to set it up with the LUNI software)
* LUNI Compatable LEGO Universe client for testing ([Nexus2 (Direct Download)](http://timtechsoftware.com/lu/nexus2.zip) client suggested)

##Usage

1. Gather and install all neccessary components listed in requirements
2. Clone this repository locally, or download the source directly
3. Import the .sql to your MySQL database. If using phpMyAdmin, start Apache and mysql and then open the admin page. Click the import button on the right, then browse to locate the sql file from the source and import it. Apache should not be needed afterwords (unless you need to use phpMyAdmin).
4. From here, as long as the MySQL database is running, you should be able to run the LUNI Server application through a built executable or using the debugger in Visual Studio, and connect into it using the Nexus2 Client.

##Contributor Guidelines & Getting Started in Development
See [CONTRIBUTING](CONTRIBUTING.md)

##ChangeLog
Much info thanks to [luni.info's server changelog](http://luni.info/servers/).

0.7.0: [In Progress]

0.6.0a: Same relative changes as 0.5.1a

0.6.0: Dynamic packet construction (auth and char) and forced saving to SQL, bug in the world server

0.5.1a: Fixed common compilation and execution errors caused by incorrect project settings and missing DLLs. 

0.5.1: Allowed for server to be compiled into Release

0.5.0: Switching between four worlds (Nimbus Isle, Nimbus Station, Crux Prime, and Gnarled Forest) by selection of different character

0.4.0: World loading

0.3.0: Successfully communicate with client, characters creatable but not saved or used, and the server could authenticate with the client and load worlds up to 35% (33% and 40% on some worlds)

0.2.0: Framework, MySQL database, no code

0.1.0: Concept

##Credits and License
See [CREDITS AND LICENSE](CREDITS AND LICENSE.md)

##Disclaimer

The LEGO® Group does endorse or authorize the operation of this game and is not liable for any safety issues in relation to the operation of this game. Use permitted on a solely non-commercial basis.


This project is not endorsed or affiliated with the original LUNI Server repository at http://sourceforge.net/projects/luniserver/
