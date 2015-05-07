#include "FileVerifier.h"
#include "StringCompressor.h"
#include "CheckSum.h"
#include <assert.h>
#include <cstring>
#include <sys/stat.h>
#include <cstdio>

FileVerifier::FileVerifier()
{
}

FileVerifier::~FileVerifier()
{
	unsigned i;
	for (i=0; i < fileList.Size(); i++)
		delete fileList[i];
	fileList.Clear();
}

void FileVerifier::Serialize(RakNet::BitStream *bitStream)
{
	unsigned i;
	
	bitStream->WriteCompressed(fileList.Size());

	for (i=0; i < fileList.Size(); i++)
	{
		stringCompressor->EncodeString(fileList[i]->fileName, 256, bitStream);
		bitStream->Write(fileList[i]->checkSum);
	}
}

char* FileVerifier::DeserializeAndValidate(RakNet::BitStream *bitStream)
{
	unsigned serializedDataSize;
	unsigned fileListIndex;
	int foundFileIndex;
	char filename[256];
	static char failedFilename[256];
	unsigned int checkSum;

	// For each file that is required on the server, set fileFound to false
	for (fileListIndex=0; fileListIndex < fileList.Size(); fileListIndex++)
		if (fileList[fileListIndex]->fileIsRequired)
			fileList[fileListIndex]->fileFound=false;

	// Parse the incoming data
	if (bitStream->ReadCompressed(serializedDataSize)==false)
	{
#ifdef _DEBUG
		assert(0); // Invalid serialized data
#endif
		return "Invalid bitstream";
	}

	filename[0]=0;
	failedFilename[0]=0;
	
	// Go through the list of serialized files.  For each file:
	// If this file is totally not in the server list, then validate it with OnUnknownFile
	// If the file is in the server list, then set fileFound to true
	while (serializedDataSize>0)
	{
		stringCompressor->DecodeString(filename, 256, bitStream);
		if (filename[0]==0 || bitStream->Read(checkSum)==false)
		{
#ifdef _DEBUG
			assert(0); // Invalid serialized data
#endif
			if (failedFilename[0])
				return failedFilename;
			else
				return "Invalid bitstream";
		}

		if (failedFilename[0]==0)
		{
			foundFileIndex=FindFileInList(filename);
			if (foundFileIndex!=-1)
			{
				if (fileList[foundFileIndex]->checkSum!=checkSum)
				// On failure, keep reading to get through the rest of the bitstream so any following
				// data is still usable.
					strcpy(failedFilename, filename); 
				else
				{
					printf("File OK: %s\n", filename);
					fileList[foundFileIndex]->fileFound=true;
				}
			}
			else
			{
				// True means ignore the file.  False means fail
				if (OnUnknownFile(filename)==false)
				{
					strcpy(failedFilename, filename); 
				}
			}
		}

		serializedDataSize--;
	}

	if (failedFilename[0])
		return failedFilename;

	// Go through the file list.  If a file is required and was not found, return that filename
	for (fileListIndex=0; fileListIndex < fileList.Size(); fileListIndex++)
	{
		if (fileList[fileListIndex]->fileIsRequired && fileList[fileListIndex]->fileFound==false)
		{
			strcpy(failedFilename, filename); 
			return failedFilename;
		}
	}

	// Success
	return 0;
}

void FileVerifier::AddFileForVerification(char *filename, bool requiredFile)
{
	CheckSum checkSum;
	FILE *fp;
	unsigned i;
	char *fileData;
	FileWithCheckSum *fwcs;
#ifndef WIN32
#define _stat stat
#endif
	struct _stat fileInfo;

	if (filename==0 || strlen(filename)>=256)
	{
		assert(0);
		return;
	}

	if (_stat(filename, &fileInfo)==-1)
	{
		printf("File could not be opened: %s\n", filename);
		return;
	}

	printf("File added to verify list: %s\n", filename);
	
	fp = fopen(filename, "rb");
#ifdef _DEBUG
	assert(fp);
#endif
	if (fp==0)
		return;

	fileData = new char [fileInfo.st_size];
	fread(fileData, 1, fileInfo.st_size, fp);
	fclose(fp);

	for (i=0; (int)i < fileInfo.st_size; i++)
		checkSum.Add((unsigned char)(fileData[i]));

	delete [] fileData;

	fwcs=new FileWithCheckSum;
	strcpy(fwcs->fileName, filename);
	fwcs->fileIsRequired=requiredFile;
	fwcs->checkSum=checkSum.Get();
	fileList.Insert(fwcs);
}

int FileVerifier::FindFileInList(char *filename)
{
	unsigned i;
	for (i=0; i < fileList.Size(); i++)
		if (strncmp(fileList[i]->fileName, filename, 256)==0)
			return (int)i;

	return -1;
}

bool FileVerifier::OnUnknownFile(char *filename)
{
	return true;
}
