/// \file
/// \brief Connection between FMOD and RakVoice
///

#ifndef __FMODVOICEBRIDGE_H
#define __FMODVOICEBRIDGE_H

#include "RakVoice.h"

// If you get:
// Error	1	fatal error C1083: Cannot open include file: 'fmod.hpp': No such file or directory	c:\raknet\samples\rakvoicefmod\fmodvoiceadapter.h	9
// It is because this project depends on Fmod.  If you don't have FMOD you can't use it.
#include "fmod.hpp"

/// \brief Connects FMOD with RakVoice.
class FMODVoiceAdapter {

public:

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	/// Returns the singleton
	static FMODVoiceAdapter* Instance();

	/// \brief Setups the connection between FMOD and RakVoice
	/// You must call this method to create the connection between FMOD and RakVoice.
	/// \param[in] fmodSystem FMOD system object to use.
	/// \param[in] rakVoice RakVoice object to use, fully Initialized AND attached to a RakPeerInterface.
	/// \pre IMPORTANT : Don't forget to initialized and attach rakVoice, before calling this method.
	/// \sa \link FMODVoiceAdapter::Update \endlink
	/// \return true on success, false if an error occurred.
	bool SetupAdapter(FMOD::System *fmodSystem, RakVoice *rakVoice);

	/// Release any resources used.
	void Release();

	/// You need to call this once in a while, depending on the parameters used. Ex: call once every 20-30 milliseconds
	void Update();

	/// Turns on/off outgoing traffic
	/// \param[in] true to mute, false to allow outgoing traffic.
	void SetMute(bool mute);

private:

	void UpdateSound(bool isRec);
	void BroadcastFrame(void *ptr);

	static FMODVoiceAdapter instance;

	//  As required by the Singleton Pattern, make those ones private,
	// to keep the user from creating objects of this class.
	FMODVoiceAdapter();
	FMODVoiceAdapter(const FMODVoiceAdapter &obj) {};

	//  FMOD releases all his resources at shutdown, so we don't need to do anything, which 
    // cames in handy, as we don't need to worry about when to destroy the singleton.
	~FMODVoiceAdapter() {};

	RakVoice *rakVoice;
	FMOD::System *fmodSystem;
	FMOD::Sound *recSound; // sound used for recording
	FMOD::Sound *sound; // sound used to play what we hear
	FMOD::Channel *channel;
	bool mute;
	unsigned int lastPlayPos;
	unsigned int lastRecordingPos;
};

#endif
