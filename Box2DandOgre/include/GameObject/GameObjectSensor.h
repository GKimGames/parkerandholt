/*=============================================================================

		GameObjectSensor.h

		Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_SENSOR_H
#define GAME_OBJECT_SENSOR_H

#include "HelperFunctions.h"

#include "GameObjectOgreBox2D.h"
#include "MessageDispatcher.h"


class GameObjectSensor : public GameObjectOgreBox2D
{
public:

	/// Constructor takes a message to send to all GameObjects registered
	/// to this sensor. Shapes for the sensor can be added using the 
	/// AddShape function, this must be done or optionally a body already
	/// initialized can be passed in the constructor. An entity can be
	/// passed in the constructor as well.
	///
	/// The position for this object is by default at 0,0 and should set
	/// using SetBodyPosition.
	GameObjectSensor(KGBMessageType messageOn = GAME_SENSOR_ON,
					 KGBMessageType messageOff = GAME_SENSOR_OFF,
					 b2Body* body = 0, Ogre::Entity* entity = 0):
	GameObjectOgreBox2D(body, entity)
	{
		defaultMessageOn_  = messageOn;
		defaultMessageOff_ = messageOff;
	}

	virtual ~GameObjectSensor(){}

	bool Update(double timeSinceLastFrame){ return true;}

	/// Adds a shape to this sensor. This will initialize a static body if it is
	/// not already done so. 
	void AddShape(b2Shape* shape)
	{
		if(body_ == 0)
		{
			// Create body and fixture
			b2BodyDef bd;
			bd.type = b2_staticBody;
			body_= world_->CreateBody(&bd);
		}

		b2FixtureDef fd;
		fd.shape = shape;
		fd.isSensor = true;

		body_->CreateFixture(&fd); 
	}

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
	virtual void SendOnMessageToList(boost::any* pUserData = 0)
	{
		SensorMap::iterator it;

		for(it = messageList_.begin(); it != messageList_.end(); it++)
		{
			if((*it).second.on != NO_MESSAGE)
			{
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, objectId_, (*it).first, (*it).second.on, pUserData);
			}
		}
	}

	/// Send out a message to all the Objects in messageList_.
	virtual void SendOffMessageToList(boost::any* pUserData = 0)
	{
		SensorMap::iterator it;

		for(it = messageList_.begin(); it != messageList_.end(); it++)
		{
			if((*it).second.on != NO_MESSAGE)
			{
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, objectId_, (*it).first, (*it).second.off, pUserData);
			}
		}
	}

	/// Send out a message to all the Objects in messageList_.
	virtual void SendToMessageList(KGBMessageType message, boost::any* pUserData = 0)
	{
		SensorMap::iterator it;

		if(message != NO_MESSAGE)
		{
			for(it = messageList_.begin(); it != messageList_.end(); it++)
			{
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, objectId_, (*it).first, message, pUserData);
			}
		}
	}

	/// Add an Object to the message list
	void AddToMessageList(GameObjectId Id, KGBMessageType messageOn = MESSAGE_NULL, KGBMessageType messageOff = MESSAGE_NULL) 
	{ 
		SensorMessage sm;
		// If the message is MESSAGE_NULL the default message is used.
		if(messageOn != MESSAGE_NULL)
		{
			sm.on = messageOn;
		}
		else
		{
			sm.on = defaultMessageOn_;
		}

		if(messageOff != MESSAGE_NULL)
		{
			sm.off = messageOff;
		}
		else
		{
			sm.off = defaultMessageOff_;
		}

		messageList_.insert(std::make_pair<GameObjectId, SensorMessage>(Id, sm));

	}

	/// Remove an Object from the message list
	void RemoveFromMessageList(GameObjectId Id)
	{ 
		messageList_.erase(messageList_.find(Id));
	}

	void IgnoreSensors(){ignoreSensors_ = true;} 
	void IgnoreSensorsOff(){ignoreSensors_ = false;}
	bool IsIgnoringSensors(){return ignoreSensors_;}

protected:

	struct SensorMessage
	{
		KGBMessageType on;
		KGBMessageType off;
		SensorMessage(){}
		SensorMessage(KGBMessageType pOn, KGBMessageType pOff):on(pOn), off(pOff){}
	};

	typedef std::map<GameObjectId, SensorMessage> SensorMap;
	

	KGBMessageType defaultMessageOn_;
	KGBMessageType defaultMessageOff_;

	/// Map of GameObjectId's and a message to send to that object
	/// when the sensor is activated.
	SensorMap messageList_;

	bool ignoreSensors_;

};



#endif