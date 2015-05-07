#define INTERACTIVE

#if defined(INTERACTIVE)
#include "Kbhit.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakSleep.h"
#include "RakVoice.h"
#include "RakNetStatistics.h"
#include "GetTime.h"
#include "RakAssert.h"

#include "fmod.hpp"
#include "fmod_errors.h"

#include "FMODVoiceAdapter.h"

#if defined(_CONSOLE_2)
#include "Console2Includes.h"
#include "fmodps3.h"
#endif


// Reads and writes per second of the sound data
// Speex only supports these 3 values
#define SAMPLE_RATE  (8000)
//#define SAMPLE_RATE  (16000)
//#define SAMPLE_RATE  (32000)

#define FRAMES_PER_BUFFER  (2048 / (32000 / SAMPLE_RATE))

// define sample type. Only short(16 bits sound) is supported at the moment.
typedef short SAMPLE;

RakPeerInterface *rakPeer=NULL;
FMOD::System *fmodSystem=NULL;
RakVoice rakVoice;
bool mute;

void FMOD_ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
#if defined(INTERACTIVE)
		system("pause");
#endif
		exit(-1);
	}
}

struct myStat{
	unsigned int time;
	unsigned int bitsRec;
	unsigned int bitsSent;
};

// Keeps a record of the last 20 calls, to give a faster response to traffic change
void LogStats(){
	const int numStats = 20;
	static myStat data[numStats];

	for(int i=0; i<=numStats-2; i++){
		data[i] = data[i+1];
	}
	
	RakNetStatistics *rss=rakPeer->GetStatistics(rakPeer->GetSystemAddressFromIndex(0));
	unsigned int currTime = RakNet::GetTime();

	data[numStats-1].time = currTime;
	data[numStats-1].bitsSent = rss->totalBitsSent;
	data[numStats-1].bitsRec = rss->bitsReceived;

	float totalTime = (data[numStats-1].time - data[0].time) / 1000.f ;
	unsigned int totalBitsSent = data[numStats-1].bitsSent - data[0].bitsSent;
	unsigned int totalBitsRec = data[numStats-1].bitsRec - data[0].bitsRec;
	float bpsSent = totalBitsSent/totalTime;
	float bpsRec = totalBitsRec/totalTime;
	float avgBpsSent = rss->totalBitsSent/((currTime-rss->connectionStartTime)/1000.0f);
	float avgBpsRec = rss->bitsReceived/((currTime-rss->connectionStartTime)/1000.0f);

	printf("avgKbpsSent=%02.1f avgKbpsRec=%02.1f kbpsSent=%02.1f kbpsRec=%02.1f   \r", avgBpsSent/1000, avgBpsRec/1000, bpsSent/1000 , bpsRec/1000);
}

// Prints the current encoder parameters
void PrintParameters(void)
{
	printf("\nComplexity=%3d Noise filter=%3s VAD=%3s VBR=%3s\n"
			,rakVoice.GetEncoderComplexity()
			,(rakVoice.IsNoiseFilterActive()) ? "ON" : "OFF"
			,(rakVoice.IsVADActive()) ? "ON" : "OFF"
			,(rakVoice.IsVBRActive()) ? "ON" : "OFF");
}

int main(void)
{
	FMOD_RESULT    result;
	unsigned int   version;

	/*
	Create a System object and initialize.
	*/
	result = FMOD::System_Create(&fmodSystem);
	RakAssert(result>=0);

	result = fmodSystem->getVersion(&version);
	RakAssert(result>=0);

	if (version < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		return -1;
	}

	//  result = fmodSystem->init(100, FMOD_INIT_NORMAL, (void *)&extradriverdata);
	result = fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
	RakAssert(result>=0);
//	ERRCHECK(result);

	printf("A sample on how to use RakVoice. You need a microphone for this sample.\n");
	printf("RakVoice relies on Speex for voice encoding and decoding.\n");
	printf("See DependentExtensions/RakVoice/speex-1.1.12 for speex projects.\n");
	printf("For windows, I had to define HAVE_CONFIG_H, include win32/config.h,\n");
	printf("and include the files under libspeex, except those that start with test.\n");
	printf("Difficulty: Advanced\n\n");

	mute=false;
	bool quit;
	char ch;

	char port[256];
	rakPeer = RakNetworkFactory::GetRakPeerInterface();
#if defined(INTERACTIVE)
	printf("Enter local port: ");
	gets(port);
	if (port[0]==0)
#endif
		strcpy(port, "60000");
	SocketDescriptor socketDescriptor(atoi(port),0);

	rakPeer->Startup(4, 30, &socketDescriptor, 1);

	rakPeer->SetMaximumIncomingConnections(4);
	rakPeer->AttachPlugin(&rakVoice);

	rakVoice.Init(SAMPLE_RATE, FRAMES_PER_BUFFER*sizeof(SAMPLE));


	// Initialize our connection with FMOD
	if (!FMODVoiceAdapter::Instance()->SetupAdapter(fmodSystem, &rakVoice)){
			printf("An error occurred while initializing FMOD sounds.\n");
			exit(-1);
		}

	Packet *p;
	quit=false;
#if defined(INTERACTIVE)
	printf("(Q)uit. (C)onnect. (D)isconnect. (M)ute. ' ' for stats.\n");
	printf("(+/-)encoder complexity.  (N)oise filter on/off. (V)AD on/off. (B)vbr on/off.\n");
#else
	rakPeer->Connect("1.1.1.1", 60000, 0,0);
#endif
	PrintParameters();
	while (!quit)
	{
#if defined(INTERACTIVE)
		if (kbhit())
		{
			ch=getch();
			if (ch=='+'){
				// Increase encoder complexity
				int v = rakVoice.GetEncoderComplexity();
				if (v<10) rakVoice.SetEncoderComplexity(v+1);
				PrintParameters();
			}
			else if (ch=='-'){
				// Decrease encoder complexity
				int v = rakVoice.GetEncoderComplexity();
				if (v>0) rakVoice.SetEncoderComplexity(v-1);
				PrintParameters();
			}
			else if (ch=='n'){
				// Turn on/off noise filter
				rakVoice.SetNoiseFilter(!rakVoice.IsNoiseFilterActive());
				PrintParameters();
			}
			else if (ch=='v') {
				// Turn on/off Voice detection
				rakVoice.SetVAD(!rakVoice.IsVADActive());
				PrintParameters();
			}
			else if (ch=='b') {
				// Turn on/off VBR
				rakVoice.SetVBR(!rakVoice.IsVBRActive());
				PrintParameters();
			}
			else if (ch=='y')
			{
				quit=true;
			}
			else if (ch=='c')
			{
				char ip[256];
				printf("\nEnter IP of remote system: ");
				gets(ip);
				if (ip[0]==0)
					strcpy(ip, "127.0.0.1");
				printf("\nEnter port of remote system: ");
				gets(port);
				if (port[0]==0)
					strcpy(port, "60000");
				rakPeer->Connect(ip, atoi(port), 0,0);
			}
			else if (ch=='m')
			{
				mute=!mute;
				FMODVoiceAdapter::Instance()->SetMute(mute);
				if (mute)
					printf("\nNow muted.\n");
				else
					printf("\nNo longer muted.\n");
			}
			else if (ch=='d')
			{
				rakPeer->Shutdown(100,0);
			}
			else if (ch==' ')
			{
				char message[2048];
				RakNetStatistics *rss=rakPeer->GetStatistics(rakPeer->GetSystemAddressFromIndex(0));
				StatisticsToString(rss, message, 2);
				printf("%s", message);
			}
			else if (ch=='q')
				quit=true;
			ch=0;
		}

#endif

		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				printf("\nID_CONNECTION_REQUEST_ACCEPTED from %s\n", p->systemAddress.ToString());
				rakVoice.RequestVoiceChannel(p->systemAddress);
			}
			else if (p->data[0]==ID_RAKVOICE_OPEN_CHANNEL_REQUEST)
			{
				printf("\nOpen Channel request from %s\n", p->systemAddress.ToString());
			}
			else if (p->data[0]==ID_RAKVOICE_OPEN_CHANNEL_REPLY)
			{
				printf("\nGot new channel from %s\n", p->systemAddress.ToString());
			}

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		fmodSystem->update();
		// Update or connection with FMOD
		FMODVoiceAdapter::Instance()->Update();
		LogStats();
		RakSleep(20);

	}

	// Release any FMOD resources we used, and shutdown FMOD itself
	FMODVoiceAdapter::Instance()->Release();
	fmodSystem->release();

	rakPeer->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	return 0;
}
