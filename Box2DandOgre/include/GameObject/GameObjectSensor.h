/*=============================================================================

  GameObjectSensor.h

  Author: Matt King

=============================================================================*/

#ifndef ENTITY_SENSOR_H
#define ENTITY_SENSOR_H

#include "EntityBox2DResponder.h"
#include "MessageDispatcher.h"

namespace PaH
{
	class GameObjectSensor : public GameObjectOgreBox2D
	{
	public:


		GameObjectSensor(b2Vec2 position);
		~GameObjectSensor();

		bool Update(double timeSinceLastFrame){ return true;};

		/// Called when two fixtures begin to touch.
		virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
		{
			if(ignoreSensors_ && collidedFixture->IsSensor())
			{
				return;
			}
		}

		/// Called when two fixtures cease to touch.
		virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
		{
			if(ignoreSensors_ && collidedFixture->IsSensor())
			{
				return;
			}

		}

		/// Send out a message to all the Objects in messageList_.
		virtual void SendToMessageList(double pDelay,KGBMessageType messageType, boost::any* pUserData)
		{
			std::vector<unsigned int>::iterator it;

			for(it = messageList_.begin(); it != messageList_.end(); it++)
			{

				//Dispatch->DispatchMessage(pDelay, objectId_, (*it), messageType, pUserData);
			}

		}

		/// Add an Object to the message list
		void AddToMessageList(unsigned int Id) 
		{ 
			messageList_.push_back(Id); 
		}

		/// Remove an Object from the message list
		void RemoveFromMessageList(unsigned int Id)
		{ 
			std::vector<unsigned int>::iterator it;

			for(it = messageList_.begin(); it != messageList_.end(); it++)
			{
				if((*it) == Id)
				{
					messageList_.erase(it);
				}
			}
		}

		void IgnoreSensors(bool b) { ignoreSensors = b; } 

	protected:

		/// This stores a list of Object Id's to send a message to when this Object
		/// comes in contact with something.
		std::vector<unsigned int>	messageList_;

		/// The message this sensor will send out when it is hit.
		KGBMessageType messageType_;

		bool ignoreSensors_;

	};
}


#endif