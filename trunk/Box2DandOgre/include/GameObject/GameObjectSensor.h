/*=============================================================================

		GameObjectSensor.h

		Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_SENSOR_H
#define GAME_OBJECT_SENSOR_H

#include "HelperFunctions.h"

#include "GameObjectOgreBox2D.h"
#include "MessageDispatcher.h"


enum SensorType
{
	SENSORTYPE_DEFAULT,
	SENSORTYPE_LEDGE,
	SENSORTYPE_COUNT
};

/// An array allowing easy conversion of KGBMessageType to a c string.
const static char* SensorTypeString[SENSORTYPE_COUNT] = 
{
	"sensortype default",
	"sensortype ledge",
};

//=============================================================================
//						StringToSensorType
//
/// This returns an Ogre::String as a SensorType
static SensorType StringToSensorType(Ogre::String str)
{
	Ogre::StringUtil::toLowerCase(str);

	for(int i = 0; i < SENSORTYPE_COUNT; i++) 
	{
		if(str.compare(SensorTypeString[i]) == 0)
		{
			return (SensorType) i;
		}
	}

	return SENSORTYPE_COUNT;
}

/// This class sends out messages when its Box2D position is hit.
class GameObjectSensor : public GameObjectOgreBox2D
{
public:

	friend class GameObjectSensorCreator;

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
					 Ogre::String name = "GameObjectSensor",
					 b2Body* body = 0, Ogre::Entity* entity = 0):
	GameObjectOgreBox2D(name, body, entity)
	{
		defaultMessageOn_.messageType  = messageOn;
		defaultMessageOff_.messageType = messageOff;
		sensorType_ = SENSORTYPE_DEFAULT;
		gameObjectType_ = GOType_Sensor;
		messageObjectOnContact_ = false;
		isEnabled_ = true;
	}

	virtual ~GameObjectSensor()
	{

	}

	bool Update(double timeSinceLastFrame)
	{ 
		GameObjectOgreBox2D::Update(timeSinceLastFrame);

		return true;
	}

	bool HandleMessage(const KGBMessage message)
	{
		GameObject::HandleMessage(message);

		// Debug stuff.
		if(message.messageType == defaultMessageOff_.messageType)
		{
			if(message.sender == objectId_)
			{
				body_->ApplyImpulse(b2Vec2(-10,50), body_->GetWorldCenter());
			}
		}

		return false;
	}

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
		if(isEnabled_)
		{
			if(ignoreSensors_ && collidedFixture->IsSensor())
			{
				return;
			}

			if(messageObjectOnContact_)
			{
				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();

				if(go)
				{
					boost::any pUserData = 0;
					Dispatch->DispatchMessage(SEND_IMMEDIATELY,objectId_, go->GetId(), defaultMessageOn_.messageType, defaultMessageOn_.messageData);
				}
			}

			SendOnMessageToList();
		}
	}

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture)
	{
		if(isEnabled_)
		{
			if(ignoreSensors_ && collidedFixture->IsSensor())
			{
				return; 
			}

			if(messageObjectOnContact_)
			{
				GameObject* go = (GameObject*) collidedFixture->GetBody()->GetUserData();
				if(go)
				{
					boost::any pUserData = 0;
					Dispatch->DispatchMessage(SEND_IMMEDIATELY,objectId_, go->GetId(), defaultMessageOff_.messageType, defaultMessageOff_.messageData);
				}
			}

			SendOffMessageToList();
		}
	}

	/// Send out a message to all the Objects in messageList_.
	virtual void SendOnMessageToList(boost::any pUserData = 0)
	{
		SensorMap::iterator it;

		for(it = messageList_.begin(); it != messageList_.end(); it++)
		{
			if((*it).second.on.messageType != NO_MESSAGE)
			{
				Dispatch->DispatchMessage((*it).second.on.messageType, objectId_, (*it).first, (*it).second.on.messageType, (*it).second.on.messageData);
			}
		}
	}

	/// Send out a message to all the Objects in messageList_.
	virtual void SendOffMessageToList(boost::any pUserData = 0)
	{
		SensorMap::iterator it;

		for(it = messageList_.begin(); it != messageList_.end(); it++)
		{
			if((*it).second.off.messageType != NO_MESSAGE)
			{
				Dispatch->DispatchMessage(SEND_IMMEDIATELY, objectId_, (*it).first, (*it).second.off.messageType, (*it).second.off.messageData);
			}
		}
	}

	/// Send out a message to all the Objects in messageList_.
	virtual void SendToMessageList(KGBMessageType message, boost::any pUserData = 0)
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

		sm.off.dispatchTime = SEND_IMMEDIATELY;
		sm.on.dispatchTime = SEND_IMMEDIATELY;
		// If the message is MESSAGE_NULL the default message is used.
		if(messageOn != MESSAGE_NULL)
		{
			sm.on.messageType = messageOn;
		}
		else
		{
			sm.on.messageType = defaultMessageOn_.messageType;
		}

		if(messageOff != MESSAGE_NULL)
		{
			sm.off.messageType = messageOff;
		}
		else
		{
			sm.off.messageType = defaultMessageOff_.messageType;
		}

		messageList_.insert(std::make_pair<GameObjectId, SensorMessage>(Id, sm));

	}

	/// Add an Object to the message list
	void AddToMessageList(GameObjectId Id, KGBMessage messageOn, KGBMessage messageOff) 
	{ 
		SensorMessage sm;

		sm.off = KGBMessage(messageOff);
		sm.on = KGBMessage(messageOn);

		// If the message is MESSAGE_NULL the default message is used.
		if(messageOn.messageType == MESSAGE_NULL)
		{
			sm.on.messageType = defaultMessageOn_.messageType;
		}

		if(messageOff.messageType == MESSAGE_NULL)
		{
			sm.off.messageType = defaultMessageOff_.messageType;
		}

		messageList_.insert(std::make_pair<GameObjectId, SensorMessage>(Id, sm));

	}

	/// Remove an Object from the message list
	void RemoveFromMessageList(GameObjectId Id)
	{ 
		messageList_.erase(messageList_.find(Id));
	}


	bool Initialize()
	{
		return GameObjectOgreBox2D::Initialize();
	}

	void SetDefaultOnMessage(KGBMessageType onMessage){ defaultMessageOn_.messageType = onMessage; }
	void SetDefaultOffMessage(KGBMessageType offMessage){ defaultMessageOff_.messageType = offMessage; }

	void IgnoreSensors(){ignoreSensors_ = true;} 
	void IgnoreSensorsOff(){ignoreSensors_ = false;}
	bool IsIgnoringSensors(){return ignoreSensors_;}

	void SetEnabled(bool e){ isEnabled_ = e; }
	bool IsEnabled(){ return isEnabled_;}
	SensorType GetSensorType(){return sensorType_;}
	

protected:

	struct SensorMessage
	{
		KGBMessage on;
		KGBMessage off;
		SensorMessage(){}
		SensorMessage(KGBMessageType pOn, KGBMessageType pOff)
		{
			on.messageType = pOn;
			off.messageType = pOff;
		}
	};

	

	typedef std::map<GameObjectId, SensorMessage> SensorMap;
	
	SensorType sensorType_;

	KGBMessage defaultMessageOn_;
	KGBMessage defaultMessageOff_;

	/// Map of GameObjectId's and a message to send to that object
	/// when the sensor is activated.
	SensorMap messageList_;

	bool isEnabled_;

	/// Does this object Message the object that is touching it.
	bool messageObjectOnContact_;

	/// Does this object ignore fixtures that are sensors.
	bool ignoreSensors_;

};



#endif