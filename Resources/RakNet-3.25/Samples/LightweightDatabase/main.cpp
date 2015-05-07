#include "LightweightDatabaseServer.h"
#include "LightweightDatabaseClient.h"
#include "TableSerializer.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "DS_Table.h"
#include "LightweightDatabaseClient.h"
#include "LightweightDatabaseServer.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "RakSleep.h"
#include "MessageIdentifiers.h"
#include "Kbhit.h"
#include <stdlib.h>

void main(void)
{
	char ch;
	bool isServer;
	LightweightDatabaseServer databaseServer;
	LightweightDatabaseClient databaseClient;
	RakPeerInterface *rakPeer = RakNetworkFactory::GetRakPeerInterface();
	char str[256];
	char columnName[256];
	char tableName[256], tablePassword[256];
	printf("(S)erver or (C)lient?\n");
	ch=getch();
	if (ch=='s')
	{
		isServer=true;
		rakPeer->SetMaximumIncomingConnections(32);
		SocketDescriptor socketDescriptor(12345,0);
		rakPeer->Startup(32, 0, &socketDescriptor, 1 );
		rakPeer->AttachPlugin(&databaseServer);
		printf("Server started\n");
		printf("(C)reate table\n");
		printf("(R)emove table\n");
	}
	else
	{
		isServer=false;
		SocketDescriptor socketDescriptor(0,0);
		rakPeer->Startup(1, 0, &socketDescriptor, 1);
		rakPeer->AttachPlugin(&databaseClient);
		printf("Client started\n");

		printf("Enter server IP: ");
		gets(str);
		if (str[0]==0)
			strcpy(str, "127.0.0.1");
		printf("Connecting to server.\n");
		rakPeer->Connect(str, 12345, 0, 0);
		printf("(Q)uery table\n");
		printf("(U)pdate row\n");
		printf("(R)emove row\n");
	}
	printf("(E)xit\n");

	Packet *p;
	while (1)
	{
		p=rakPeer->Receive();
		while (p)
		{
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION)
				printf("ID_DISCONNECTION_NOTIFICATION\n");
			else if (p->data[0]==ID_CONNECTION_LOST)
				printf("ID_CONNECTION_LOST\n");
			else if (p->data[0]==ID_NO_FREE_INCOMING_CONNECTIONS)
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			else if (p->data[0]==ID_NEW_INCOMING_CONNECTION)
				printf("ID_NEW_INCOMING_CONNECTION\n");
			else if (p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
				printf("ID_CONNECTION_ATTEMPT_FAILED\n");
			else if (p->data[0]==ID_DATABASE_UNKNOWN_TABLE)
				printf("ID_DATABASE_UNKNOWN_TABLE\n");
			else if (p->data[0]==ID_DATABASE_INCORRECT_PASSWORD)
				printf("ID_DATABASE_INCORRECT_PASSWORD\n");
			else if (p->data[0]==ID_DATABASE_QUERY_REPLY)
			{
				printf("Incoming table:\n");
				DataStructures::Table table;
				if (TableSerializer::DeserializeTable(p->data+sizeof(MessageID), p->length-sizeof(MessageID), &table))
				{
					DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *cur = table.GetListHead();
					unsigned i;

					printf("Columns:\n");
					for (i=0; i < table.GetColumns().Size(); i++)
					{
						printf("%i. %s : ", i+1, table.GetColumns()[i].columnName);
						if (table.GetColumns()[i].columnType==DataStructures::Table::BINARY)
							printf("BINARY");
						else if (table.GetColumns()[i].columnType==DataStructures::Table::NUMERIC)
							printf("NUMERIC");
						else
							printf("STRING");
						printf("\n");
					}
					if (cur)
						printf("Rows:\n");
					else
						printf("Table has no rows.\n");
					while (cur)
					{
						for (i=0; i < (unsigned)cur->size; i++)
						{
							table.PrintRow(str, 256, ',', true, cur->data[i]);
							printf("RowID %i: %s\n", cur->keys[i], str );
						}
						cur=cur->next;
					}
				}
				else
					printf("Deserialization of table failed.\n");
			}

			rakPeer->DeallocatePacket(p);
			p=rakPeer->Receive();
		}

		if (kbhit())
		{
			char ch=getch();
			if (isServer)
			{
				if (ch=='c')
				{
					bool allowRemoteUpdate;
					bool allowRemoteQuery;
					bool allowRemoteRemove;
					char queryPassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];
					char updatePassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];		
					char removePassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];
					bool oneRowPerSystemAddress;
					bool onlyUpdateOwnRows;
					bool removeRowOnPingFailure;
					bool removeRowOnDisconnect;
					bool autogenerateRowIDs;

					printf("Enter name of table to create: ");
					gets(tableName);
					if (tableName[0]==0)
						strcpy(tableName, "Default Table");

					printf("Allow remote row updates? (y (Default) / n)\n");
					if (getche()=='n')
					{
						printf("\n");
						allowRemoteUpdate=false;
					}
					else
					{
						printf("\n");
						allowRemoteUpdate=true;
						printf("Enter remote update password (Enter for none): ");
						gets(updatePassword);

						printf("Only allow one row per uploading IP? (y (Default) / n)\n");
						oneRowPerSystemAddress = (getche()=='n')==false;

						printf("Only allow updates on rows created by that system? (y (Default) / n)\n");
						onlyUpdateOwnRows = (getche()=='n')==false;								

						printf("Remove row if can't ping system? (y (Default) / n)\n");
						removeRowOnPingFailure = (getche()=='n')==false;

						printf("Remove row on system disconnect? (y / n (Default))\n");
						removeRowOnDisconnect = (getche()=='y')==true;
					}

					printf("Allow remote table queries? (y (Default) / n)\n");
					if (getche()=='n')
					{
						printf("\n");
						allowRemoteQuery=false;
					}
					else
					{
						printf("\n");
						allowRemoteQuery=true;
						printf("Enter remote table query password (Enter for none): ");
						gets(queryPassword);
					}

					printf("Allow remote row removal? (y (Default) / n)\n");
					if (getche()=='n')
					{
						printf("\n");
						allowRemoteRemove=false;
					}
					else
					{
						printf("\n");
						allowRemoteRemove=true;
						printf("Enter remote row removal password (Enter for none): ");
						gets(removePassword);
					}

					printf("Autogenerate row ids? (y (Default) / n)\n");
					autogenerateRowIDs = (getche()=='n')==false;

					DataStructures::Table *table;
					table=databaseServer.AddTable(tableName, allowRemoteUpdate, allowRemoteQuery, allowRemoteRemove, queryPassword, updatePassword, removePassword, oneRowPerSystemAddress, onlyUpdateOwnRows, removeRowOnPingFailure, removeRowOnDisconnect, autogenerateRowIDs);
					if (table)
					{
						printf("Table %s created.\n", tableName);
						while (1)
						{
							printf("Enter name of new column\n");
							printf("Hit enter when done\n");
							gets(columnName);
							if (columnName[0]==0)
								break;
							DataStructures::Table::ColumnType columnType;
							printf("Enter column type\n1=STRING\n2=NUMERIC\n3=BINARY\n");
							gets(str);
							if (str[0]=='1')
								columnType=DataStructures::Table::STRING;
							else if (str[0]=='2')
								columnType=DataStructures::Table::NUMERIC;
							else if (str[0]=='3')
								columnType=DataStructures::Table::BINARY;
							else
							{
								printf("Defaulting to string\n");
								columnType=DataStructures::Table::STRING;
							}
							table->AddColumn(columnName, columnType);
							printf("%s added.\n", columnName);
						}
						printf("Done.\n");
					}
					else
						printf("Table %s creation failed.  Possibly already exists.\n", tableName);
				
				}
				else if (ch=='r')
				{
					printf("Enter name of table to remove: ");
					gets(str);
					if (str[0]==0)
						strcpy(str, "Default Table");
					if (databaseServer.RemoveTable(str))
						printf("Success\n");
					else
						printf("Table %s not found\n", str);
				}
			}
			else
			{
				if (ch=='q' || ch=='u' || ch=='r')
				{
					printf("Enter table name: ");
					gets(tableName);
					if (tableName[0]==0)
						strcpy(tableName, "Default Table");
					printf("Enter password (if any): ");
					gets(tablePassword);
				}

				if (ch=='q')
				{
					// TODO - let the user enter filters, columns, and rows to return.
					databaseClient.QueryTable(tableName, tablePassword, 0, 0, 0, 0, 0, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				else if (ch=='u')
				{
					RowUpdateMode updateMode;
					unsigned rowId;
					bool hasRowId;
					printf("Enter row update mode\n1=update existing\n2=update or add\n3=add new\n");
					gets(str);
					if (str[0]=='1')
					{
						updateMode=RUM_UPDATE_EXISTING_ROW;
						printf("Updating existing row (You must enter a row ID).\n");
					}
					else if (str[0]=='2')
					{
						updateMode=RUM_UPDATE_OR_ADD_ROW;
						printf("Updating existing row and adding\nnew row if existing row does not exist.\n");
					}
					else
					{
						updateMode=RUM_ADD_NEW_ROW;
						printf("Adding new row.\n");
					}

					printf("Enter row ID (enter for none): ");
					gets(str);
					if (str[0])
					{
						rowId=atoi(str);
						hasRowId=true;
					}
					else
					{
						hasRowId=false;
						rowId=0;
					}
					
					printf("Enter cells\n");
					unsigned char numCellUpdates;
					DatabaseCellUpdate cellUpdates[64];
					for (numCellUpdates=0; numCellUpdates<64; numCellUpdates++)
					{
						
						printf("Enter column name (Enter when done): ");
						gets(cellUpdates[numCellUpdates].columnName);
						if (cellUpdates[numCellUpdates].columnName[0]==0)
							break;
						printf("Enter column type\n1=STRING\n2=NUMERIC\n3=BINARY\n");
						gets(str);
						if (str[0]=='1' || str[0]==0)
						{
							cellUpdates[numCellUpdates].columnType=DataStructures::Table::STRING;
							printf("Enter string value: ");
							gets(str);
							cellUpdates[numCellUpdates].cellValue.Set(str);
						}
						else if (str[0]=='2')
						{
							cellUpdates[numCellUpdates].columnType=DataStructures::Table::NUMERIC;
							printf("Enter numeric value: ");
							gets(str);
							cellUpdates[numCellUpdates].cellValue.Set(atoi(str));
						}
						else
						{
							cellUpdates[numCellUpdates].columnType=DataStructures::Table::BINARY;
							// TODO - Pain in the ass to write this demo code
							printf("TODO\n");
						}
					}

					databaseClient.UpdateRow(tableName, tablePassword, updateMode, hasRowId, rowId, cellUpdates, numCellUpdates, UNASSIGNED_SYSTEM_ADDRESS, true );
				}
				else if (ch=='r')
				{
					unsigned rowId;
					printf("Enter row ID to remove: ");
					gets(str);
					rowId=atoi(str);
					databaseClient.RemoveRow(tableName, tablePassword, rowId, UNASSIGNED_SYSTEM_ADDRESS, true);
				}
			}

			if (ch=='e')
				break;

			ch=0;
		}

		RakSleep(30);
	}

	rakPeer->Shutdown(100,0);
	RakNetworkFactory::DestroyRakPeerInterface(rakPeer);
}
