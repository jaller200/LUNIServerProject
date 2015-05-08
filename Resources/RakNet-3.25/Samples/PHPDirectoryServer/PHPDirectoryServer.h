/// \file
/// \brief Contains WebGameList, a client for communicating with a HTTP list of game servers
///
/// This file is part of RakNet Copyright 2008 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.jenkinssoftware.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __WEB_GAME_LIST
#define __WEB_GAME_LIST

#include "Export.h"
#include "RakString.h"
#include "HTTPConnection.h"
#include "RakNetTypes.h"
#include "DS_Queue.h"
#include "DS_Table.h"
#include "DS_Map.h"

struct SystemAddress;

/// \brief Use PHPDirectoryServer as a C++ client to DirectoryServer.php
///
/// Initialize with the constructor, associating an instance of HTTPConnection with this class. HTTPConnection itself should be associated with TCPInterface.
/// Use SetField() to set fields, and UploadTable() to upload the fields to the web server, including reserved columns
/// Use DownloadTable() to get the table, which will be returned through GetLastDownloadedTable() as soon as ProcessDataPacket() returns an empty string
/// Call Update() periodically
/// Pass packets to this class with ProcessDataPacket(). These packets should be returned by TCPInterface on a connection handled by an instance of HTTPInterface. See main.cpp in the sample PHPDirectoryServer for how this is done.
///
class RAK_DLL_EXPORT PHPDirectoryServer : public RakNet::RakMemoryOverride
{
public:
	/// \param[in] http This should be initialized with the server you
    ///              will be communicating with
    /// \param[in] path The path on the remote server to the WebGameList
    ///             PHP script. For example, RakNet/DirectoryServer.php
    PHPDirectoryServer(HTTPConnection& http, const char *path);

    /// Set a parameter (these are passed to the server)
	/// To delete a column, just pass an empty string for value
	/// Store the game name and port with UploadTable, rather than SetField, as these columns are required and use reserved column names
	/// \param[in] columnName The name of the column to store
	/// \param[in] value What value to hold for the uploaded row (only one row can be uploaded at a time)
	void SetField(RakNet::RakString columnName, RakNet::RakString value);

	/// Set all parameters at once from a table
	void SetFields(DataStructures::Table *table);

	/// Clear all fields
	void ClearFields(void);

	/// Upload table that was populated via calls to SetField() or SetFields()
	/// If http.IsFinished()==false, then the upload will occur when it returns true
	/// Otherwise, the upload will occur the next time we call Update()
	/// Your sending IP address and gamePort form a unique row ID on the webpage.
	/// \param[in] timeoutSeconds How long to persist the entry, in seconds. As long as Update() is called, the entry will be refreshed at least this often. A lower value finds crashes faster, but requires more bandwidth and page updates. 1-5 minutes is a good default.
	/// \param[in] gameName Name of the game to list on the webpage
	/// \param[in] gamePort Port your game is running on
	/// \param[in] password Password required to update the webpage
	void UploadTable(RakNetTime timeoutSeconds, RakNet::RakString gameName, unsigned short gamePort, RakNet::RakString password);

	/// Request an update to the game list.
	/// Get it by calling GetLastDownloadedTable();
	void DownloadTable(RakNet::RakString password);

	/// When HTTPConnection::ProcessDataPacket() returns true, and not an error, pass HTTPConnection::Read() to this function
	/// The message will be parsed into DataStructures::Table, and a copy stored internally which can be retrieved by GetLastDownloadedTable();
	/// \param[in] packetData Returned from HTTPInterface::Read()
	/// \return Empty string for success, "Deleted" for successful deletion, "N/A" if the packet does not contain any tables, "Got Configuration" if we downloaded configuration info, error or status message otherwise
	RakNet::RakString ProcessHTTPRead(RakNet::RakString httpRead);

	/// Returns the last value returned from ProcessHTTPString
	/// \return The table created by parsing httpString
	const DataStructures::Table *GetLastDownloadedTable(void) const;

	/// Call this periodically - it will handle refreshing updates to the server
	void Update(void);

    // escape supplied strings so they don't contain the field and
    // column delimiters and handle newlines and carriage returns
    static void PHPDirectoryServer::Escape(RakNet::RakString &str);
    static void PHPDirectoryServer::Unescape(RakNet::RakString &str);
    
private:
    HTTPConnection &http;
    
	/// outgoing fields
    DataStructures::Map<RakNet::RakString, RakNet::RakString> fields;
    
    bool downloadTableRequested;
    bool waitingForReply;
	RakNetTime lastUpdate;
	RakNetTime maxTimeoutAllowed;
    size_t maxCharactersInARecord;
    RakNet::RakString path;


	RakNet::RakString gameNameParam;
	RakNet::RakString passwordParam;
	unsigned short gamePortParam;
	RakNetTime timeoutParam;
	bool needsUpdate;

	DataStructures::Table lastDownloadedTable;

	/// Parses out text from the body of an HTTP reply, storing configuration varaibles
	/// Returns a pointer to the table itself
	const char * HandleReply(RakNet::RakString &body, RakNet::RakString &resultString);

};

#endif

