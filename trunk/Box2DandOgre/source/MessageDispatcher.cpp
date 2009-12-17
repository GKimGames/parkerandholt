/*=============================================================================

	MessageDispatcher.cpp

	Author: Matt King

	Dispatches messages to Entitys.

=============================================================================*/

#include "MessageDispatcher.h"
#include <iostream>

using std::cout;
using std::set;

template<> KGBMessageDispatcher* Ogre::Singleton<KGBMessageDispatcher>::ms_Singleton = 0;
//=============================================================================
//	Discharge
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
		PaH::Entity* entity = PaH::Entity::GetEntity(pMsg.receiver);
		
		if(entity)
		{
			entity->HandleMessage(pMsg);
		}
		else
		{
			//KGBMessage could not be handled
			cout << "Message not handled";
		}
	}
}


//=============================================================================
//	DischargeToAll
//
/// Dispatch a message to all Entitys
void KGBMessageDispatcher::DischargeToAll(const KGBMessage& pMsg)
{
	
	PaH::EntityMap::iterator it;

	PaH::EntityMap entityList = PaH::Entity::entityList;

	for (it = entityList.begin(); it != entityList.end(); it++)
	{
		(*it).second->HandleMessage(pMsg);	
	}
}

//=============================================================================
//	Dispatch Message
//
/// Given a message, a receiver, a sender and any time delay , this function
/// routes the message to the correct agent (if no delay) or stores
/// in the message queue to be dispatched at the correct time
void KGBMessageDispatcher:: DispatchMessage(double			pDelay,
											int				pSender,
											int				pReceiver,
											int				pMessageType,
											boost::any*		pUserData)
{


	// Create the KGBMessage
	KGBMessage message(0, pSender, pReceiver, pMessageType, pUserData);

	// If there is no delay, route KGBMessage immediately                       
	if (pDelay <= SEND_IMMEDIATELY)                                                        
	{

		// Discharge the message
		Discharge(message);

		/*
		cout << "\nInstant KGBMessage dispatched at time: " << GameFramework::getSingletonPtr()->timer_->getMillisecondsCPU()
		<< " by " << GetNameOfEntity(pSender->ID()) << " for " << GetNameOfEntity(pReceiver->ID()) 
		<< ". Msg is "<< MsgToStr(msg);
		*/
	}
	// Else calculate the time when the KGBMessage should be dispatched
	else
	{
		double CurrentTime = GameFramework::getSingletonPtr()->timer_->getMillisecondsCPU();

		message.dispatchTime = CurrentTime + pDelay;

		// Insert the message into the queue
		messageQueue_.insert(message);   

		/*
		cout	<< "\nDelayed KGBMessage from " << GetNameOfEntity(pSender->ID()) << " recorded at time " 
		<< Clock->GetCurrentTime() << " for " << GetNameOfEntity(pReceiver->ID())
		<< ". Msg is "<< MsgToStr(msg);
		*/

	}
}


//=============================================================================
//	DispatchDelayedMessages
//
/// This function dispatches any KGBMessages with a timestamp that has
/// expired. Any dispatched KGBMessages are removed from the queue
void KGBMessageDispatcher::DispatchDelayedMessages()
{
  
	// Get current time
	double CurrentTime = GameFramework::getSingletonPtr()->timer_->getMillisecondsCPU();

	//now peek at the queue to see if any KGBMessages need dispatching.
	//remove all KGBMessages from the front of the queue that have gone
	//past their sell by date
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



