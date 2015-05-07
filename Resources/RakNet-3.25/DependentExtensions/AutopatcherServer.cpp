/// \file
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
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

#include "AutopatcherServer.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringCompressor.h"
#include "RakPeerInterface.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "AutopatcherRepositoryInterface.h"
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

AutopatcherServer::AutopatcherServer()
{
	rakPeer=0;
	fileListTransfer=0;
	priority=HIGH_PRIORITY;
	orderingChannel=0;
	repository=0;
}
AutopatcherServer::~AutopatcherServer()
{
	Clear();
}
void AutopatcherServer::SetUploadSendParameters(PacketPriority _priority, char _orderingChannel)
{
	priority=_priority;
	orderingChannel=_orderingChannel;
}
void AutopatcherServer::SetFileListTransferPlugin(FileListTransfer *flt)
{
	fileListTransfer=flt;
}
void AutopatcherServer::SetAutopatcherRepositoryInterface(AutopatcherRepositoryInterface *ari)
{
	RakAssert(ari);
	repository=ari;
}
void AutopatcherServer::OnAttach(RakPeerInterface *peer)
{
	rakPeer=peer;
	threadPool.StartThreads(1, 0);
}
void AutopatcherServer::OnDetach(RakPeerInterface *peer)
{
	Clear();
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::Update(RakPeerInterface *peer)
{
	ResultTypeAndBitstream* rtab;
	while (threadPool.HasOutputFast() && threadPool.HasOutput())
	{
		rtab = threadPool.GetOutput();
		if (rtab->bitStream1.GetNumberOfBitsUsed()>0)
			rakPeer->Send(&(rtab->bitStream1), priority, RELIABLE_ORDERED, orderingChannel, rtab->systemAddress, false);
		if (rtab->bitStream2.GetNumberOfBitsUsed()>0)
			rakPeer->Send(&(rtab->bitStream2), priority, RELIABLE_ORDERED, orderingChannel, rtab->systemAddress, false);
		delete rtab;
	}	
}
PluginReceiveResult AutopatcherServer::OnReceive(RakPeerInterface *peer, Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE:
		OnGetChangelistSinceDate(peer, packet);
		return RR_STOP_PROCESSING;
	case ID_AUTOPATCHER_GET_PATCH:
		OnGetPatch(peer, packet);
		return RR_STOP_PROCESSING;
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
		RemoveFromThreadPool(packet->systemAddress);
	break;
	}

	return RR_CONTINUE_PROCESSING;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnShutdown(RakPeerInterface *peer)
{
	Clear();
}
void AutopatcherServer::Clear(void)
{
	// Clear the waiting input and output from the thread pool.
	unsigned i;
	threadPool.StopThreads();
	for (i=0; i < threadPool.InputSize(); i++)
		rakPeer->DeallocatePacket(threadPool.GetInputAtIndex(i).packet);
	threadPool.ClearInput();
	for (i=0; i < threadPool.OutputSize(); i++)
		delete threadPool.GetOutputAtIndex(i);
	threadPool.ClearOutput();
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnStartup(RakPeerInterface *peer)
{
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress)
{
	RemoveFromThreadPool(systemAddress);
}
void AutopatcherServer::RemoveFromThreadPool(SystemAddress systemAddress)
{
	unsigned i;
	i=0;
	threadPool.LockInput();
	while (i < threadPool.InputSize())
	{
		if (threadPool.GetInputAtIndex(i).packet->systemAddress==systemAddress)
		{
			rakPeer->DeallocatePacket(threadPool.GetInputAtIndex(i).packet);
			threadPool.RemoveInputAtIndex(i);
		}
		else
			i++;
	}
	threadPool.UnlockInput();

	i=0;
	threadPool.LockOutput();
	while (i < threadPool.OutputSize())
	{
		if (threadPool.GetOutputAtIndex(i)->systemAddress==systemAddress)
		{
			delete threadPool.GetOutputAtIndex(i);
			threadPool.RemoveOutputAtIndex(i);
		}
		else
			i++;
	}
	threadPool.UnlockOutput();
}
AutopatcherServer::ResultTypeAndBitstream* GetChangelistSinceDateCB(AutopatcherServer::PatcherAndPacket pap, bool *returnOutput, void* perThreadData)
{
	Packet *packet = pap.packet;
	AutopatcherServer *server = pap.server;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	char applicationName[512];
	char lastUpdateDate[64];
	stringCompressor->DecodeString(applicationName, 512, &inBitStream);
	if (stringCompressor->DecodeString(lastUpdateDate, 64, &inBitStream)==false)
	{
		server->rakPeer->DeallocatePacket(packet);
		*returnOutput=false;
		return 0; // Invalid bitstream
	}

	FileList addedFiles, deletedFiles;
	char currentDate[64];

	AutopatcherServer::ResultTypeAndBitstream *rtab = new AutopatcherServer::ResultTypeAndBitstream;
	rtab->systemAddress=pap.packet->systemAddress;

	// Query the database for a changelist since this date
	RakAssert(server);
	if (server->repository->GetChangelistSinceDate(applicationName, &addedFiles, &deletedFiles, lastUpdateDate, currentDate))
	{
		if (deletedFiles.fileList.Size())
		{
			rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_DELETION_LIST);
			deletedFiles.Serialize(&rtab->bitStream1);
		}

		if (addedFiles.fileList.Size())
		{
			rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_CREATION_LIST);
			addedFiles.Serialize(&rtab->bitStream2);
			stringCompressor->EncodeString(currentDate,64,&rtab->bitStream2);
			addedFiles.Clear();
		}
		else
		{
			rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_FINISHED);
			stringCompressor->EncodeString(currentDate,64,&rtab->bitStream2);
		}
	}
	else
	{
		rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
		stringCompressor->EncodeString(server->repository->GetLastError(), 256, &rtab->bitStream2);
	}

	*returnOutput=true;
	server->rakPeer->DeallocatePacket(packet);
	return rtab;
}
void AutopatcherServer::OnGetChangelistSinceDate(RakPeerInterface *peer, Packet *packet)
{
	PatcherAndPacket pap;
	pap.packet=packet;
	pap.server=this;
	threadPool.AddInput(GetChangelistSinceDateCB, pap);
}
AutopatcherServer::ResultTypeAndBitstream* GetPatchCB(AutopatcherServer::PatcherAndPacket pap, bool *returnOutput, void* perThreadData)
{
	Packet *packet = pap.packet;
	AutopatcherServer *server = pap.server;

	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	FileList clientList, patchList;
	unsigned short setId;
	char applicationName[512];

	inBitStream.IgnoreBits(8);
	inBitStream.Read(setId);
	stringCompressor->DecodeString(applicationName, 512, &inBitStream);
	if (clientList.Deserialize(&inBitStream)==false)
	{
		*returnOutput=false;
		server->rakPeer->DeallocatePacket(packet);
		return 0;
	}
	if (clientList.fileList.Size()==0)
	{
		RakAssert(0);
		*returnOutput=false;
		server->rakPeer->DeallocatePacket(packet);
		return 0;
	}

	AutopatcherServer::ResultTypeAndBitstream *rtab = new AutopatcherServer::ResultTypeAndBitstream;
	rtab->systemAddress=pap.packet->systemAddress;
	RakAssert(server);
	RakAssert(server->repository);
	char currentDate[64];
	if (server->repository->GetPatches(applicationName, &clientList, &patchList, currentDate))
	{
		if (patchList.fileList.Size())
			server->fileListTransfer->Send(&patchList, server->rakPeer, packet->systemAddress, setId, server->priority, server->orderingChannel, false);

		rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_FINISHED_INTERNAL);
		stringCompressor->EncodeString(currentDate,64,&rtab->bitStream1);

	}
	else
	{
		rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
		stringCompressor->EncodeString(server->repository->GetLastError(), 256, &rtab->bitStream1);
	}

	*returnOutput=true;
	server->rakPeer->DeallocatePacket(packet);
	return rtab;
}
void AutopatcherServer::OnGetPatch(RakPeerInterface *peer, Packet *packet)
{
	PatcherAndPacket pap;
	pap.packet=packet;
	pap.server=this;
	threadPool.AddInput(GetPatchCB, pap);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
