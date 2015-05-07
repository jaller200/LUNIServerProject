#ifndef __TRANFORMATION_HISTORY_H
#define __TRANFORMATION_HISTORY_H

#include "RakNetTypes.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "DS_Queue.h"
#include "RakMemoryOverride.h"

struct TransformationHistoryCell : public RakNet::RakMemoryOverride
{
	TransformationHistoryCell();
	TransformationHistoryCell(RakNetTime t, const Ogre::Vector3& pos, const Ogre::Vector3& vel, const Ogre::Quaternion& quat  );

	RakNetTime time;
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Ogre::Vector3 velocity;
};

class TransformationHistory : public RakNet::RakMemoryOverride
{
public:
	void Init(RakNetTime maxWriteInterval, RakNetTime maxHistoryTime);
	void Write(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNetTime curTimeMS);
	void Overwrite(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNetTime when);
	// Parameters are in/out, modified to reflect the history
	void Read(Ogre::Vector3 *position, Ogre::Vector3 *velocity, Ogre::Quaternion *orientation,
		RakNetTime when, RakNetTime curTime);
	void Clear(void);
protected:
	DataStructures::Queue<TransformationHistoryCell> history;
	unsigned maxHistoryLength;
	RakNetTime writeInterval;
};

#endif
