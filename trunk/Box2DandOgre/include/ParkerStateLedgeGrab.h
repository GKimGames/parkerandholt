/*=============================================================================

	ParkerStateLedgeGrab.h

	Author: Matt King

	State for Parker grabbing a ledge.

=============================================================================*/
#ifndef PARKER_STATE_LEDGE_GRAB_H
#define PARKER_STATE_LEDGE_GRAB_H

#include "Parker.h"
#include "ParkerState.h"

class CharacterParker;


///State for Parker grabbing a ledge.
class ParkerStateLedgeGrab : public ParkerState
{

public:

	
	ParkerStateLedgeGrab(CharacterParker* parker, FSMStateMachine<CharacterParker>* stateMachine);

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

	void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse);

protected:


	b2Body*				ledgeSensorBody_;
	b2DistanceJoint*	grabJoint_;

	b2Vec2				originalPosition_;
	b2Vec2				ledgePosition_;
	bool				climbing_;
	
	/// Continuously called to climb up the ledge over a period of time.
	void Climb();

	/// Fall from the ledge.
	void Drop();


	void UpdateAnimation();

};

#endif
