#pragma once

// This is a class to read data from a .ini file
class CIniReader {
	public:
		// Initialize the IniReader
		CIniReader(char* szFileName);

		// Read an integer from ini file
		int ReadInteger(char* szSection, char* szKey, int iDefaultValue);

		// Read a float from ini file
		float ReadFloat(char* szSection, char* szKey, float fltDefaultValue);

		// Read a boolean from ini file
		bool ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue);

		// Read a string from ini file
		char* ReadString(char* szSection, char* szKey, const char* szDefaultValue);
	private:
		char m_szFileName[255]; // Filename must only be up to 255 characters long
};