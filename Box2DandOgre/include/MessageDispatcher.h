/*=============================================================================

	MessageDispatcher.h

	Author: Matt King

	Dispatches messages to Entitys.

=============================================================================*/

#ifndef KGBMESSAGE_DISPATCHER_H
#define KGBMESSAGE_DISPATCHER_H

#pragma warning (disable:4786)

#include <set>
#include "OgreSingleton.h"
#include "Message.h"

#define Dispatch KGBMessageDispatcher::getSingletonPtr()
typedef unsigned int GameObjectId;

/// Send the message immediately with no delay.
const double SEND_IMMEDIATELY = 0.0;

/// Send the message to all Objects.
const unsigned int SEND_TO_ALL = 0;

/// Forward declare GameObject.
class GameObject;

/// This class sends KGBMessages to GameObjects. All GameObjects can handle
/// messages.
///
/// Messages can be sent immediately or after a specified amount of time has
/// passed. Messages can also be sent to all GameObjects or to a specific
/// GameObject by using the GameObject's GameObjectId.
class KGBMessageDispatcher : public Ogre::Singleton<KGBMessageDispatcher>
{
public:


	/// Logging of messages is by default true.
	KGBMessageDispatcher();

	/// Send a message to either all or one other Object 
	void DispatchMessage(double			pDelay,
						 GameObjectId	pSender,
						 GameObjectId	pReceiver,
						 KGBMessageType	pMessageType,
						 boost::any	pUserData);

	/// Call this to send out delayed messages
	void DispatchDelayedMessages();

	/// Empty the delayed message queue
	void ClearDelayedMessages(){ messageQueue_.clear(); }

	/// Turn message logging on.
	void MessageLoggingOn()  { logMessages_ = true; }

	/// Turn message logging off.
	void MessageLoggingOff() { logMessages_ = false; }

	/// This returns a the objects name from its GameObjectId.
	Ogre::String GetObjectName(GameObjectId id);

private:  

	/// This method discharges the message to all Entitys
	void DischargeToAll(const KGBMessage pMsg);

	/// A std::set is used as the container for the delayed messages
	/// because of the benefit of automatic sorting and avoidance
	/// of duplicates. Messages are sorted by their dispatch time.
	std::set<KGBMessage> messageQueue_;

	/// This method is used by DispatchMessage or DispatchDelayedMessages to
	/// send messages to either all or one Entity.
	void Discharge(const KGBMessage& pMsg);

	/// This method is used by DispatchMessage or DispatchDelayedMessages to
	/// send messages to either all or one Entity. This function logs the
	/// message.
	void DischargeWithLog(const KGBMessage& pMsg);

	///copy ctor and assignment should be private for a singleton.
	KGBMessageDispatcher(const KGBMessageDispatcher&);
	KGBMessageDispatcher& operator=(const KGBMessageDispatcher&);

	/// Should messages be logged?
	bool logMessages_;

};

#endif