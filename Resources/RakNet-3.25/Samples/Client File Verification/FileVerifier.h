#ifndef __FILE_VERIFIER_H
#define __FILE_VERIFIER_H

#include "DS_List.h"
#include "BitStream.h"

class FileVerifier
{
public:
	FileVerifier();
	~FileVerifier();

	// Serializes all files that have checksums generated for them.
	// Used by the sender
	void Serialize(RakNet::BitStream *bitStream);
	
	// Returns the name of the file that failed
	// Deserializes a set of files from Serialize
	// On failure, return the filename of the file that failed or "Invalid bitstream"
	// Returns 0 on success
	char* DeserializeAndValidate(RakNet::BitStream *bitStream);

	// Adds a file to be verified.
	// Required file - the sender must serialize this file in the list of files or validation fails
	// Otherwise, the file must match if it exists - otherwise it is ignored.
    void AddFileForVerification(char *filename, bool requiredFile);

	// Overridable - called from DeserializeAndValidate when a file that is not known about is serialized.
	// Return true on allow the file, false to fail validation
	virtual bool OnUnknownFile(char *filename);

private:
	// Returns the index where the file was found, or -1 if not found
    int FindFileInList(char *filename);
	struct FileWithCheckSum
	{
		char fileName[256];
		unsigned int checkSum;
		bool fileIsRequired;
		bool fileFound;
	};
	DataStructures::List<FileWithCheckSum*> fileList;
};

#endif

