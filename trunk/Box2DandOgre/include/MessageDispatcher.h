/*=============================================================================

	MessageDispatcher.cpp

	Author: Matt King

	Dispatches messages to Entitys.

=============================================================================*/

#ifndef KGBMESSAGE_DISPATCHER_H
#define KGBMESSAGE_DISPATCHER_H

#pragma warning (disable:4786)

#include <set>

#include "OgreSingleton.h"
#include "GameObject.h"
#include "Message.h"
#include "HelperFunctions.h"

#define Dispatch KGBMessageDispatcher::getSingletonPtr()
typedef unsigned int GameObjectId;

/// Send the message immediately with no delay.
const double SEND_IMMEDIATELY = 0.0;

/// Send the message to all Objects.
const unsigned int SEND_TO_ALL = 0;


class KGBMessageDispatcher : public Ogre::Singleton<KGBMessageDispatcher>
{
public:



	KGBMessageDispatcher();

	// Send a message to either all or one other Object 
	void DispatchMessage(double			pDelay,
						 GameObjectId	pSender,
						 GameObjectId	pReceiver,
						 KGBMessageType	pMessageType,
						 boost::any*	pUserData);

	// Call this to send out delayed messages
	void DispatchDelayedMessages();

	void ClearDelayedMessages(){ messageQueue_.clear(); }

	void MessageLoggingOn()  { logMessages_ = true; }
	void MessageLoggingOff() { logMessages_ = false; }

	Ogre::String GetObjectName(GameObjectId id);

private:  

	// This method discharges the message to all Entitys
	void DischargeToAll(const KGBMessage& pMsg);

	// A std::set is used as the container for the delayed messages
	// because of the benefit of automatic sorting and avoidance
	// of duplicates. Messages are sorted by their dispatch time.
	std::set<KGBMessage> messageQueue_;

	// This method is utilized by DispatchMessage or DispatchDelayedMessages to
	// send messages to either all or one Entity.
	void Discharge(const KGBMessage& pMsg);

	// This method is utilized by DispatchMessage or DispatchDelayedMessages to
	// send messages to either all or one Entity. This function logs the
	// message.
	void DischargeWithLog(const KGBMessage& pMsg);

	//copy ctor and assignment should be private
	KGBMessageDispatcher(const KGBMessageDispatcher&);
	KGBMessageDispatcher& operator=(const KGBMessageDispatcher&);

	bool logMessages_;

};

#endif