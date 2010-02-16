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
	MESSAGE_ZERO,
	MESSAGE_NULL,
	NO_MESSAGE,
	CHARACTER_MOVE_LEFT_PLUS,
	CHARACTER_MOVE_LEFT_MINUS,
	CHARACTER_MOVE_RIGHT_PLUS,
	CHARACTER_MOVE_RIGHT_MINUS,
	CHARACTER_JUMP_PLUS,
	CHARACTER_JUMP_MINUS,
	CHARACTER_ENTER_PLATFORMSTATE,
	CHARACTER_ENTER_GRAVITYSTATE,
	CHARACTER_EXIT_PLACINGSTATE,
	GAME_SENSOR_ON,
	GAME_SENSOR_OFF,
	STUPID_MESSAGE,
	CREATE_BOX,
	UPDATE_MOUSE,
	LEFT_MOUSE_PLUS,
	LEFT_MOUSE_MINUS,
	MIDDLE_MOUSE_PLUS,
	MIDDLE_MOUSE_MINUS,
	UPDATE_CHECKPOINT,
	RETURN_TO_CHECKPOINT,
	ADD_ITEM,
	MESSAGE_TYPE_COUNT
};  

/// An array allowing easy conversion of KGBMessageType to a c string.
const static char* KGBMessageTypeString[MESSAGE_TYPE_COUNT] = 
{
	"message zero",
	"message null",
	"no message",
	"character move left plus",
	"character move left minus",
	"character move right plus",
	"character move right minus",
	"character jump plus",
	"character jump minus",
	"character enter platformstate",
	"character enter gravitystate",
	"character exit placingstate",
	"game sensor on",
	"game sensor off",
	"stupid message",
	"create box",
	"update mouse",
	"left mouse plus",
	"left mouse minus",
	"middle mouse plus",
	"middle mouse minus"
	"update checkpoint",
	"return to checkpoint",
	"add item",
	"MESSAGE_TYPE_COUNT"
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
//						MessageToString
//
/// This returns a KGBMessageType as an Ogre::String
static Ogre::String MessageToString(KGBMessageType message)
{
	if(message > MESSAGE_ZERO && message < MESSAGE_TYPE_COUNT)
	{
		return KGBMessageTypeString[message];
	}

	return KGBMessageTypeString[MESSAGE_NULL];
}



//=============================================================================
//						StringToMessage
//
/// This returns an Ogre::String as a KGBMessageType
static KGBMessageType StringToMessage(Ogre::String str)
{
	Ogre::StringUtil::toLowerCase(str);
	
	for(int i = 0; i < MESSAGE_TYPE_COUNT; i++) {
		if(str.compare(KGBMessageTypeString[i]) == 0)
		{
			return (KGBMessageType) i;
		}
	}

	return NO_MESSAGE;

}


#endif