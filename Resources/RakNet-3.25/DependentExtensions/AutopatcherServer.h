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

#ifndef __AUTOPATCHER_SERVER_H
#define __AUTOPATCHER_SERVER_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface.h"
#include "PacketPriority.h"
#include "ThreadPool.h"
#include "BitStream.h"

class RakPeerInterface;
class FileList;
struct Packet;
class FileListTransfer;
class AutopatcherRepositoryInterface;

/// \addtogroup PLUGINS_GROUP
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
class RAK_DLL_EXPORT AutopatcherServer : public PluginInterface
{
public:
	/// Constructor
	AutopatcherServer();

	/// Destructor
	~AutopatcherServer();

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// This class only does the network transfers for the autopatcher.  All the data is stored in a repository.
	/// Pass the interface to your repository with this function.  RakNet comes with AutopatcherPostgreRepository if you wish to use that.
	/// \param[in] ari An implementation of the interface for the autopatcher repository.
	void SetAutopatcherRepositoryInterface(AutopatcherRepositoryInterface *ari);

	/// Clear buffered input and output
	void Clear(void);

	/// \internal For plugin handling
	virtual void OnAttach(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void OnDetach(RakPeerInterface *peer);;
	/// \internal For plugin handling
	virtual void Update(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(RakPeerInterface *peer, Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void OnStartup(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void OnCloseConnection(RakPeerInterface *peer, SystemAddress systemAddress);

	struct PatcherAndPacket
	{
		AutopatcherServer *server;
		Packet *packet;
	};

	struct ResultTypeAndBitstream
	{
		int resultType;
		SystemAddress systemAddress;
		RakNet::BitStream bitStream1;
		RakNet::BitStream bitStream2;
	};

protected:
	friend AutopatcherServer::ResultTypeAndBitstream* GetChangelistSinceDateCB(AutopatcherServer::PatcherAndPacket pap, bool *returnOutput, void* perThreadData);
	friend AutopatcherServer::ResultTypeAndBitstream* GetPatchCB(AutopatcherServer::PatcherAndPacket pap, bool *returnOutput, void* perThreadData);
	void OnGetChangelistSinceDate(RakPeerInterface *peer, Packet *packet);
	void OnGetPatch(RakPeerInterface *peer, Packet *packet);
	void RemoveFromThreadPool(SystemAddress systemAddress);

	AutopatcherRepositoryInterface *repository;
	FileListTransfer *fileListTransfer;
	RakPeerInterface *rakPeer;
	PacketPriority priority;
	char orderingChannel;

	// The point of the threadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
	// The threadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
	ThreadPool<PatcherAndPacket, ResultTypeAndBitstream*> threadPool;
};

#endif
