#include <stdio.h>
#ifdef _COMPATIBILITY_1
#include "Compatibility1Includes.h" // Developers of a certain platform will know what to do here.
#elif defined(_WIN32)
#include <windows.h> // Sleep and CreateProcess
#include <process.h> // system
#else
#include <unistd.h> // usleep
#include <cstdio>
#endif

// This is a simple tool to take the output of PatchApplication::restartOutputFilename
// Perform file operations while that application is not running, and then relaunch that application
void main(int argc, char **argv)
{
	// Run commands on argv[1] and launch argv[2];
	if (argc!=2)
	{
		printf("Usage: FileContainingCommands\n");
		return;
	}

	bool deleteFile=false;
	FILE *fp;
	fp = fopen(argv[1], "rt");
	if (fp==0)
	{
		printf("Error: Cannot open %s\n", argv[1]);
		return;
	}

	char buff[256];
	int offset=0;

	if (fgets(buff,255,fp)==0)
		return;
	buff[strlen(buff)]=0;
	while (buff[0])
	{
		if (strncmp(buff, "#Sleep ", 7)==0)
		{
			int sleepTime=atoi(buff+7);
#ifdef _WIN32
			Sleep(sleepTime);
#else
			usleep(sleepTime * 1000);
#endif
		}
		else if (strncmp(buff, "#DeleteThisFile", 15)==0)
			deleteFile=true;
		else if (strncmp(buff, "#CreateProcess ", 15)==0)
		{
			PROCESS_INFORMATION pi;
			STARTUPINFO si;

			// Set up the start up info struct.
			memset(&si, 0,  sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);

			// Launch the child process.
			if (!CreateProcess(
				NULL,
				buff+15,
				NULL, NULL,
				TRUE,
				CREATE_NEW_CONSOLE,
				NULL, NULL,
				&si,
				&pi))
				return;

			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}
		else
		{
            system(buff);
		}

		if (fgets(buff,255,fp)==0)
			break;
		buff[strlen(buff)]=0;
	}
	
	fclose(fp);

	// Done!
	if (deleteFile)
	{
		unlink(argv[1]);
	}
}
