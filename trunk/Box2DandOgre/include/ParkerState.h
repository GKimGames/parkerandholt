/*=============================================================================

	ParkerState.h

	Author: Matt King

	State for Parker on the ground.

=============================================================================*/
#ifndef PARKER_STATE_H
#define PARKER_STATE_H

#include "FSMState.h"
#include "Message.h"

class CharacterParker;

class ParkerState : public FSMState<CharacterParker>
{

public:

	typedef FSMStateMachine<CharacterParker> ParkerStateMachine;

	ParkerState(CharacterParker* parker, ParkerStateMachine* stateMachine):
	  FSMState<CharacterParker>(parker,stateMachine){};

	~ParkerState(){}

	/// This is called when the state is entered.
	virtual void Enter()=0;

	/// Update.
	virtual bool Update()=0;

	/// This will execute when the state is exited.
	virtual void Exit()=0;

	/// Used to handle messages.
	virtual bool HandleMessage(const KGBMessage message) { return false; }

	/// Called when two fixtures begin to touch.
	virtual void BeginContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	/// Called when two fixtures cease to touch.
	virtual void EndContact(ContactPoint* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture){}

	virtual void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse){};
};

#endif
