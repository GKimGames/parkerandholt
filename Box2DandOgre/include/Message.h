/*=============================================================================

KGBMessage.h

Author: Matt King

This is a message that allows objects to send commands to each other.
All objects have the method HandleMessage

=============================================================================*/

#ifndef KGBMESSAGE_H
#define KGBMESSAGE_H

#include <boost/any.hpp>
#include <iostream>
#include <math.h>
#include "Ogre.h"

typedef unsigned int GameObjectId;

/// An Enum for Messages.
enum KGBMessageType
{
	MESSAGE_NULL,
	NO_MESSAGE,
	CHARACTER_MOVE_LEFT,
	CHARACTER_MOVE_RIGHT,
	CHARACTER_JUMP,
	GAME_SENSOR_ON,
	GAME_SENSOR_OFF,
	STUPID_MESSAGE
}; 

struct KGBMessage
{
	/// The object that sent this Message
	unsigned int	sender;

	/// The object that is to receive this Message
	unsigned int	receiver;

	/// The message itself
	KGBMessageType messageType;

	/// Messages can be dispatched immediately or delayed for a specified amount
	/// of time. If a delay is necessary this field is stamped with the time 
	/// the message should be dispatched.
	double		dispatchTime;

	/// Any extra message data.
	boost::any	messageData;


	KGBMessage():dispatchTime(-1),
		sender(-1),
		receiver(-1),
		messageData(NULL)
	{}


	KGBMessage(double pTime,
		unsigned int	pSender,
		unsigned int	pReceiver,
		KGBMessageType	pMessageType,
		boost::any  pInfo = NULL):
		
		dispatchTime(pTime),
		sender(pSender),
		receiver(pReceiver),
		messageType(pMessageType),
		messageData(pInfo)
	{}

};


// These Messages will be stored in a priority queue. Therefore the <
// operator needs to be overloaded so that the queue can sort the Messages
// by time priority. Note how the times must be smaller than
// SmallestDelay apart before two Messages are considered unique.
const double SmallestDelay = 0.25;


inline bool operator == (const KGBMessage& t1, const KGBMessage& t2)
{
	return  (fabs(t1.dispatchTime-t2.dispatchTime) < SmallestDelay) &&
		(t1.sender == t2.sender)        &&
		(t1.receiver == t2.receiver)    &&
		(t1.messageType == t2.messageType);
}

inline bool operator < (const KGBMessage& t1, const KGBMessage& t2)
{
	if (t1 == t2)
	{
		return false;
	}

	else
	{
		return  (t1.dispatchTime < t2.dispatchTime);
	}
}



//=============================================================================
//						StringToMessage
//
/// This returns an Ogre::Stringas a KGBMessageType
static KGBMessageType StringToMessage(const Ogre::String str)
{
	if(str.compare("CHARACTER_MOVE_LEFT") == 0)
	{
		return CHARACTER_MOVE_LEFT;
	}
	else if(str.compare("CHARACTER_MOVE_RIGHT") == 0)
	{
		return CHARACTER_MOVE_RIGHT;
	}
	else if(str.compare("CHARACTER_JUMP") == 0)
	{
		return CHARACTER_JUMP;
	}

	return NO_MESSAGE;

}


//=============================================================================
//						MessageToString
//
/// This returns a KGBMessageType as an Ogre::String
static Ogre::String MessageToString(KGBMessageType message)
{

	switch(message)
	{
	case CHARACTER_MOVE_LEFT:
		{
			return Ogre::String("CHARACTER_MOVE_LEFT");
		}
	case CHARACTER_MOVE_RIGHT:
		{
			return Ogre::String("CHARACTER_MOVE_RIGHT");
		}
	case CHARACTER_JUMP:
		{
			return Ogre::String("CHARACTER_JUMP");
		}
	case GAME_SENSOR_ON:
		{
			return Ogre::String("GAME_SENSOR_ON");
		}
	case GAME_SENSOR_OFF:
		{
			return Ogre::String("GAME_SENSOR_OFF");
		}
	}

	return Ogre::String("Unrecognized message type");
}

#endif