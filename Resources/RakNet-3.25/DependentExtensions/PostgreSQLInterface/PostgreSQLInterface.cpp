#include "PostgreSQLInterface.h"

// libpq-fe.h is part of PostgreSQL which must be installed on this computer to use the PostgreRepository
#include "libpq-fe.h"

#ifdef _CONSOLE_1
#include "Console1Includes.h"
#elif defined(_WIN32)
#include <winsock2.h> // htonl
#elif defined(_CONSOLE_2)
#include "Console2Includes.h"
#else
#include <arpa/inet.h>
#endif

// alloca
#ifdef _COMPATIBILITY_1
#elif defined(_WIN32)
#include <malloc.h>
#else
//#include <stdlib.h>
#endif

#include "RakAssert.h"
#include "BitStream.h"
#include "FormatString.h"
#include "LinuxStrings.h"

#define PQEXECPARAM_FORMAT_TEXT		0
#define PQEXECPARAM_FORMAT_BINARY	1

PostgreSQLInterface::PostgreSQLInterface()
{
	pgConn=0;
	isConnected=false;
	lastError[0]=0;
	pgConnAllocatedHere=false;
}
PostgreSQLInterface::~PostgreSQLInterface()
{
	if (isConnected && pgConnAllocatedHere)
		PQfinish(pgConn);
}
bool PostgreSQLInterface::Connect(const char *conninfo)
{
	if (isConnected==false)
	{
		pgConn=PQconnectdb(conninfo);
		ConnStatusType status = PQstatus(pgConn);
		isConnected=status==CONNECTION_OK;
		if (isConnected==false)
		{
			PQfinish(pgConn);
			return false;
		}
		pgConnAllocatedHere=true;
	}

	return isConnected;
}
void PostgreSQLInterface::AssignConnection(PGconn *_pgConn)
{
	pgConnAllocatedHere=false;
	pgConn=_pgConn;
	ConnStatusType status = PQstatus(pgConn);
	isConnected=status==CONNECTION_OK;
}
PGconn *PostgreSQLInterface::GetPGConn(void) const
{
	return pgConn;
}
void PostgreSQLInterface::Disconnect(void)
{
	if (isConnected)
	{
		PQfinish(pgConn);
		isConnected=false;
	}
}
void PostgreSQLInterface::Rollback(void)
{
	PGresult *result = PQexec(pgConn, "ROLLBACK;");
	PQclear(result);
}
bool PostgreSQLInterface::IsResultSuccessful(PGresult *result, bool rollbackOnFailure)
{
	if (result==0)
		return false;

	bool success=false;
	ExecStatusType execStatus = PQresultStatus(result);
	strcpy(lastError,PQresultErrorMessage(result));
	switch (execStatus)
	{
	case PGRES_COMMAND_OK:
		success=true;
		break;
	case PGRES_EMPTY_QUERY:
		break;
	case PGRES_TUPLES_OK:
		success=true;
		break;
	case PGRES_COPY_OUT:
		break;
	case PGRES_COPY_IN:
		break;
	case PGRES_BAD_RESPONSE:
		break;
	case PGRES_NONFATAL_ERROR:
		break;
	case PGRES_FATAL_ERROR:
		if (rollbackOnFailure)
			Rollback();
		break;
	}
	return success;
}
bool PostgreSQLInterface::ExecuteBlockingCommand(const char *command, PGresult **result, bool rollbackOnFailure)
{
	*result = PQexec(pgConn, command);
	return IsResultSuccessful(*result, rollbackOnFailure);
}
bool PostgreSQLInterface::PQGetValueFromBinary(int *output, PGresult *result, unsigned int rowIndex, const char *columnName)
{
	return PQGetValueFromBinary(output,result,(int) rowIndex,columnName);
}
bool PostgreSQLInterface::PQGetValueFromBinary(int *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(int));
		memcpy(output, binaryData, sizeof(int));
		EndianSwapInPlace((char*)output, sizeof(int));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(unsigned int *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;	
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(unsigned int));
		memcpy(output, binaryData, sizeof(unsigned int));
		EndianSwapInPlace((char*)output, sizeof(unsigned int));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(long long *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	if (binaryData)
	{
		RakAssert(PQgetlength(result, rowIndex, columnIndex)==sizeof(long long));
		memcpy(output, binaryData, sizeof(long long));
		EndianSwapInPlace((char*)output, sizeof(long long));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(float *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	int len = PQgetlength(result, rowIndex, columnIndex);
	RakAssert(len==sizeof(float));
	RakAssert(binaryData);
	if (binaryData)
	{
		memcpy(output, binaryData, sizeof(float));
		EndianSwapInPlace((char*)output, sizeof(float));
		return true;
	}
	return false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(bool *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *binaryData = PQgetvalue(result, rowIndex, columnIndex);
	if (binaryData==0)
		return false;
	*output = binaryData[0]!=0;
	
	// Seems to return 1 byte only
	/*
	int tempResult=0;
//	int len = PQgetlength(result, rowIndex, columnIndex);
	// RakAssert(len==sizeof(int)); // bug here, returns 0, but is an int
	RakAssert(binaryData);
	if (binaryData)
	{
		memcpy(&tempResult, binaryData, sizeof(int));
		EndianSwapInPlace((char*)&tempResult, sizeof(int));
		*output = tempResult!=0;
		return true;
	}
	*/
	
	return true;
}
bool PostgreSQLInterface::PQGetValueFromBinary(RakNet::RakString *output, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName); if (columnIndex==-1) return false;
	char *gv;
	gv = PQgetvalue(result, rowIndex, columnIndex);
	if (gv && gv[0])
		*output=gv;
	else
		output->Clear();
	return output->IsEmpty()==false;
}
bool PostgreSQLInterface::PQGetValueFromBinary(char **output, unsigned int *outputLength, PGresult *result, int rowIndex, const char *columnName)
{
	return PQGetValueFromBinary(output, (int*) outputLength,result, rowIndex,columnName);
}
bool PostgreSQLInterface::PQGetValueFromBinary(char **output, int *outputLength, PGresult *result, int rowIndex, const char *columnName)
{
	int columnIndex = PQfnumber(result, columnName);
	if (columnIndex!=-1)
	{
		*outputLength=PQgetlength(result, rowIndex, columnIndex);
		if (*outputLength==0)
		{
			*output=0;
			return false;
		}
		else
		{
			*output = new char[*outputLength];
			memcpy(*output, PQgetvalue(result, rowIndex, columnIndex), *outputLength);
			return true;
		}
	}
	else
		return false;
	
}
void PostgreSQLInterface::EndianSwapInPlace(char* data, int dataLength)
{
	static bool alreadyNetworkOrder=(htonl(12345) == 12345);
	if (alreadyNetworkOrder)
		return;
	int i;
	char tmp;
	for (i=0; i < dataLength/2; i++)
	{
		tmp=data[i];
		data[i]=data[dataLength-1-i];
		data[dataLength-1-i]=tmp;
	}
}

char *PostgreSQLInterface::GetLocalTimestamp(void)
{
	static char strRes[512];

	PGresult *result;
	if (ExecuteBlockingCommand("SELECT LOCALTIMESTAMP", &result, false))
	{
		char *ts=PQgetvalue(result, 0, 0);
		if (ts)
		{
			sprintf(strRes,"Local timestamp is: %s\n", ts);
		}
		else
		{
			sprintf(strRes,"Can't read current time\n");
		}
		PQclear(result);
	}
	else
		sprintf(strRes,"Failed to read LOCALTIMESTAMP\n");

	return (char*)strRes;

}

const char* PostgreSQLInterface::GetLastError(void) const
{
	return (char*)lastError;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, unsigned int value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, unsigned int value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, int value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, bool value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	if (value)
		valueStr += "true";
	else
		valueStr += "false";
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, int value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%i", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, float value, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr += FormatString("%f", value);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, float value, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr += FormatString("%f", value);
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, char *binaryData, int binaryDataLength, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty)
{
	if (writeEmpty==false && (binaryData==0 || binaryDataLength==0))
		return;

	if (binaryData==0)
		binaryDataLength=0;

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr+=FormatString("$%i::bytea", numParams+1);

	paramData[numParams]=binaryData;
	paramLength[numParams]=binaryDataLength;
	paramFormat[numParams]=PQEXECPARAM_FORMAT_BINARY;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, char *binaryData, int binaryDataLength, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat)
{
	if (binaryData==0 || binaryDataLength==0)
		return;

	if (binaryData==0)
		binaryDataLength=0;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr += " = ";
	valueStr+=FormatString("$%i::bytea", numParams+1);

	paramData[numParams]=binaryData;
	paramLength[numParams]=binaryDataLength;
	paramFormat[numParams]=PQEXECPARAM_FORMAT_BINARY;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, const char *str, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty, const char *type)
{
	if (writeEmpty==false && (str==0 || str[0]==0))
		return;

	static char *emptyStr="";
	static char *emptyDate="01/01/01";

	if (paramTypeStr.IsEmpty()==false)
	{
		paramTypeStr += ", ";
		valueStr += ", ";
	}
	paramTypeStr += colName;
	valueStr+=FormatString("$%i::%s", numParams+1, type);

	if (writeEmpty && (str==0 || str[0]==0))
	{
		if (strcmp(type,"date")==0)
		{
			paramData[numParams]=emptyDate;
			paramLength[numParams]=(int)strlen(emptyDate);
		}
		else
		{
			paramData[numParams]=emptyStr;
			paramLength[numParams]=0;
		}		
	}
	else
	{
		paramData[numParams]=(char*) str;
		paramLength[numParams]=(int) strlen(str);
	}	
	paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, const char *str, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, const char *type)
{
	if (str==0 || str[0]==0)
		return;

	if (valueStr.IsEmpty()==false)
	{
		valueStr += ", ";
	}
	valueStr += colName;
	valueStr+=" = ";
	valueStr+=FormatString("$%i::%s", numParams+1, type);

	paramData[numParams]=(char*) str;
	paramLength[numParams]=(int) strlen(str);

	paramFormat[numParams]=PQEXECPARAM_FORMAT_TEXT;
	numParams++;
}
void PostgreSQLInterface::EncodeQueryInput(const char *colName, const RakNet::RakString &str, RakNet::RakString &paramTypeStr, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, bool writeEmpty, const char *type)
{
	EncodeQueryInput(colName, str.C_String(), paramTypeStr, valueStr, numParams, paramData, paramLength, paramFormat, writeEmpty, type);
}
void PostgreSQLInterface::EncodeQueryUpdate(const char *colName, const RakNet::RakString &str, RakNet::RakString &valueStr, int &numParams, char **paramData, int *paramLength, int *paramFormat, const char *type)
{
	EncodeQueryUpdate(colName, str.C_String(), valueStr, numParams, paramData, paramLength, paramFormat, type);
}
RakNet::RakString PostgreSQLInterface::GetEscapedString(const char *input) const
{
	unsigned long len = (unsigned long) strlen(input);
	char *fn = new char [len*2+1];
	int error;
	PQescapeStringConn(pgConn, fn, input, len, &error);
	RakNet::RakString output;
	// Use assignment so it doesn't parse printf escape strings
	output = fn;
	delete [] fn;
	return output;
}
