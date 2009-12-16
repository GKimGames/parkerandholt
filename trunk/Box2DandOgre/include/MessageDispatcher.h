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
#include "Message.h"
#include "Entity.h"

#define Dispatch KGBMessageDispatcher::getSingletonPtr()

/// Send the message immediately with no delay.
const double SEND_IMMEDIATELY = 0.0;

/// Send the message to all Entitys.
const unsigned int SEND_TO_ALL = 0;


class KGBMessageDispatcher : public Ogre::Singleton<KGBMessageDispatcher>
{
public:

	KGBMessageDispatcher(){}

	// Send a message to either all or one other Entity 
	void DispatchMessage(double			pDelay,
						 int			pSender,
						 int			pReceiver,
						 int			pMessageType,
						 boost::any*	pUserData);

	// Call this to send out delayed messages
	void DispatchDelayedMessages();

private:  

	// This method discharges the message to all Entitys
	void DischargeToAll(const KGBMessage& pMsg);

	//a std::set is used as the container for the delayed messages
	//because of the benefit of automatic sorting and avoidance
	//of duplicates. Messages are sorted by their dispatch time.
	std::set<KGBMessage> messageQueue_;

	// This method is utilized by DispatchMessage or DispatchDelayedMessages to
	// send messages to either all or one Entity.
	void Discharge(const KGBMessage& pMsg);

	

	//copy ctor and assignment should be private
	KGBMessageDispatcher(const KGBMessageDispatcher&);
	KGBMessageDispatcher& operator=(const KGBMessageDispatcher&);
};



#endif