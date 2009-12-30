/*=============================================================================

GameObjectSensor.h

Author: Matt King

=============================================================================*/

#ifndef GAME_OBJECT_SENSOR_H
#define GAME_OBJECT_SENSOR_H

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
	GameObjectSensor(KGBMessageType message,b2Body* body = 0, Ogre::Entity* entity = 0):
	GameObjectOgreBox2D(body, entity)
	{
		message_ = message;
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
	  virtual void SendToMessageList(double pDelay,KGBMessageType messageType, boost::any* pUserData)
	  {
		  std::vector<unsigned int>::iterator it;

		  for(it = messageList_.begin(); it != messageList_.end(); it++)
		  {
			  Dispatch->DispatchMessage(pDelay, objectId_, (*it), messageType, pUserData);
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

	  void IgnoreSensors(bool b) { ignoreSensors_ = b; } 

protected:

	/// This stores a list of Object Id's to send a message to when this Object
	/// comes in contact with something.
	std::vector<unsigned int>	messageList_;

	/// The message this sensor will send out when it is hit.
	KGBMessageType message_;

	bool ignoreSensors_;

};



#endif