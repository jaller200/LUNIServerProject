#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++-4.2
CXX=g++-4.2
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Debug/GNU-Linux-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Source/FileList.o \
	${OBJECTDIR}/Source/ExtendedOverlappedPool.o \
	${OBJECTDIR}/Source/TCPInterface.o \
	${OBJECTDIR}/Source/MessageFilter.o \
	${OBJECTDIR}/Source/BitStream_NoTemplate.o \
	${OBJECTDIR}/Source/PacketConsoleLogger.o \
	${OBJECTDIR}/Source/FileOperations.o \
	${OBJECTDIR}/Source/GridSectorizer.o \
	${OBJECTDIR}/Source/RakNetStatistics.o \
	${OBJECTDIR}/Source/NetworkIDObject.o \
	${OBJECTDIR}/Source/DS_ByteQueue.o \
	${OBJECTDIR}/Source/BitStream.o \
	${OBJECTDIR}/Source/RakMemoryOverride.o \
	${OBJECTDIR}/Source/TelnetTransport.o \
	${OBJECTDIR}/Source/AutoRPC.o \
	${OBJECTDIR}/Source/LightweightDatabaseServer.o \
	${OBJECTDIR}/Source/LightweightDatabaseCommon.o \
	${OBJECTDIR}/Source/AsynchronousFileIO.o \
	${OBJECTDIR}/Source/TableSerializer.o \
	${OBJECTDIR}/Source/GetTime.o \
	${OBJECTDIR}/Source/ConnectionGraph.o \
	${OBJECTDIR}/Source/Rand.o \
	${OBJECTDIR}/Source/CheckSum.o \
	${OBJECTDIR}/Source/ReplicaManager2.o \
	${OBJECTDIR}/Source/DS_HuffmanEncodingTree.o \
	${OBJECTDIR}/Source/PluginInterface.o \
	${OBJECTDIR}/Source/Router.o \
	${OBJECTDIR}/Source/ConsoleServer.o \
	${OBJECTDIR}/Source/LogCommandParser.o \
	${OBJECTDIR}/Source/RakNetworkFactory.o \
	${OBJECTDIR}/Source/SimpleMutex.o \
	${OBJECTDIR}/Source/RakNetTransport.o \
	${OBJECTDIR}/Source/RakPeer.o \
	${OBJECTDIR}/Source/RakNetTypes.o \
	${OBJECTDIR}/Source/RakThread.o \
	${OBJECTDIR}/Source/NatPunchthrough.o \
	${OBJECTDIR}/Source/DS_BytePool.o \
	${OBJECTDIR}/Source/RakNetMemory.o \
	${OBJECTDIR}/Source/StringTable.o \
	${OBJECTDIR}/Source/FileListTransfer.o \
	${OBJECTDIR}/Source/EncodeClassName.o \
	${OBJECTDIR}/Source/HTTPConnection.o \
	${OBJECTDIR}/Source/NetworkIDManager.o \
	${OBJECTDIR}/Source/RPCMap.o \
	${OBJECTDIR}/Source/_FindFirst.o \
	${OBJECTDIR}/Source/Itoa.o \
	${OBJECTDIR}/Source/ReadyEvent.o \
	${OBJECTDIR}/Source/ReliabilityLayer.o \
	${OBJECTDIR}/Source/DirectoryDeltaTransfer.o \
	${OBJECTDIR}/Source/RakSleep.o \
	${OBJECTDIR}/Source/SystemAddressList.o \
	${OBJECTDIR}/Source/FormatString.o \
	${OBJECTDIR}/Source/PacketFileLogger.o \
	${OBJECTDIR}/Source/SocketLayer.o \
	${OBJECTDIR}/Source/DS_Table.o \
	${OBJECTDIR}/Source/EmailSender.o \
	${OBJECTDIR}/Source/DataCompressor.o \
	${OBJECTDIR}/Source/DataBlockEncryptor.o \
	${OBJECTDIR}/Source/RakString.o \
	${OBJECTDIR}/Source/LinuxStrings.o \
	${OBJECTDIR}/Source/FunctionThread.o \
	${OBJECTDIR}/Source/EpochTimeToString.o \
	${OBJECTDIR}/Source/SHA1.o \
	${OBJECTDIR}/Source/CommandParserInterface.o \
	${OBJECTDIR}/Source/StringCompressor.o \
	${OBJECTDIR}/Source/ThreadsafePacketLogger.o \
	${OBJECTDIR}/Source/PacketLogger.o \
	${OBJECTDIR}/Source/FullyConnectedMesh.o \
	${OBJECTDIR}/Source/ReplicaManager.o \
	${OBJECTDIR}/Source/SuperFastHash.o \
	${OBJECTDIR}/Source/RakNetCommandParser.o \
	${OBJECTDIR}/Source/LightweightDatabaseClient.o \
	${OBJECTDIR}/Source/rijndael.o \
	${OBJECTDIR}/Source/Gen_RPC8.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} Lib/GNU-Linux-x86/libraknetd.a

Lib/GNU-Linux-x86/libraknetd.a: ${OBJECTFILES}
	${MKDIR} -p Lib/GNU-Linux-x86
	${RM} Lib/GNU-Linux-x86/libraknetd.a
	${AR} rv Lib/GNU-Linux-x86/libraknetd.a ${OBJECTFILES} 
	$(RANLIB) Lib/GNU-Linux-x86/libraknetd.a

${OBJECTDIR}/Source/FileList.o: Source/FileList.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FileList.o Source/FileList.cpp

${OBJECTDIR}/Source/ExtendedOverlappedPool.o: Source/ExtendedOverlappedPool.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ExtendedOverlappedPool.o Source/ExtendedOverlappedPool.cpp

${OBJECTDIR}/Source/TCPInterface.o: Source/TCPInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/TCPInterface.o Source/TCPInterface.cpp

${OBJECTDIR}/Source/MessageFilter.o: Source/MessageFilter.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/MessageFilter.o Source/MessageFilter.cpp

${OBJECTDIR}/Source/BitStream_NoTemplate.o: Source/BitStream_NoTemplate.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/BitStream_NoTemplate.o Source/BitStream_NoTemplate.cpp

${OBJECTDIR}/Source/PacketConsoleLogger.o: Source/PacketConsoleLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/PacketConsoleLogger.o Source/PacketConsoleLogger.cpp

${OBJECTDIR}/Source/FileOperations.o: Source/FileOperations.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FileOperations.o Source/FileOperations.cpp

${OBJECTDIR}/Source/GridSectorizer.o: Source/GridSectorizer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/GridSectorizer.o Source/GridSectorizer.cpp

${OBJECTDIR}/Source/RakNetStatistics.o: Source/RakNetStatistics.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetStatistics.o Source/RakNetStatistics.cpp

${OBJECTDIR}/Source/NetworkIDObject.o: Source/NetworkIDObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/NetworkIDObject.o Source/NetworkIDObject.cpp

${OBJECTDIR}/Source/DS_ByteQueue.o: Source/DS_ByteQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DS_ByteQueue.o Source/DS_ByteQueue.cpp

${OBJECTDIR}/Source/BitStream.o: Source/BitStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/BitStream.o Source/BitStream.cpp

${OBJECTDIR}/Source/RakMemoryOverride.o: Source/RakMemoryOverride.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakMemoryOverride.o Source/RakMemoryOverride.cpp

${OBJECTDIR}/Source/TelnetTransport.o: Source/TelnetTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/TelnetTransport.o Source/TelnetTransport.cpp

${OBJECTDIR}/Source/AutoRPC.o: Source/AutoRPC.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/AutoRPC.o Source/AutoRPC.cpp

${OBJECTDIR}/Source/LightweightDatabaseServer.o: Source/LightweightDatabaseServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/LightweightDatabaseServer.o Source/LightweightDatabaseServer.cpp

${OBJECTDIR}/Source/LightweightDatabaseCommon.o: Source/LightweightDatabaseCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/LightweightDatabaseCommon.o Source/LightweightDatabaseCommon.cpp

${OBJECTDIR}/Source/AsynchronousFileIO.o: Source/AsynchronousFileIO.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/AsynchronousFileIO.o Source/AsynchronousFileIO.cpp

${OBJECTDIR}/Source/TableSerializer.o: Source/TableSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/TableSerializer.o Source/TableSerializer.cpp

${OBJECTDIR}/Source/GetTime.o: Source/GetTime.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/GetTime.o Source/GetTime.cpp

${OBJECTDIR}/Source/ConnectionGraph.o: Source/ConnectionGraph.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ConnectionGraph.o Source/ConnectionGraph.cpp

${OBJECTDIR}/Source/Rand.o: Source/Rand.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/Rand.o Source/Rand.cpp

${OBJECTDIR}/Source/CheckSum.o: Source/CheckSum.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/CheckSum.o Source/CheckSum.cpp

${OBJECTDIR}/Source/ReplicaManager2.o: Source/ReplicaManager2.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ReplicaManager2.o Source/ReplicaManager2.cpp

${OBJECTDIR}/Source/DS_HuffmanEncodingTree.o: Source/DS_HuffmanEncodingTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DS_HuffmanEncodingTree.o Source/DS_HuffmanEncodingTree.cpp

${OBJECTDIR}/Source/PluginInterface.o: Source/PluginInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/PluginInterface.o Source/PluginInterface.cpp

${OBJECTDIR}/Source/Router.o: Source/Router.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/Router.o Source/Router.cpp

${OBJECTDIR}/Source/ConsoleServer.o: Source/ConsoleServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ConsoleServer.o Source/ConsoleServer.cpp

${OBJECTDIR}/Source/LogCommandParser.o: Source/LogCommandParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/LogCommandParser.o Source/LogCommandParser.cpp

${OBJECTDIR}/Source/RakNetworkFactory.o: Source/RakNetworkFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetworkFactory.o Source/RakNetworkFactory.cpp

${OBJECTDIR}/Source/SimpleMutex.o: Source/SimpleMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/SimpleMutex.o Source/SimpleMutex.cpp

${OBJECTDIR}/Source/RakNetTransport.o: Source/RakNetTransport.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetTransport.o Source/RakNetTransport.cpp

${OBJECTDIR}/Source/RakPeer.o: Source/RakPeer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakPeer.o Source/RakPeer.cpp

${OBJECTDIR}/Source/RakNetTypes.o: Source/RakNetTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetTypes.o Source/RakNetTypes.cpp

${OBJECTDIR}/Source/RakThread.o: Source/RakThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakThread.o Source/RakThread.cpp

${OBJECTDIR}/Source/NatPunchthrough.o: Source/NatPunchthrough.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/NatPunchthrough.o Source/NatPunchthrough.cpp

${OBJECTDIR}/Source/DS_BytePool.o: Source/DS_BytePool.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DS_BytePool.o Source/DS_BytePool.cpp

${OBJECTDIR}/Source/RakNetMemory.o: Source/RakNetMemory.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetMemory.o Source/RakNetMemory.cpp

${OBJECTDIR}/Source/StringTable.o: Source/StringTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/StringTable.o Source/StringTable.cpp

${OBJECTDIR}/Source/FileListTransfer.o: Source/FileListTransfer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FileListTransfer.o Source/FileListTransfer.cpp

${OBJECTDIR}/Source/EncodeClassName.o: Source/EncodeClassName.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/EncodeClassName.o Source/EncodeClassName.cpp

${OBJECTDIR}/Source/HTTPConnection.o: Source/HTTPConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/HTTPConnection.o Source/HTTPConnection.cpp

${OBJECTDIR}/Source/NetworkIDManager.o: Source/NetworkIDManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/NetworkIDManager.o Source/NetworkIDManager.cpp

${OBJECTDIR}/Source/RPCMap.o: Source/RPCMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RPCMap.o Source/RPCMap.cpp

${OBJECTDIR}/Source/_FindFirst.o: Source/_FindFirst.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/_FindFirst.o Source/_FindFirst.cpp

${OBJECTDIR}/Source/Itoa.o: Source/Itoa.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/Itoa.o Source/Itoa.cpp

${OBJECTDIR}/Source/ReadyEvent.o: Source/ReadyEvent.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ReadyEvent.o Source/ReadyEvent.cpp

${OBJECTDIR}/Source/ReliabilityLayer.o: Source/ReliabilityLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ReliabilityLayer.o Source/ReliabilityLayer.cpp

${OBJECTDIR}/Source/DirectoryDeltaTransfer.o: Source/DirectoryDeltaTransfer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DirectoryDeltaTransfer.o Source/DirectoryDeltaTransfer.cpp

${OBJECTDIR}/Source/RakSleep.o: Source/RakSleep.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakSleep.o Source/RakSleep.cpp

${OBJECTDIR}/Source/SystemAddressList.o: Source/SystemAddressList.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/SystemAddressList.o Source/SystemAddressList.cpp

${OBJECTDIR}/Source/FormatString.o: Source/FormatString.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FormatString.o Source/FormatString.cpp

${OBJECTDIR}/Source/PacketFileLogger.o: Source/PacketFileLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/PacketFileLogger.o Source/PacketFileLogger.cpp

${OBJECTDIR}/Source/SocketLayer.o: Source/SocketLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/SocketLayer.o Source/SocketLayer.cpp

${OBJECTDIR}/Source/DS_Table.o: Source/DS_Table.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DS_Table.o Source/DS_Table.cpp

${OBJECTDIR}/Source/EmailSender.o: Source/EmailSender.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/EmailSender.o Source/EmailSender.cpp

${OBJECTDIR}/Source/DataCompressor.o: Source/DataCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DataCompressor.o Source/DataCompressor.cpp

${OBJECTDIR}/Source/DataBlockEncryptor.o: Source/DataBlockEncryptor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/DataBlockEncryptor.o Source/DataBlockEncryptor.cpp

${OBJECTDIR}/Source/RakString.o: Source/RakString.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakString.o Source/RakString.cpp

${OBJECTDIR}/Source/LinuxStrings.o: Source/LinuxStrings.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/LinuxStrings.o Source/LinuxStrings.cpp

${OBJECTDIR}/Source/FunctionThread.o: Source/FunctionThread.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FunctionThread.o Source/FunctionThread.cpp

${OBJECTDIR}/Source/EpochTimeToString.o: Source/EpochTimeToString.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/EpochTimeToString.o Source/EpochTimeToString.cpp

${OBJECTDIR}/Source/SHA1.o: Source/SHA1.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/SHA1.o Source/SHA1.cpp

${OBJECTDIR}/Source/CommandParserInterface.o: Source/CommandParserInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/CommandParserInterface.o Source/CommandParserInterface.cpp

${OBJECTDIR}/Source/StringCompressor.o: Source/StringCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/StringCompressor.o Source/StringCompressor.cpp

${OBJECTDIR}/Source/ThreadsafePacketLogger.o: Source/ThreadsafePacketLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ThreadsafePacketLogger.o Source/ThreadsafePacketLogger.cpp

${OBJECTDIR}/Source/PacketLogger.o: Source/PacketLogger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/PacketLogger.o Source/PacketLogger.cpp

${OBJECTDIR}/Source/FullyConnectedMesh.o: Source/FullyConnectedMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/FullyConnectedMesh.o Source/FullyConnectedMesh.cpp

${OBJECTDIR}/Source/ReplicaManager.o: Source/ReplicaManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/ReplicaManager.o Source/ReplicaManager.cpp

${OBJECTDIR}/Source/SuperFastHash.o: Source/SuperFastHash.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/SuperFastHash.o Source/SuperFastHash.cpp

${OBJECTDIR}/Source/RakNetCommandParser.o: Source/RakNetCommandParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/RakNetCommandParser.o Source/RakNetCommandParser.cpp

${OBJECTDIR}/Source/LightweightDatabaseClient.o: Source/LightweightDatabaseClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/LightweightDatabaseClient.o Source/LightweightDatabaseClient.cpp

${OBJECTDIR}/Source/rijndael.o: Source/rijndael.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/rijndael.o Source/rijndael.cpp

${OBJECTDIR}/Source/Gen_RPC8.o: Source/Gen_RPC8.cpp 
	${MKDIR} -p ${OBJECTDIR}/Source
	$(COMPILE.cc) -g -o ${OBJECTDIR}/Source/Gen_RPC8.o Source/Gen_RPC8.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} Lib/GNU-Linux-x86/libraknetd.a

# Subprojects
.clean-subprojects:
