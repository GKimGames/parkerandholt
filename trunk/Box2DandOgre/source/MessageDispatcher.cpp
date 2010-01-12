/*=============================================================================

	MessageDispatcher.cpp

	Author: Matt King

	Dispatches messages to GameObjects.

=============================================================================*/

#include "MessageDispatcher.h"
#include "GameFramework.h"
#include "GameObject.h"
#include "HelperFunctions.h"
#include <iostream>

using std::cout;
using std::set;

template<> KGBMessageDispatcher* Ogre::Singleton<KGBMessageDispatcher>::ms_Singleton = 0;


//=============================================================================
//						Constructor
//
/// Logging of messages is by default false.
KGBMessageDispatcher::KGBMessageDispatcher()
{
	logMessages_ = true;
}


//=============================================================================
//						Discharge
///
///	Discharges a message to the Entitys.
void KGBMessageDispatcher::Discharge(const KGBMessage& pMsg)
{

	if(pMsg.receiver == SEND_TO_ALL)
	{
		DischargeToAll(pMsg);
	}
	else
	{

		GameObject* object = GameObject::GetObjectById(pMsg.receiver);
		
		if(object)
		{
			object->HandleMessage(pMsg);
		}

	}
}

//=============================================================================
//						Discharge
///
///	Discharges a message to the Entitys.
void KGBMessageDispatcher::DischargeWithLog(const KGBMessage& pMsg)
{

	bool messageOk = false;


	Ogre::String s = "Message: ";
	s += MessageToString(pMsg.messageType);
	s += " to ";
	if(pMsg.receiver == SEND_TO_ALL)
	{
		s += "ALL_OBJECTS";
		s += " from ";
		s += GetObjectName(pMsg.sender);

		DischargeToAll(pMsg);

	}
	else
	{
		// Get the receiver and sender.
		s += GetObjectName(pMsg.receiver);
		s += " from ";
		s += GetObjectName(pMsg.sender);

		GameObject* object = GameObject::GetObjectById(pMsg.receiver);
		
		if(object)
		{
			messageOk = object->HandleMessage(pMsg);
		}

		// Did the message get handled?
		if(messageOk)
		{
			s += Ogre::String(": Message Handled");
		}
		else
		{
			// KGBMessage could not be handled
			s += Ogre::String(": Message failed");
		}
	}

	DEBUG_LOG(s);
}


//=============================================================================
//						DischargeToAll
//
/// Dispatch a message to all objects
void KGBMessageDispatcher::DischargeToAll(const KGBMessage pMsg)
{
	
	GameObjectMap::iterator it;

	GameObjectMap objectList = GameObject::objectList;

	for (it = objectList.begin(); it != objectList.end(); it++)
	{
		(*it).second->HandleMessage(pMsg);	
	}
}

//=============================================================================
//						Dispatch Message
//
/// Given a message, a receiver, a sender and any time delay , this function
/// routes the message to the correct agent (if no delay) or stores
/// in the message queue to be dispatched at the correct time
void KGBMessageDispatcher::DispatchMessage(double			pDelay,
											GameObjectId	pSender,
											GameObjectId	pReceiver,
											KGBMessageType	pMessageType,
											boost::any		pUserData)
{

	// Create the KGBMessage
	KGBMessage message(0, pSender, pReceiver, pMessageType, pUserData);

	// If there is no delay, route KGBMessage immediately                       
	if (pDelay <= SEND_IMMEDIATELY)                                                        
	{

		if(logMessages_ == false)
		{
			// Discharge the message
			Discharge(message);
		}
		else
		{
			// Discharge the message with logging
			DischargeWithLog(message);
		}
		
	}
	// Else calculate the time when the KGBMessage should be dispatched
	else
	{
		double CurrentTime = GameFramework::getSingletonPtr()->timer_->getMillisecondsCPU();

		message.dispatchTime = CurrentTime + pDelay;

		// Insert the message into the queue
		messageQueue_.insert(message);  

		if(logMessages_ == true)
		{
			Ogre::String s = "Delayed Message - To ";
			// Get the receiver and sender.
			s += GetObjectName(message.receiver);
			s += " from ";
			s += GetObjectName(message.sender);
			s += ". Message: ";
			s += MessageToString(message.messageType);
			DEBUG_LOG(s);
		}
	}
}



//=============================================================================
//						DispatchDelayedMessages
//
/// This function dispatches any KGBMessages with a timestamp that has
/// expired. Any dispatched KGBMessages are removed from the queue
void KGBMessageDispatcher::DispatchDelayedMessages()
{
  
	// Get current time
	double CurrentTime = GameFramework::getSingletonPtr()->timer_->getMillisecondsCPU();

	// Now peek at the queue to see if any KGBMessages need dispatching.
	// Remove all KGBMessages from the front of the queue that have gone
	// past their sell by date
	while( !messageQueue_.empty() &&
		(  messageQueue_.begin()->dispatchTime < CurrentTime) && 
		(  messageQueue_.begin()->dispatchTime > 0) )
	{
		// Read the KGBMessage from the front of the queue
		const KGBMessage& message = (*messageQueue_.begin());

		Discharge(message);

		// Remove it from the queue
		messageQueue_.erase(messageQueue_.begin());
	}
}

//=============================================================================
//						GetObjectName
//
/// This returns a the objects name from its GameObjectId.
Ogre::String KGBMessageDispatcher::GetObjectName(GameObjectId id)
{
	if(id == SEND_TO_ALL)
	{
		// All objects are the receiver
		return Ogre::String("ALL_OBJECTS");
	}
	GameObject* go = GameObject::GetObjectById(id);
	if(go)
	{
		return go->GetName();
	}

	// We didn't find the object.
	return Ogre::String("UNABLE_TO_FIND_OBJECT");

}






