RakNet 3.25
Copyright 2002-2005 Kevin Jenkins (rakkar@jenkinssoftware.com).
This API and the code herein created by and wholly and privately owned by Kevin Jenkins except where specifically indicated otherwise.
Licensed under the "RakNet" brand by Jenkins Software and subject to the terms of the relevant licensing agreement available at http://www.jenkinssoftware.com

------------------------------------------
Windows users (Visual Studio 2005)
-----------------------------------------
Load RakNet.sln and convert.
If it doesn't work, see Help/compilersetup.html or the training video at http://www.jenkinssoftware.com/raknet/manual/helloworldvideo.html on how to setup the project.

-----------------------------------------
Windows users (.NET 2003)
-----------------------------------------
Load RakNet.sln

-----------------------------------------
Windows users (VC6)
-----------------------------------------
Not officially supported.

-----------------------------------------
CYGWIN users
-----------------------------------------
Copy Include, Source, and whatever you want to run in the home directory.  Then type
g++ ../../lib/w32api/libws2_32.a *.cpp
You can run a.exe
You might have to copy *.dll from cygwin\bin as well.

-----------------------------------------
Linux users
-----------------------------------------
The Makefile was created by Netbeans 6.0. You can also open the project in Netbeans, as it reads the "nbproject" directory.

-----------------------------------------
DevCPP Users
-----------------------------------------
Load RakNet.dev

-----------------------------------------
CodeBlocks Users
-----------------------------------------
Load RakNet.cbp

------------------------------------------
DigitalMars Users
-----------------------------------------
A user contributed file DMCMakefile.win is included in the distribution.  I have no idea if it works or not.  If it doesn't, you can email me updates at rakkar@jenkinssoftware.com

-----------------------------------------
Mac Users
-----------------------------------------
From http://www.jenkinssoftware.com/raknet/forum/index.php?topic=746.0;topicseen
Open a Terminal window and type: cd ~/Desktop/RakNet/Source

Give the following command:

Code:
gcc -c -I ../Include -isysroot /Developer/SDKs/MacOSX10.3.9.sdk/ -arch ppc *.cpp

The sources should build cleanly. This gives you a bunch of PowerPC binaries, compiled against the 10.3.9 SDK which is a good thing.

Give the following command:

Code:
libtool -static -o raknetppc.a *.o

This will stitch together a static library for the PowerPC architecture. There may be warnings that some .o files do not have any symbols. If you want to be prudent, remove the named files (the .o files, not the .cpp files!) and re-run the libtool command.

Now, we build the source files for Intel:

Code:
gcc -c -I ../Include -isysroot /Developer/SDKs/MacOSX10.4u.sdk/ -arch i386 *.cpp

..and stitch it into a i386 library:

Code:
libtool -static -o rakneti386.a *.o

Now, type:

Code:
ls *.a

which should list the two .a files. Now, we make them into a universal binary:

Code:
lipo -create *.a -o libraknet.a

You now have a file named libraknet.a. This is the RakNet library, built to run on both PowerPC and Intel Macs. Enjoy! ;-)


-----------------------------------------
Other Platforms
-----------------------------------------
The define _COMPATIBILITY_1 while adding Compatibility1Includes.h may work for you.

-----------------------------------------
Package notes
-----------------------------------------
The Source directory contain all files required for the core of Raknet and is used if you want to use the source in your program or create your own dll
The Samples directory contains code samples and one game using an older version of Raknet.  The code samples each demonstrate one feature of Raknet.  The game samples cover several features.
The lib directory contains libs for debug and release versions of RakNet and RakVoice
The Help directory contains index.html, which is full help documentation in HTML format
There is a make file for linux users in the root directory.  Windows users can use projects under Samples\Project Samples

For support please visit
http://www.jenkinssoftware.com
