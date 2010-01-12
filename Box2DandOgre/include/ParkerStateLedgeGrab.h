
/*=============================================================================

	ParkerStateLedgeGrab.h

	Author: Matt King

	State for Parker grabbing a ledge.

=============================================================================*/
#ifndef PARKER_STATE_LEDGE_GRAB_H
#define PARKER_STATE_LEDGE_GRAB_H

#include "FSMState.h"
#include "Parker.h"

class CharacterParker;
class ParkerStateLedgeGrab : public FSMState<CharacterParker>
{

public:

	ParkerStateLedgeGrab(CharacterParker* parker);

	~ParkerStateLedgeGrab(){}

	/// This is called when the state is entered.
	virtual void Enter();

	/// Update.
	virtual bool Update();

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message);

	/// This will execute when the state is exited.
	virtual void Exit();

	/// Called when two fixtures begin to touch.
	void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

	/// Called when two fixtures cease to touch.
	void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture);

protected:


	b2Body*			 ledgeSensorBody_;
	b2RevoluteJoint* grabJoint_;

	void MoveLeft();
	void MoveRight();
	void Climb();
	void Drop();
	void UpdateAnimation();

};

#endif
