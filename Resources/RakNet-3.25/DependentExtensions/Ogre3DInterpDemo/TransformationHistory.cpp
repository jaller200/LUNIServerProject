#include "TransformationHistory.h"
#include "RakAssert.h"

TransformationHistoryCell::TransformationHistoryCell()
{

}
TransformationHistoryCell::TransformationHistoryCell(RakNetTime t, const Ogre::Vector3& pos, const Ogre::Vector3& vel, const Ogre::Quaternion& quat  ) :
time(t),
velocity(vel),
position(pos),
orientation(quat)
{
}

void TransformationHistory::Init(RakNetTime maxWriteInterval, RakNetTime maxHistoryTime)
{
	writeInterval=maxWriteInterval;
	maxHistoryLength = maxHistoryTime/maxWriteInterval+1;
	history.ClearAndForceAllocation(maxHistoryLength+1);
	RakAssert(writeInterval>0);
}
void TransformationHistory::Write(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNetTime curTimeMS)
{
	if (history.Size()==0)
	{
		history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation));
	}
	else
	{
		const TransformationHistoryCell &lastCell = history.PeekTail();
		if (curTimeMS-lastCell.time>=writeInterval)
		{
			history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation));
			if (history.Size()>maxHistoryLength)
				history.Pop();
		}
	}	
}
void TransformationHistory::Overwrite(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, RakNetTime when)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		history.Push(TransformationHistoryCell(when,position,velocity,orientation));
	}
	else
	{
		// Find the history matching this time, and change the values.
		int i;
		for (i=historySize-1; i>=0; i--)
		{
			TransformationHistoryCell &cell = history[i];
			if (when >= cell.time)
			{
				if (i==historySize-1 && when-cell.time>=writeInterval)
				{
					// Not an overwrite at all, but a new cell
					history.Push(TransformationHistoryCell(when,position,velocity,orientation));
					if (history.Size()>maxHistoryLength)
						history.Pop();
					return;
				}

				cell.time=when;
				cell.position=position;
				cell.velocity=velocity;
				cell.orientation=orientation;
				return;
			}
		}
	}	
}
void TransformationHistory::Read(Ogre::Vector3 *position, Ogre::Vector3 *velocity, Ogre::Quaternion *orientation,
								 RakNetTime when, RakNetTime curTime)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		return;
	}

	int i;
	for (i=historySize-1; i>=0; i--)
	{
		const TransformationHistoryCell &cell = history[i];
		if (when >= cell.time)
		{
			if (i==historySize-1)
			{
				if (curTime<=cell.time)
					return;

				float divisor = (float)(curTime-cell.time);
				RakAssert(divisor!=0.0f);
				float lerp = (float)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (*position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (*velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, *orientation,true);
			}
			else
			{
				const TransformationHistoryCell &nextCell = history[i+1];
				float divisor = (float)(nextCell.time-cell.time);
				RakAssert(divisor!=0.0f);
				float lerp = (float)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (nextCell.position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (nextCell.velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, nextCell.orientation,true);
			}
			return;
		}
	}

	// Return the oldest one
	const TransformationHistoryCell &cell = history.Peek();
	if (position)
		*position=cell.position;
	if (orientation)
		*orientation=cell.orientation;
	if (velocity)
		*velocity=cell.velocity;
}
void TransformationHistory::Clear(void)
{
	history.Clear();
}