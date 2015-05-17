#include "IniReader.h"
#include <iostream>
#include <Windows.h>

// Initialize the IniReader with the filename
CIniReader::CIniReader(char* szFileName) {
	memset(m_szFileName, 0x00, 255); // Set the memory to 255 bytes from 0x00
	memcpy(m_szFileName, szFileName, strlen(szFileName)); // Copy the filename into the memory
}

// Read an integer from the ini file
int CIniReader::ReadInteger(char* szSection, char* szKey, int iDefaultValue) {
	int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, m_szFileName); // Read the integer from the file
	return iResult; // Return the integer
}

// Read a float from the ini file
float CIniReader::ReadFloat(char* szSection, char* szKey, float fltDefaultValue) {
	char szResult[255]; // Allocate 255 bytes for the result
	char szDefault[255]; // Allocate 255 bytes for the default
	float fltResult; // Create the float variable
	sprintf_s(szDefault, "%f", fltDefaultValue); // Copy the default value into szDefault
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); // Read the value from the file
	fltResult = atof(szResult); // Convert the resulting string into a float
	return fltResult; // Return the float
}

// Read a boolean from the ini file
bool CIniReader::ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue) {
	char szResult[255]; // Allocate 255 bytes for the result 
	char szDefault[255]; // Allocate 255 bytes for the default value
	bool bolResult; // Create the bolResult variable
	sprintf_s(szDefault, "%s", bolDefaultValue ? "True" : "False"); // Copy the default value into szDefault
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); // Read the bool from the ini file
	bolResult = (strcmp(szResult, "True") == 0 ||
		strcmp(szResult, "true") == 0) ? true : false; // Set the bool to "true" or "false"
	return bolResult; // Return the boolean
}

// Read char* from the ini file
char* CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue) {
	char* szResult = new char[255]; // Allocate a char array of 255 chars
	memset(szResult, 0x00, 255); // Set the memory to 255 bytes
	GetPrivateProfileString(szSection, szKey, // Read the string into the szResult
		szDefaultValue, szResult, 255, m_szFileName);
	return szResult; // Return the string as a char array
}