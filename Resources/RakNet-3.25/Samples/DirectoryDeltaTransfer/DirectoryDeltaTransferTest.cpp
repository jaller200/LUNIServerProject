#include "RakNetworkFactory.h"
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetStatistics.h"
#include "DirectoryDeltaTransfer.h"
#include "FileListTransfer.h"
#include <cstdio>
#include <stdlib.h>
#include "Kbhit.h"
#include "FileList.h"
#include "DataCompressor.h"
#include "FileListTransferCBInterface.h"
#include "RakSleep.h"

#ifdef _WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h> // usleep
#endif

class TestCB : public FileListTransferCBInterface
{
public:
	bool OnFile(
		OnFileStruct *onFileStruct)
	{
        printf("%i. (100%%) %i/%i %s %ib->%ib / %ib->%ib\n", onFileStruct->setID, onFileStruct->fileIndex+1, onFileStruct->setCount, onFileStruct->fileName, onFileStruct->compressedTransmissionLength, onFileStruct->finalDataLength, onFileStruct->setTotalCompressedTransmissionLength, onFileStruct->setTotalFinalLength);

		// Return true to have RakNet delete the memory allocated to hold this file.
		// False if you hold onto the memory, and plan to delete it yourself later
		return true;
	}

	virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength, char *firstDataChunk)
	{
		printf("%i (%i%%) %i/%i %s %ib->%ib / %ib->%ib\n", onFileStruct->setID, 100*partCount/partTotal, onFileStruct->fileIndex+1, onFileStruct->setCount, onFileStruct->fileName, onFileStruct->compressedTransmissionLength, onFileStruct->finalDataLength, onFileStruct->setTotalCompressedTransmissionLength, onFileStruct->setTotalFinalLength, firstDataChunk);
	}

	virtual bool OnDownloadComplete(void)
	{
		printf("Download complete.\n");

		// Returning false automatically deallocates the automatically allocated handler that was created by DirectoryDeltaTransfer
		return false;
	}

} transferCallback;


int main(void)
{
	char ch;
	RakPeerInterface *rakPeer;

	// directoryDeltaTransfer is the main plugin that does the work for this sample.
	DirectoryDeltaTransfer directoryDeltaTransfer;
	// The fileListTransfer plugin is used by the DirectoryDeltaTransfer plugin and must also be registered (you could use this yourself too if you wanted, of course).
	FileListTransfer fileListTransfer;

	rakPeer = RakNetworkFactory::GetRakPeerInterface();
	rakPeer->AttachPlugin(&directoryDeltaTransfer);
	rakPeer->AttachPlugin(&fileListTransfer);
	// Get download progress notifications.  Handled by the plugin.
	rakPeer->SetSplitMessageProgressInterval(1);
	directoryDeltaTransfer.SetFileListTransferPlugin(&fileListTransfer);

	printf("This sample demonstrates the plugin to incrementally transfer compressed\n");
	printf("deltas of directories.  In essence, it's a simple autopatcher.\n");
	printf("Unlike the full autopatcher, it has no dependencies.  It is suitable for\n");
	printf("patching from non-dedicated servers at runtime.\n");
	printf("Difficulty: Intermediate\n\n");

	printf("Enter listen port. Enter for default. If running two instances on the\nsame computer, use 0 for the client.\n");
	unsigned short localPort;
	char str[256];
	gets(str);
	if (str[0]==0)
		localPort=60000;
	else
		localPort=atoi(str);
	SocketDescriptor socketDescriptor(localPort,0);
	if (rakPeer->Startup(8,30,&socketDescriptor, 1)==false)
	{
		RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
		printf("RakNet initialize failed.  Possibly duplicate port.\n");
		return 1;
	}
	rakPeer->SetMaximumIncomingConnections(8);

	printf("Commands:\n");
	printf("(S)et application directory.\n");
	printf("(A)dd allowed uploads from subdirectory.\n");
	printf("(D)ownload from subdirectory.\n");
	printf("(C)lear allowed uploads.\n");
	printf("C(o)nnect to another system.\n");
	printf("(Q)uit.\n");

	RakNetTime nextStatTime = RakNet::GetTime() + 1000;

	Packet *p;
	while (1)
	{
		/*
		if (//directoryDeltaTransfer.GetNumberOfFilesForUpload()>0 &&
			RakNet::GetTime() > nextStatTime)
		{
			// If sending, periodically show connection stats
			char statData[2048];
			RakNetStatistics *statistics = rakPeer->GetStatistics(rakPeer->GetSystemAddressFromIndex(0));
		//	if (statistics->messagesOnResendQueue>0 || statistics->internalOutputQueueSize>0)
			if (rakPeer->GetSystemAddressFromIndex(0)!=UNASSIGNED_SYSTEM_ADDRESS)
			{
				StatisticsToString(statistics, statData, 2);
				printf("%s\n", statData);
			}
			
			nextStatTime=RakNet::GetTime()+5000;
		}
		*/

		// Process packets
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
				printf("ID_NEW_INCOMING_CONNECTION\n");
			else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
			else if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
				printf("ID_DISCONNECTION_NOTIFICATION\n");
			else if (p->data[0]==ID_CONNECTION_LOST)
				printf("ID_CONNECTION_LOST\n");

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (kbhit())
		{
			ch=getch();
			if (ch=='s')
			{
				printf("Enter application directory\n");
				gets(str);
				if (str[0]==0)
					strcpy(str, "C:/Temp");
				directoryDeltaTransfer.SetApplicationDirectory(str);
				printf("This directory will be prefixed to upload and download subdirectories.\n");
			}
			else if (ch=='a')
			{
				printf("Enter uploads subdirectory\n");
				gets(str);
				directoryDeltaTransfer.AddUploadsFromSubdirectory(str);
				printf("%i files for upload.\n", directoryDeltaTransfer.GetNumberOfFilesForUpload());
			}
			else if (ch=='d')
			{
				char subdir[256];
				char outputSubdir[256];
				printf("Enter remote subdirectory to download from.\n");
				printf("This directory may be any uploaded directory, or a subdir therein.\n");
				gets(subdir);
				printf("Enter subdirectory to output to.\n");
				gets(outputSubdir);
                
				unsigned short setId;
				setId=directoryDeltaTransfer.DownloadFromSubdirectory(subdir, outputSubdir, true, rakPeer->GetSystemAddressFromIndex(0), &transferCallback, HIGH_PRIORITY, 0, 0);
				if (setId==(unsigned short)-1)
					printf("Download failed.  Host unreachable.\n");
				else
					printf("Downloading set %i\n", setId);
			}
			else if (ch=='c')
			{
				directoryDeltaTransfer.ClearUploads();
				printf("Uploads cleared.\n");
			}
			else if (ch=='o')
			{
				char host[256];
				printf("Enter host IP: ");
				gets(host);
				if (host[0]==0)
					strcpy(host, "127.0.0.1");
				unsigned short remotePort;
				printf("Enter host port: ");
				gets(str);
				if (str[0]==0)
					remotePort=60000;
				else
					remotePort=atoi(str);
				rakPeer->Connect(host, remotePort, 0, 0);
				printf("Connecting.\n");
			}
			else if (ch=='q')
			{
				printf("Bye!\n");
				rakPeer->Shutdown(1000,0);
				break;
			}
		}

		// Keeps the threads responsive
		RakSleep(0);
	}

	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 0;
}
