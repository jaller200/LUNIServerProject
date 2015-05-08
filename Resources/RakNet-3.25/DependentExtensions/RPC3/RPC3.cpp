#include "RPC3.h"
#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include "StringCompressor.h"
#include "BitStream.h"
#include "Types.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include <stdlib.h>

using namespace RakNet;

int RPC3::RemoteRPCFunctionComp( const RPC3::RPCIdentifier &key, const RemoteRPCFunction &data )
{
	return strcmp(key.C_String(), data.identifier.C_String());
}

RPC3::RPC3()
{
	currentExecution[0]=0;
	rakPeer=0;
	networkIdManager=0;
	outgoingTimestamp=0;
	outgoingPriority=HIGH_PRIORITY;
	outgoingReliability=RELIABLE_ORDERED;
	outgoingOrderingChannel=0;
	outgoingBroadcast=true;
	incomingTimeStamp=0;
	DataStructures::Map<SystemAddress, DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *>::IMPLEMENT_DEFAULT_COMPARISON();
}

RPC3::~RPC3()
{
	Clear();
}

void RPC3::SetNetworkIDManager(NetworkIDManager *idMan)
{
	networkIdManager=idMan;
}

bool RPC3::UnregisterFunction(const char *uniqueIdentifier)
{
	return false;
}

bool RPC3::IsFunctionRegistered(const char *uniqueIdentifier)
{
	unsigned i = GetLocalFunctionIndex(uniqueIdentifier);
	return i!=-1;
}

void RPC3::SetTimestamp(RakNetTime timeStamp)
{
	outgoingTimestamp=timeStamp;
}

void RPC3::SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	outgoingPriority=priority;
	outgoingReliability=reliability;
	outgoingOrderingChannel=orderingChannel;
}

void RPC3::SetRecipientAddress(SystemAddress systemAddress, bool broadcast)
{
	outgoingSystemAddress=systemAddress;
	outgoingBroadcast=broadcast;
}

void RPC3::SetRecipientObject(NetworkID networkID)
{
	outgoingNetworkID=networkID;
}

RakNetTime RPC3::GetLastSenderTimestamp(void) const
{
	return incomingTimeStamp;
}

SystemAddress RPC3::GetLastSenderAddress(void) const
{
	return incomingSystemAddress;
}

RakPeerInterface *RPC3::GetRakPeer(void) const
{
	return rakPeer;
}

const char *RPC3::GetCurrentExecution(void) const
{
	return (const char *) currentExecution;
}

bool RPC3::SendCall(RakString uniqueIdentifier, char parameterCount, RakNet::BitStream *serializedParameters)
{	
	SystemAddress systemAddr;
	unsigned int outerIndex;
	unsigned int innerIndex;

	if (uniqueIdentifier.IsEmpty())
		return false;

	RakNet::BitStream bs;
	if (outgoingTimestamp!=0)
	{
		bs.Write((MessageID)ID_TIMESTAMP);
		bs.Write(outgoingTimestamp);
	}
	bs.Write((MessageID)ID_AUTO_RPC_CALL);
	bs.Write(parameterCount);
	if (outgoingNetworkID!=UNASSIGNED_NETWORK_ID)
	{
		bs.Write(true);
		bs.Write(outgoingNetworkID);
	}
	else
	{
		bs.Write(false);
	}
	// This is so the call SetWriteOffset works
	bs.AlignWriteToByteBoundary();
	BitSize_t writeOffset = bs.GetWriteOffset();
	if (outgoingBroadcast)
	{
		unsigned systemIndex;
		for (systemIndex=0; systemIndex < rakPeer->GetMaximumNumberOfPeers(); systemIndex++)
		{
			systemAddr=rakPeer->GetSystemAddressFromIndex(systemIndex);
			if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS && systemAddr!=outgoingSystemAddress)
			{
				if (GetRemoteFunctionIndex(systemAddr, uniqueIdentifier, &outerIndex, &innerIndex))
				{
					// Write a number to identify the function if possible, for faster lookup and less bandwidth
					bs.Write(true);
					bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
				}
				else
				{
					bs.Write(false);
					stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
				}

				bs.WriteCompressed(serializedParameters->GetNumberOfBitsUsed());
				bs.WriteAlignedBytes((const unsigned char*) serializedParameters->GetData(), serializedParameters->GetNumberOfBytesUsed());
				rakPeer->Send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);

				// Start writing again after ID_AUTO_RPC_CALL
				bs.SetWriteOffset(writeOffset);
			}
		}
	}
	else
	{
		systemAddr = outgoingSystemAddress;
		if (systemAddr!=UNASSIGNED_SYSTEM_ADDRESS)
		{
			if (GetRemoteFunctionIndex(systemAddr, uniqueIdentifier, &outerIndex, &innerIndex))
			{
				// Write a number to identify the function if possible, for faster lookup and less bandwidth
				bs.Write(true);
				bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
			}
			else
			{
				bs.Write(false);
				stringCompressor->EncodeString(uniqueIdentifier, 512, &bs, 0);
			}

			bs.WriteCompressed(serializedParameters->GetNumberOfBitsUsed());
			bs.WriteAlignedBytes((const unsigned char*) serializedParameters->GetData(), serializedParameters->GetNumberOfBytesUsed());
			rakPeer->Send(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);
		}
		else
			return false;
	}
	return true;
}

void RPC3::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
	outgoingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
	outgoingNetworkID=UNASSIGNED_NETWORK_ID;
	incomingSystemAddress=UNASSIGNED_SYSTEM_ADDRESS;
}

PluginReceiveResult RPC3::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	RakNetTime timestamp=0;
	unsigned char packetIdentifier, packetDataOffset;
	if ( ( unsigned char ) packet->data[ 0 ] == ID_TIMESTAMP )
	{
		if ( packet->length > sizeof( unsigned char ) + sizeof( RakNetTime ) )
		{
			packetIdentifier = ( unsigned char ) packet->data[ sizeof( unsigned char ) + sizeof( RakNetTime ) ];
			// Required for proper endian swapping
			RakNet::BitStream tsBs(packet->data+sizeof(MessageID),packet->length-1,false);
			tsBs.Read(timestamp);
			packetDataOffset=sizeof( unsigned char )*2 + sizeof( RakNetTime );
		}
		else
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	else
	{
		packetIdentifier = ( unsigned char ) packet->data[ 0 ];
		packetDataOffset=sizeof( unsigned char );
	}

	switch (packetIdentifier)
	{
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		OnCloseConnection(peer, packet->systemAddress);
		return RR_CONTINUE_PROCESSING;
	case ID_AUTO_RPC_CALL:
		incomingTimeStamp=timestamp;
		incomingSystemAddress=packet->systemAddress;
		OnRPC3Call(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTO_RPC_REMOTE_INDEX:
		OnRPCRemoteIndex(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;		
	}

	return RR_CONTINUE_PROCESSING;
}

void RPC3::OnRPC3Call(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	RakNet::BitStream bs(data,lengthInBytes,false);

	bool hasParameterCount=false;
	char parameterCount;
	NetworkIDObject *networkIdObject;
	NetworkID networkId;
	bool hasNetworkId=false;
	bool hasFunctionIndex=false;
	unsigned int functionIndex;
	BitSize_t bitsOnStack;
	char strIdentifier[512];
	incomingExtraData.Reset();
	bs.Read(parameterCount);
	bs.Read(hasNetworkId);
	if (hasNetworkId)
	{
		bs.Read(networkId);
		if (networkIdManager==0 && (networkIdManager=rakPeer->GetNetworkIDManager())==0)
		{
			// Failed - Tried to call object member, however, networkIDManager system was never registered
			SendError(systemAddress, RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE, "");
			return;
		}
		networkIdObject = (NetworkIDObject*) networkIdManager->GET_OBJECT_FROM_ID(networkId);
		if (networkIdObject==0)
		{
			// Failed - Tried to call object member, object does not exist (deleted?)
			SendError(systemAddress, RPC_ERROR_OBJECT_DOES_NOT_EXIST, "");
			return;
		}
	}
	else
	{
		networkIdObject=0;
	}
	bs.AlignReadToByteBoundary();
	bs.Read(hasFunctionIndex);
	if (hasFunctionIndex)
		bs.ReadCompressed(functionIndex);
	else
		stringCompressor->DecodeString(strIdentifier,512,&bs,0);
	bs.ReadCompressed(bitsOnStack);
	RakNet::BitStream serializedParameters;
	serializedParameters.AddBitsAndReallocate(BITS_TO_BYTES(bitsOnStack));
	bs.ReadAlignedBytes(serializedParameters.GetData(), BITS_TO_BYTES(bitsOnStack));
	serializedParameters.SetWriteOffset(bitsOnStack);

	if (hasFunctionIndex)
	{
		if (functionIndex>localFunctions.Size())
		{
			// Failed - other system specified a totally invalid index
			// Possible causes: Bugs, attempts to crash the system, requested function not registered
			SendError(systemAddress, RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE, "");
			return;
		}
	}
	else
	{
		// Find the registered function with this str
		for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
		{
			bool isObjectMember = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);
	//		boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);

			if (isObjectMember == (networkIdObject!=0) &&
				strcmp(localFunctions[functionIndex].identifier.C_String(), strIdentifier)==0)
			{
				// SEND RPC MAPPING
				RakNet::BitStream outgoingBitstream;
				outgoingBitstream.Write((MessageID)ID_AUTO_RPC_REMOTE_INDEX);
				outgoingBitstream.Write(hasNetworkId);
				outgoingBitstream.WriteCompressed(functionIndex);
				stringCompressor->EncodeString(strIdentifier,512,&outgoingBitstream,0);
				rakPeer->Send(&outgoingBitstream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
				break;
			}
		}

		if (functionIndex==localFunctions.Size())
		{
			for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
			{
				if (strcmp(localFunctions[functionIndex].identifier.C_String(), strIdentifier)==0)
				{
					bool isObjectMember = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);
					if (isObjectMember==true && networkIdObject==0)
					{
						// Failed - Calling C++ function as C function
						SendError(systemAddress, RPC_ERROR_CALLING_CPP_AS_C, strIdentifier);
						return;
					}

					if (isObjectMember==false && networkIdObject!=0)
					{
						// Failed - Calling C function as C++ function
						SendError(systemAddress, RPC_ERROR_CALLING_C_AS_CPP, strIdentifier);
						return;
					}
				}
			}

			SendError(systemAddress, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
			return;
		}
	}

	bool isObjectMember = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);
	boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<1>(localFunctions[functionIndex].functionPointer);
//	int arity = boost::fusion::get<2>(localFunctions[functionIndex].functionPointer);
//	if (isObjectMember)
//		arity--; // this pointer
	if (functionPtr==0)
	{
		// Failed - Function was previously registered, but isn't registered any longer
		SendError(systemAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, localFunctions[functionIndex].identifier);
		return;
	}

// Boost doesn't support this for class members
//	if (arity!=parameterCount)
//	{
//		// Failed - The number of parameters that this function has was explicitly specified, and does not match up.
//		SendError(systemAddress, RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS, localFunctions[functionIndex].identifier);
//		return;
//	}

	_RPC3::InvokeArgs functionArgs;
	functionArgs.bitStream=&serializedParameters;
	functionArgs.networkIDManager=networkIdManager;
	functionArgs.caller=this;
	functionArgs.thisPtr=networkIdObject;
//	serializedParameters.PrintBits();
	_RPC3::InvokeResultCodes res2 = functionPtr(functionArgs);
}

void RPC3::OnRPCRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	// A remote system has given us their internal index for a particular function.
	// Store it and use it from now on, to save bandwidth and search time
	bool objectExists;
	RakString strIdentifier;
	unsigned int insertionIndex;
	unsigned int remoteIndex;
	RemoteRPCFunction newRemoteFunction;
	RakNet::BitStream bs(data,lengthInBytes,false);
	RPCIdentifier identifier;
	bool isObjectMember;
	bs.Read(isObjectMember);
	bs.ReadCompressed(remoteIndex);
	bs.Read(strIdentifier);

	if (strIdentifier.IsEmpty())
		return;

	DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList;
	if (remoteFunctions.Has(systemAddress))
	{
		theList = remoteFunctions.Get(systemAddress);
		insertionIndex=theList->GetIndexFromKey(identifier, &objectExists);
		if (objectExists==false)
		{
			newRemoteFunction.functionIndex=remoteIndex;
			newRemoteFunction.identifier = strIdentifier;
			theList->InsertAtIndex(newRemoteFunction, insertionIndex);
		}
	}
	else
	{
		theList = new DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp>;

		newRemoteFunction.functionIndex=remoteIndex;
		newRemoteFunction.identifier = strIdentifier;
		theList->InsertAtEnd(newRemoteFunction);

		remoteFunctions.SetNew(systemAddress,theList);
	}
}

void RPC3::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	(void) peer;
	if (remoteFunctions.Has(systemAddress))
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions.Get(systemAddress);
		delete theList;
		remoteFunctions.Delete(systemAddress);
	}
}

void RPC3::OnShutdown(RakPeerInterface *peer)
{
	(void) peer;
	Clear();
}

void RPC3::Clear(void)
{
	unsigned j;
	for (j=0; j < remoteFunctions.Size(); j++)
	{
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions[j];
		delete theList;
	}
	localFunctions.Clear();
	remoteFunctions.Clear();
	outgoingExtraData.Reset();
	incomingExtraData.Reset();
}

void RPC3::SendError(SystemAddress target, unsigned char errorCode, const char *functionName)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_RPC_REMOTE_ERROR);
	bs.Write(errorCode);
	bs.WriteAlignedBytes((const unsigned char*) functionName,(const unsigned int) strlen(functionName)+1);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
}

unsigned RPC3::GetLocalFunctionIndex(RPC3::RPCIdentifier identifier)
{
	unsigned i;
	for (i=0; i < localFunctions.Size(); i++)
	{
		if (localFunctions[i].identifier==identifier)
			return i;
	}
	return (unsigned) -1;
}

bool RPC3::GetRemoteFunctionIndex(SystemAddress systemAddress, RPC3::RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex)
{
	bool objectExists=false;
	if (remoteFunctions.Has(systemAddress))
	{
		*outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions[*outerIndex];
		*innerIndex = theList->GetIndexFromKey(identifier, &objectExists);
	}
	return objectExists;
}

