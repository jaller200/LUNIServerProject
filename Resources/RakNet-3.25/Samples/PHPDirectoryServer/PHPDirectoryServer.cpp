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
/// Software Foundation

#include "PHPDirectoryServer.h"
#include "HTTPConnection.h"
#include "RakSleep.h"
#include "RakString.h"
#include "RakNetTypes.h"
#include "GetTime.h"
#include "RakAssert.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

// Column with this header contains the name of the game, passed to UploadTable()
static const char *GAME_NAME_COMMAND="__GAME_NAME";
// Column with this header contains the port of the game, passed to UploadTable()
static const char *GAME_PORT_COMMAND="__GAME_PORT";
// Column with this header contains the IP address of the game, passed to UploadTable()
static const char *SYSTEM_ADDRESS_COMMAND="__SystemAddress";
// Returned from the PHP server indicating when this row was last updated.
static const char *LAST_UPDATE_COMMAND="__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE";
// Command to delete a row. Internal
static const char *DELETEME_COMMAND="__DELETE_ROW";
// Column passed to the PHP server as the password. Internal
static const char *GAME_PASSWORD_COMMAND="__PHP_DIRECTORY_SERVER_PASSWORD";
// Column passed to the PHP server as how long to list this server before auto expiring. Internal
static const char *GAME_TIMEOUT_COMMAND="__GAME_LISTING_TIMEOUT";

using namespace RakNet;
using namespace DataStructures;

PHPDirectoryServer::PHPDirectoryServer(HTTPConnection& _http, const char *_path)
    : http(_http), path(_path), maxTimeoutAllowed(120), maxCharactersInARecord(5000),
    waitingForReply(false), downloadTableRequested(false), lastUpdate(0) 
{
    Map<RakString, RakString>::IMPLEMENT_DEFAULT_COMPARISON();
	needsUpdate=false;
}

void PHPDirectoryServer::Escape(RakString &str)
{
    unsigned int i = (unsigned int) str.GetLength();

    while(i--)
        {
        unsigned char old = str[i];
        if(str[i] == '\001' || str[i] == '\002')
            str.Replace(i, 1, ' ');
        else if(str[i] == '\n')
            str.Replace(i, 1, '\r');
        }
}

void PHPDirectoryServer::Unescape(RakString &str)
{
    unsigned int i = (unsigned int) str.GetLength();

    while(i--)
        if(str[i] == '\r')
            str.Replace(i, 1, '\n');
}

void PHPDirectoryServer::SetField( RakNet::RakString columnName, RakNet::RakString value )
{
	if (columnName.IsEmpty())
		return;

	if (columnName==DELETEME_COMMAND ||
		columnName==GAME_NAME_COMMAND ||
		columnName==GAME_PORT_COMMAND ||
		columnName==GAME_PASSWORD_COMMAND)
	{
		RakAssert("PHPDirectoryServer::SetField attempted to set reserved column name" && 0);
		return;
	}

    Escape(columnName);
    Escape(value);
    if(! value.IsEmpty())
        fields.Set(columnName, value);
    else
        fields.Delete(columnName);
}
void PHPDirectoryServer::SetFields(DataStructures::Table *table)
{
	ClearFields();

	unsigned columnIndex, rowIndex;
	DataStructures::Table::Row *row;
	
	for (rowIndex=0; rowIndex < table->GetRowCount(); rowIndex++)
	{
		row = table->GetRowByIndex(rowIndex, 0);
		for (columnIndex=0; columnIndex < table->GetColumnCount(); columnIndex++)
		{
			SetField( table->ColumnName(columnIndex), row->cells[columnIndex]->ToString(table->GetColumnType(columnIndex)) );
		}
	}
}

void PHPDirectoryServer::ClearFields(void)
{
	fields.Clear();
}

void PHPDirectoryServer::UploadTable(RakNetTime timeoutSeconds, RakNet::RakString gameName, unsigned short gamePort,RakNet::RakString password)
{
	timeoutParam=timeoutSeconds;
	gameNameParam=gameName;
	passwordParam=password;
	gamePortParam=gamePort;
	needsUpdate=true;
}


void PHPDirectoryServer::DownloadTable(RakNet::RakString password)
{
	passwordParam=password;
	downloadTableRequested = true;
}


RakNet::RakString PHPDirectoryServer::ProcessHTTPRead(RakNet::RakString httpRead)
{
	RakNet::RakString resultString;
	const char *c = HandleReply(httpRead, resultString);
	
	if (c==0 || *c==0)
		return resultString;

	// add each line to the table
	DataStructures::Table::Row *row = 0;

	RakString IPAddress;
	if (*c=='\n')
		c++;

	// Add a binary column to represent the system address 
	unsigned col = lastDownloadedTable.ColumnIndex(SYSTEM_ADDRESS_COMMAND);

	while(c && *c)
	{
		const char *c2 = c;
		if (row==0)
		{
			if (col!=(unsigned)-1)
			{
				// First pass to find IP and port
				while (c2 && *c2)
				{
					const char *delim = strpbrk(c2,"\n\001");
					const char *delim2 = strpbrk(c2,"\002");

					if(delim==0 || delim2==0)
						return RakNet::RakString();

					// if the field name and value are more than 1 character
					if(delim2 > c2 + 1 && delim > delim2 + 1)
					{
						RakString column("%.*s", delim2 - c2, c2);

						PHPDirectoryServer::Unescape(column);

						if (column==SYSTEM_ADDRESS_COMMAND)
						{
							RakString value("%.*s", delim - delim2 - 1, delim2 + 1);
							PHPDirectoryServer::Unescape(value);
							IPAddress=value;

							// Use the IP address and port combination as the unique key into the downloaded table
							unsigned columnIndicesSubset[1];
							columnIndicesSubset[0]=col;
							DataStructures::Table::Cell cell;
							cell.Set(IPAddress);
							DataStructures::Table::FilterQuery inclusionFilters[1];
							inclusionFilters[0].columnIndex=col;
							inclusionFilters[0].columnName[0]=0;
							inclusionFilters[0].operation=DataStructures::Table::QF_EQUAL;
							inclusionFilters[0].cellValue=&cell;
							DataStructures::Table result;
							lastDownloadedTable.QueryTable(columnIndicesSubset, 1, inclusionFilters, 1, 0, 0, &result);
							// If our query for this binary address already exists, get that row into the existing table by the key returned from the query. Else add a new row with the first available ID
							unsigned key;
							if (result.GetRowByIndex(0, &key))
							{
								row = lastDownloadedTable.GetRowByID(key);
							}
							else
							{
								row = lastDownloadedTable.AddRow(lastDownloadedTable.GetAvailableRowId());
								row->UpdateCell(col, IPAddress);
							}
							break;
						}
					}

					if(*delim == '\n')
						break;

					c2 = (char*) delim + 1;
				}
			}
		}
		

		const char *delim = strpbrk(c,"\n\001");
		const char *delim2 = strpbrk(c,"\002");

		if(! delim)
			break;

		// if the field name and value are more than 1 character
		if(delim2 > c + 1 && delim > delim2 + 1)
		{
			RakString column("%.*s", delim2 - c, c);
			RakString value("%.*s", delim - delim2 - 1, delim2 + 1);

			PHPDirectoryServer::Unescape(column);
			PHPDirectoryServer::Unescape(value);

			unsigned col = lastDownloadedTable.ColumnIndex(column);
			if(col == (unsigned)-1)
				col = lastDownloadedTable.AddColumn(column, DataStructures::Table::STRING);

			if (row==NULL)
				row = lastDownloadedTable.AddRow(lastDownloadedTable.GetAvailableRowId());
			row->UpdateCell(col,value.C_String());
		}

		if(*delim == '\n')
		{
			// Got another row
			row = NULL;
		}

		c = (char*) delim + 1;

	}

	return RakNet::RakString();
}
const DataStructures::Table *PHPDirectoryServer::GetLastDownloadedTable(void) const
{
	return &lastDownloadedTable;
}

void PHPDirectoryServer::Update(void)
{
	if (http.IsBusy())
		return;

	RakNetTime time = GetTime();
	RakNetTime nextUpdate;
	if (timeoutParam < 5)
		nextUpdate = lastUpdate + timeoutParam*1000/2;		
	else
		nextUpdate = lastUpdate + timeoutParam*1000 - 2000;

    // make sure we update our status before the timeout expires
    if(needsUpdate ||
		(fields.Size()>0 && time > nextUpdate))
    {
		RakString outgoing;
		char buff[64];
		if (timeoutParam==0)
		{
			outgoing += DELETEME_COMMAND;
			outgoing += "=true&";
		}
		else
		{
			for (unsigned i = 0; i < fields.Size(); i++)
			{
				RakString value = fields[i];
				value.URLEncode();
				outgoing += RakString("%s=%s&",
									  fields.GetKeyAtIndex(i).C_String(),
									  value.C_String());
			}

			outgoing += GAME_TIMEOUT_COMMAND;
			outgoing += "=";
			outgoing += itoa(timeoutParam,buff,10);
			outgoing += "&";
		}

		RakString encodedPassword = passwordParam;
		encodedPassword.URLEncode();

		outgoing += GAME_NAME_COMMAND;
		outgoing += "=";
		outgoing += gameNameParam;
		outgoing += "&";
		outgoing += GAME_PORT_COMMAND;
		outgoing += "=";
		outgoing += itoa(gamePortParam,buff,10);
		outgoing += "&";
		outgoing += GAME_PASSWORD_COMMAND;
		outgoing += "=";
		outgoing += encodedPassword;
		//outgoing += "\001";

		//http.Post(RakString("%s?update", path.C_String()), outgoing, "application/octet-stream");
		//outgoing.URLEncode();
		http.Post(RakString("%s?update", path.C_String()), outgoing, "application/x-www-form-urlencoded");
        waitingForReply = true;
		needsUpdate=false;
        lastUpdate = time;
    }
    else if(downloadTableRequested)
    {
		RakString outgoing;
		RakString encodedPassword = passwordParam;
		encodedPassword.URLEncode();
		outgoing += GAME_PASSWORD_COMMAND;
		outgoing += "=";
		outgoing += encodedPassword;
		//outgoing += "\001";
        //http.Post(RakString("%s?query", path.C_String()), outgoing, "application/octet-stream");
		http.Post(RakString("%s?query", path.C_String()), outgoing, "application/x-www-form-urlencoded");
        waitingForReply = true;
        downloadTableRequested = false;
    }
    else
    {
        waitingForReply = false;
    }
}

const char * PHPDirectoryServer::HandleReply(RakString &body, RakNet::RakString &resultString)
{
    const char *c = (const char*) body.C_String(); // current position
	bool gotConfiguration=false;

    while(c && *c)
    {
        if(*c == '\n')
            ++c;
        
        if(0 == strncmp(c,"max_length: ",12))
        {
            maxCharactersInARecord = atoi(c + 12);
			gotConfiguration=true;
        }
		else if (0 == strncmp(c,"<b>Fatal error</b>:", 16))
		{
			resultString=c;
			return 0;
		}
        else if(0 == strncmp(c,"max_timeout: ",13))
        {
            RakNetTime newTO = atoi(c + 13);
            if(newTO > 0)
                maxTimeoutAllowed = maxTimeoutAllowed > newTO ? newTO : maxTimeoutAllowed;
			gotConfiguration=true;
        }
        else if(0 == strncmp(c,"Game List",8))
        {
            c = strstr(c,"\n");
            return c;
        }
        else if(0 == strncmp(c,"Entry Deleted",13))
        {
//            WebPacket deleted(true);
  //          deleted.code = HTTPConnection::Deleted;
    //        packets.Push(deleted);
			resultString = "Deleted";
            return 0;
        }

        c = strstr(c,"\n");
    }
    
	if (gotConfiguration)
		resultString="Got Configuration";
	else
		resultString="N/A";
    return c;
}
