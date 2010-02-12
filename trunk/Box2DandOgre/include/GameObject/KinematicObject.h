/*=============================================================================

		  KinematicObject.h

		  Author: Matt King

=============================================================================*/
#ifndef KINEMATICOBJECT_H
#define KINEMATICOBJECT_H

#include "GameObjectOgreBox2D.h"

typedef std::vector<b2Vec2> Path;

class KinematicObject : public GameObjectOgreBox2D
{
public:

	KinematicObject();

	bool Update(double timeSinceLastFrame);

	bool Initialize();

	/// Called when two fixtures begin to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);


	/// Called when two fixtures cease to touch.
	/// Contact fixture is the fixture in this Object's body.
	/// Collided fixture is the fixture that hit this Object's body.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	void Disable() { disabled_ = true; }
	void Enable() { disabled_ = false; }
	const bool IsDisabled() { return disabled_; }

	virtual void CreatePhysics();

	/// Send out a message to all the Objects in messageList_.
	virtual void SendOnMessageToList(boost::any pUserData = 0);

	/// Send out a message to all the Objects in messageList_.
	virtual void SendOffMessageToList(boost::any pUserData = 0);

	/// Send out a message to all the Objects in messageList_.
	virtual void SendToMessageList(KGBMessageType message, boost::any pUserData = 0);
	/// Add an Object to the message list
	void AddToMessageList(GameObjectId Id, KGBMessageType messageOn = MESSAGE_NULL, KGBMessageType messageOff = MESSAGE_NULL);

	/// Remove an Object from the message list
	void RemoveFromMessageList(GameObjectId Id);


	void SetDefaultOnMessage(KGBMessageType onMessage){ defaultMessageOn_ = onMessage; }
	void SetDefaultOffMessage(KGBMessageType offMessage){ defaultMessageOff_ = offMessage; }

protected:

	KGBMessageType startReachedMessage_;
	KGBMessageType endReachedMessage_;

	bool isDisabled_;
	Path path_;
};