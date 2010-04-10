/*=============================================================================

	HoltState.h

=============================================================================*/
#ifndef HOLT_STATE_H
#define HOLT_STATE_H

#include "FSMState.h"
#include "Message.h"

class CharacterHolt;

class HoltState : public FSMState<CharacterHolt>
{

public:

	typedef FSMStateMachine<CharacterHolt> HoltStateMachine;

	HoltState(CharacterHolt* holt, HoltStateMachine* stateMachine):
	  FSMState<CharacterHolt>(holt,stateMachine){};

	~HoltState(){}

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

	/// Called After the force calculations are done after a collision.
	virtual void PostSolve(b2Contact* contact, b2Fixture* contactFixture, b2Fixture* collidedFixture, const b2ContactImpulse* impulse){};
};

#endif
